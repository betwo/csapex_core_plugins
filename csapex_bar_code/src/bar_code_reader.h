#ifndef BAR_CODE_READER_
#define BAR_CODE_READER_

/// COMPONENT
#include <csapex/model/node.h>

namespace csapex
{
class CSAPEX_EXPORT_PLUGIN BarCodeReader : public csapex::Node
{
public:
    BarCodeReader();

public:
    void setup(csapex::NodeModifier& node_modifier) override;
    void setupParameters(Parameterizable& params) override;

    void process() override;

private:
    Input* in_img;
    Output* out_str;
    Output* out_roi;

    std::string data_;
};

}  // namespace csapex

#endif  // BAR_CODE_READER_
