#ifndef INDEX_FILTER_H
#define INDEX_FILTER_H

/// PROJECT
#include <csapex/model/node.h>
#include <csapex_point_cloud/msg/point_cloud_message.h>

namespace csapex
{
class IndexFilter : public Node
{
public:
    IndexFilter();

    void setup(csapex::NodeModifier& node_modifier) override;
    void process() override;

    template <class PointT>
    void inputCloud(typename pcl::PointCloud<PointT>::ConstPtr cloud);

protected:
    Input* input_cloud_;
    Input* indices_input_;
    Output* output_cloud_;
};
}  // namespace csapex
#endif  // INDEX_FILTER_H
