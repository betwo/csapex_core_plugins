/// HEADER
#include "blur.h"

/// PROJECT
#include <csapex/model/connector_in.h>
#include <csapex/model/connector_out.h>
#include <csapex_vision/cv_mat_message.h>
#include <utils_param/parameter_factory.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/register_apex_plugin.h>


CSAPEX_REGISTER_CLASS(csapex::BoxBlur, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

BoxBlur::BoxBlur()
{
    addTag(Tag::get("Filter"));
    addTag(Tag::get("Vision"));
}

void BoxBlur::process()
{
    CvMatMessage::Ptr in = input_->getMessage<connection_types::CvMatMessage>();
    CvMatMessage::Ptr out(new connection_types::CvMatMessage(in->getEncoding()));

    int kernel = param<int>("kernel");

    cv::blur(in->value,out->value, cv::Size(kernel, kernel));

    output_->publish(out);
}

void BoxBlur::setup()
{
    input_ = modifier_->addInput<CvMatMessage>("Unblurred");
    output_ = modifier_->addOutput<CvMatMessage>("Blurred");
}

void BoxBlur::setupParameters()
{
    addParameter(param::ParameterFactory::declareRange("kernel", 1, 255, 2, 2));
}
