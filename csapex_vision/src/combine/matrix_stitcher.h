#ifndef IMAGE_STITCHER_H
#define IMAGE_STITCHER_H

/// PROJECT
#include <csapex/model/node.h>

namespace csapex
{
class MatrixStitcher : public csapex::Node
{
public:
    MatrixStitcher();

    void process() override;
    void setup(csapex::NodeModifier& node_modifier) override;
    void setupParameters(Parameterizable& parameters) override;

protected:
    enum Mode
    {
        HORIZONTAL,
        VERTICAL
    };

    csapex::Input* matrix_1_;
    csapex::Input* matrix_2_;
    csapex::Output* stitched_;
};
}  // namespace csapex
#endif  // IMAGE_STITCHER_H
