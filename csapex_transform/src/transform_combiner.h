#ifndef TRANSFORM_COMBINER_H
#define TRANSFORM_COMBINER_H

/// PROJECT
#include <csapex/model/node.h>

namespace csapex
{
class TransformCombiner : public csapex::Node
{
public:
    TransformCombiner();

    void process() override;
    void setup(NodeModifier& node_modifier) override;
    void setupParameters(Parameterizable& params) override;

private:
    Output* output_;

    Input* input_a_;
    Input* input_b_;

    int which_stamp_;
};

}  // namespace csapex

#endif  // TRANSFORM_COMBINER_H
