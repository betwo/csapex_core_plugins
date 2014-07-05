/// HEADER
#include <csapex_scan_2d/scan_message.h>

/// PROJECT
#include <csapex/utility/assert.h>

using namespace csapex;
using namespace connection_types;
using namespace lib_laser_processing;

ScanMessage::ScanMessage()
    : MessageTemplate<lib_laser_processing::Scan, ScanMessage> ("/")
{}

void ScanMessage::writeYaml(YAML::Emitter &yaml) const
{
    yaml << YAML::Flow << YAML::Key << "ranges" << YAML::Value;
    yaml << YAML::BeginSeq;
    std::vector<LaserBeam>::const_iterator r = value.rays.begin();
    for(; r != value.rays.end(); ++r) {
        yaml << r->range;
    }
    yaml << YAML::EndSeq;
}

void ScanMessage::readYaml(const YAML::Node &node)
{
    apex_assert_hard(node.Type() == YAML::NodeType::Sequence);
}
