/// HEADER
#include "equalize.h"

/// PROJECT
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/msg/io.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>
#include <utils_vision/utils/histogram.hpp>
#include <csapex/model/node_modifier.h>

using namespace csapex;
using namespace csapex::connection_types;
using namespace vision_plugins;

CSAPEX_REGISTER_CLASS(vision_plugins::Equalize, csapex::Node)

Equalize::Equalize()
{
}

void Equalize::process()
{
    CvMatMessage::ConstPtr in = msg::getMessage<connection_types::CvMatMessage>(input_);
    CvMatMessage::Ptr out(new connection_types::CvMatMessage(in->getEncoding(), in->stamp_micro_seconds));
    out->value    = in->value.clone();
    utils_vision::histogram::full_channel_equalize(out->value, out->value);
    msg::publish(output_, out);
}

void Equalize::setup()
{
    input_ = modifier_->addInput<CvMatMessage>("original");
    output_ = modifier_->addOutput<CvMatMessage>("equalized");
}
