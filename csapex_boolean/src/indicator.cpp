/// HEADER
#include "indicator.h"

/// PROJECT
#include <csapex/model/connector_out.h>
#include <csapex/model/connector_in.h>
#include <utils_param/parameter_factory.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/model/node_modifier.h>

CSAPEX_REGISTER_CLASS(csapex::boolean::Indicator, csapex::Node)

using namespace csapex;
using namespace csapex::boolean;
using namespace csapex::connection_types;

Indicator::Indicator()
{
    addTag(Tag::get("Boolean"));

    addParameter(param::ParameterFactory::declareBool("signal", true));
}

void Indicator::setup()
{
    in = modifier_->addInput<DirectMessage<bool> >("Signal");
}

void Indicator::process()
{
    DirectMessage<bool>::Ptr a = in->getMessage<DirectMessage<bool> >();
    getParameter("signal")->set<bool>(a->value);
}
