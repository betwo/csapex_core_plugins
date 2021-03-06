#ifndef CONFIDENCE_MATRIX_DISPLAY_H
#define CONFIDENCE_MATRIX_DISPLAY_H

/// COMPONENT
#include <csapex_evaluation/confidence_matrix_message.h>

/// PROJECT
#include <csapex/model/node.h>

namespace csapex
{
class ConfidenceMatrixDisplay : public Node
{
public:
    ConfidenceMatrixDisplay();

    void process() override;
    void setup(csapex::NodeModifier& node_modifier) override;

    const ConfidenceMatrix& getConfidenceMatrix() const;

public:
    slim_signal::Signal<void()> display_request;

private:
    Input* connector_;

    ConfidenceMatrix confidence_;
};

}  // namespace csapex

#endif  // CONFIDENCE_MATRIX_DISPLAY_H
