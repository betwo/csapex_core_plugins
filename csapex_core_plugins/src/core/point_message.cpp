#include <csapex_core_plugins/point_message.h>

/// PROJECT
#include <csapex/serialization/io/csapex_io.h>
#include <csapex/serialization/io/std_io.h>
#include <csapex/utility/assert.h>
#include <csapex/utility/register_msg.h>

CSAPEX_REGISTER_MESSAGE(csapex::connection_types::PointMessage)

using namespace csapex;
using namespace connection_types;

PointMessage::PointMessage(Message::Stamp stamp) : Message(type<PointMessage>::name(), "/", stamp)
{
}

PointMessage::PointMessage() : Message(type<PointMessage>::name(), "/", 0)
{
}

void PointMessage::serialize(SerializationBuffer& data, SemanticVersion& version) const
{
    Message::serialize(data, version);
    data << x << y;
}
void PointMessage::deserialize(const SerializationBuffer& data, const SemanticVersion& version)
{
    Message::deserialize(data, version);
    data >> x >> y;
}

/// YAML
namespace YAML
{
Node convert<csapex::connection_types::PointMessage>::encode(const csapex::connection_types::PointMessage& rhs)
{
    Node node;
    node["x"] = rhs.x;
    node["y"] = rhs.y;
    return node;
}

bool convert<csapex::connection_types::PointMessage>::decode(const Node& node, csapex::connection_types::PointMessage& rhs)
{
    if (!node.IsMap()) {
        return false;
    }
    rhs.x = node["x"].as<float>();
    rhs.y = node["y"].as<float>();
    return true;
}
}  // namespace YAML
