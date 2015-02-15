/// HEADER
#include "float_to_uchar.h"

/// PROJECT
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/msg/io.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>
#include <utils_cv/normalization.hpp>
#include <csapex/model/node_modifier.h>

/// SYSTEM
#include <boost/assign.hpp>

using namespace csapex;
using namespace csapex::connection_types;
using namespace vision_plugins;

CSAPEX_REGISTER_CLASS(vision_plugins::FloatToUchar, csapex::Node)

#warning "Temporary implementation fix with fixing the channel encoding!"

FloatToUchar::FloatToUchar() :
    type_(RELATIVE)
{
}

void FloatToUchar::process()
{
    CvMatMessage::ConstPtr in = msg::getMessage<connection_types::CvMatMessage>(input_);
    CvMatMessage::Ptr out(new connection_types::CvMatMessage(enc::mono, in->stamp_micro_seconds));
    out->value = in->value.clone();



//    switch(type_) {
//    case RELATIVE:
//        utils_cv::normalizeRelative<uchar>(out->value);
//        break;
//    case ABSOLUTE:
//        if((in->value.type() & 7) == CV_32F)
//            utils_cv::normalizeAbsolute<float, uchar>(out->value);
//        else
//            utils_cv::normalizeAbsolute<double, uchar>(out->value);
//        break;
//    case SCALE:
//            utils_cv::scale<uchar>(out->value);
//        break;
//    default:
//        throw std::runtime_error("Cannot perfom this operation!");
//    }

//    out->value *= 255.0;
    out->value.convertTo(out->value, CV_8UC(in->value.channels()));
    msg::publish(output_, out);
}

void FloatToUchar::setup()
{
    input_ = modifier_->addInput<CvMatMessage>("float");
    output_ = modifier_->addOutput<CvMatMessage>("uchar");

    update();
}

void FloatToUchar::setupParameters()
{
    std::map<std::string, int> types = boost::assign::map_list_of
            ("RELATIVE", RELATIVE)
            ("ABSOLUTE", ABSOLUTE)
            ("SCALE",    SCALE);

    addParameter(param::ParameterFactory::declareParameterSet("type", types, (int) RELATIVE),
                 std::bind(&FloatToUchar::update, this));
}

void FloatToUchar::update()
{
    type_ = (Type) readParameter<int>("type");
}
