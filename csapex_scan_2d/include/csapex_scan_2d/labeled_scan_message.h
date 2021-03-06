#ifndef DESCRIPTOR_MESSAGE_H
#define DESCRIPTOR_MESSAGE_H

/// COMPONENT
#include <csapex_scan_2d/binary_io.h>
#include <cslibs_laser_processing/data/labeled_scan.h>

/// PROJECT
#include <csapex/msg/message_template.hpp>

namespace csapex
{
namespace connection_types
{
struct LabeledScanMessage : public MessageTemplate<lib_laser_processing::LabeledScan, LabeledScanMessage>
{
    LabeledScanMessage();
};

/// TRAITS
template <>
struct type<LabeledScanMessage>
{
    static std::string name()
    {
        return "LabeledScan";
    }
};
}  // namespace connection_types

}  // namespace csapex

/// YAML
namespace YAML
{
template <>
struct convert<csapex::connection_types::LabeledScanMessage>
{
    static Node encode(const csapex::connection_types::LabeledScanMessage& rhs);
    static bool decode(const Node& node, csapex::connection_types::LabeledScanMessage& rhs);
};
}  // namespace YAML

#endif  // DESCRIPTOR_MESSAGE_H
