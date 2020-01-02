#ifndef TEXT_DISPLAY_H_
#define TEXT_DISPLAY_H_

/// PROJECT
#include <csapex/model/node.h>

/// SYSTEM
#include <future>

namespace csapex
{
class CSAPEX_EXPORT_PLUGIN SayText : public Node
{
public:
    SayText();

    void process() override;
    void setup(csapex::NodeModifier& node_modifier) override;
    void setupParameters(Parameterizable& parameters) override;

private:
    Input* connector_;

    bool repeat_;
    bool async_;
    std::string last_;

    bool speaking_;

    std::string language_;
    int pitch_;
    int speed_;

    std::future<void> future_;
};

}  // namespace csapex

#endif  // TEXT_DISPLAY_H_
