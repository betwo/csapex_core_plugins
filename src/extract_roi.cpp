/// HEADER
#include "extract_roi.h"

/// PROJECT
#include <csapex_core_plugins/vector_message.h>
#include <csapex_vision/cv_mat_message.h>
#include <csapex_vision/roi_message.h>

/// PROJECT
#include <csapex/model/connector_out.h>
#include <csapex/model/connector_in.h>
#include <utils_param/parameter_factory.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>
#include <boost/foreach.hpp>

CSAPEX_REGISTER_CLASS(csapex::ExtractROI, csapex::Node)

using namespace csapex;
using namespace connection_types;

ExtractROI::ExtractROI()
{
    addTag(Tag::get("Vision"));
    addTag(Tag::get("ROI"));

    addParameter(param::ParameterFactory::declare<int>("thickness", 1, 20, 1, 1));
}

void ExtractROI::allConnectorsArrived()
{
    CvMatMessage::Ptr img = input_img_->getMessage<CvMatMessage>();
    RoiMessage::Ptr roi = input_roi_->getMessage<RoiMessage>();

    CvMatMessage::Ptr out(new CvMatMessage(img->getEncoding()));

    cv::Mat(img->value, roi->value.rect()).copyTo(out->value);

    output_->publish(out);
}

void ExtractROI::setup()
{
    setSynchronizedInputs(true);

    input_img_ = addInput<CvMatMessage>("Image");
    input_roi_ = addInput<RoiMessage >("ROI");

    output_ = addOutput<CvMatMessage>("SubImage");
}
