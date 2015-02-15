/// HEADER
#include "generic_image_combiner.h"

/// PROJECT
#include <csapex_vision/cv_mat_message.h>
#include <csapex/msg/io.h>
#include <utils_param/parameter_factory.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/assert.h>

/// SYSTEM
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted.hpp>

CSAPEX_REGISTER_CLASS(vision_plugins::GenericImageCombiner, csapex::Node)

using namespace csapex;
using namespace connection_types;
using namespace vision_plugins;

BOOST_FUSION_ADAPT_STRUCT(VariableExpression, (std::string, name_))


namespace qi    = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx   = boost::phoenix;

template <typename Iterator>
struct ExpressionParser : qi::grammar<Iterator, Expression(), ascii::space_type>
{
    struct MakeFunctionExpression {
        template<typename,typename> struct result { typedef FunctionExpression type; };

        template<typename C, typename A>
        FunctionExpression operator()(C op, A const& arg) const
        { return FunctionExpression(op, arg); }
    };
    struct MakeBinaryExpression {
        template<typename,typename,typename> struct result { typedef BinaryExpression type; };

        template<typename C, typename L, typename R>
        BinaryExpression operator()(C op, L const& lhs, R const& rhs) const
        { return BinaryExpression(op, lhs, rhs); }
    };
    struct MakeUnaryExpression {
        template<typename,typename> struct result { typedef UnaryExpression type; };

        template<typename C, typename R>
        UnaryExpression operator()(C op, R const& rhs) const
        { return UnaryExpression(op, rhs); }
    };

    phx::function<MakeBinaryExpression> makebinary;
    phx::function<MakeUnaryExpression> makeunary;
    phx::function<MakeFunctionExpression> makefun;

    ExpressionParser() : ExpressionParser::base_type(expression)
    {
        using namespace qi;
        using qi::_1;
        using qi::_2;

        expression =
                bi_expr                                   [ _val = _1]
                ;

        expression_list =
                expression % ','
                ;

        bi_expr =
                primary_expr                              [ _val = _1 ]
                >> *(char_("+|&*/^-") >> primary_expr)    [ _val = makebinary(_1, _val, _2)]
                ;

        un_expr =
                *(char_("~") >> primary_expr)             [ _val = makeunary(_1, _2)]
                ;

        primary_expr =
                  function_call                           [ _val = _1 ]
                | ( '(' > expression > ')' )              [ _val = _1 ]
                | ( '|' > expression > '|' )              [ _val = makefun("abs", _1) ]
                | constant                                [ _val = _1 ]
                | variable                                [ _val = _1 ]
                ;

        function_call =
                ( +char_("a-zA-Z0-9") >> '(' >> expression_list >> ')' )
                                                          [ _val = makefun(_1, _2) ];
        constant = double_ | int_;
        variable = '$' >> lexeme [ +char_("a-zA-Z0-9") ];

        BOOST_SPIRIT_DEBUG_NODE(expression);
        BOOST_SPIRIT_DEBUG_NODE(bi_expr);

        BOOST_SPIRIT_DEBUG_NODE(primary_expr);
        BOOST_SPIRIT_DEBUG_NODE(constant);
        BOOST_SPIRIT_DEBUG_NODE(variable);
    }

    qi::rule<Iterator, Expression(), ascii::space_type> expression;
    qi::rule<Iterator, Expression(), ascii::space_type> bi_expr;
    qi::rule<Iterator, Expression(), ascii::space_type> un_expr;
    qi::rule<Iterator, std::vector<Expression>(), ascii::space_type> expression_list;

    qi::rule<Iterator, Expression(), ascii::space_type> primary_expr;
    qi::rule<Iterator, ConstantExpression(), ascii::space_type> constant;
    qi::rule<Iterator, VariableExpression(), ascii::space_type> variable;
    qi::rule<Iterator, FunctionExpression(), ascii::space_type> function_call;
    qi::rule<Iterator, std::string() , ascii::space_type> string;
};

GenericImageCombiner::GenericImageCombiner()
{
}

void GenericImageCombiner::updateFormula()
{
    std::string script = readParameter<std::string>("script");

    std::string::const_iterator iter = script.begin();
    std::string::const_iterator end = script.end();

    ExpressionParser<std::string::const_iterator> p;

    bool r = qi::phrase_parse(iter,end,p,ascii::space,e);

    if (r && iter == end) {
        modifier_->setNoError();

    } else {
        std::string rest(iter, end);
        throw std::runtime_error(std::string("Parsing failed at: ") + rest);
    }
}

void GenericImageCombiner::process()
{
    if(!e.valid()) {
        return;
    }


    CvMatMessage::ConstPtr img1 = msg::getMessage<CvMatMessage>(i1_);

    cv::Mat f1;
    img1->value.copyTo(f1);

    if(f1.channels() == 1) {
        f1.convertTo(f1, CV_32FC1);
    } else if(f1.channels() == 3) {
        f1.convertTo(f1, CV_32FC3);
    } else {
        std::stringstream msg;
        msg << "Image 1: images with " << f1.channels() << " channels not yet supported";
        throw std::runtime_error(msg.str());
    }

    VariableMap vm;
    vm.get("1") = f1;

    // handle optional second image
    if(msg::hasMessage(i2_)) {
        cv::Mat f2;
        CvMatMessage::ConstPtr img2 = msg::getMessage<CvMatMessage>(i2_);
        img2->value.copyTo(f2);

        if(f1.channels() != f2.channels()) {
            throw std::runtime_error("images have different number of channels.");
        }

        if(f2.channels() == 1) {
            f2.convertTo(f2, CV_32FC1);
        } else if(f2.channels() == 3) {
            f2.convertTo(f2, CV_32FC3);
        } else {
            std::stringstream msg;
            msg << "Image 2: images with " << f2.channels() << " channels not yet supported";
            throw std::runtime_error(msg.str());
        }

        vm.get("2") = f2;
    } else {
        vm.get("2") = cv::Mat();
    }

    CvMatMessage::Ptr out(new CvMatMessage(img1->getEncoding(), img1->stamp_micro_seconds));

    cv::Mat r = e.evaluate(vm);
    if(f1.channels() == 1) {
        r.convertTo(out->value, CV_8UC1);
    } else if(f1.channels() == 3) {
        r.convertTo(out->value, CV_8UC3);
    }
    msg::publish(out_, out);
}

void GenericImageCombiner::setup()
{
    i1_ = modifier_->addInput<CvMatMessage>("image 1");
    i2_ = modifier_->addOptionalInput<CvMatMessage>("image 2");
    out_ = modifier_->addOutput<CvMatMessage>("combined");
}

void GenericImageCombiner::setupParameters()
{
    addParameter(param::ParameterFactory::declareText("script", "$1 ^ $2"),
                 std::bind(&GenericImageCombiner::updateFormula, this));
}
