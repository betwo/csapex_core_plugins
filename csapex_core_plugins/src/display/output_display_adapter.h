#ifndef OUTPUT_DISPLAY_ADAPTER_H
#define OUTPUT_DISPLAY_ADAPTER_H

/// PROJECT
#include <csapex/view/node/resizable_node_adapter.h>
#include <csapex_core_plugins/image_widget.h>
#include <csapex/utility/yaml.h>

/// COMPONENT
#include "output_display.h"

namespace csapex
{
class ImageWidget;

class OutputDisplayDirectAdapter : public QObject, public ResizableNodeAdapter
{
    Q_OBJECT

public:
    OutputDisplayDirectAdapter(NodeFacadeImplementationPtr worker, NodeBox* parent, std::weak_ptr<OutputDisplay> instance);
    ~OutputDisplayDirectAdapter();

    void setupUi(QBoxLayout* layout) override;

    void setManualResize(bool manual) override;

public Q_SLOTS:
    void display(const QImage& img);
    void fitInView();

Q_SIGNALS:
    void displayRequest(QImage img);

protected:
    bool eventFilter(QObject* o, QEvent* e) override;
    void resize(const QSize& size) override;

private:
    QSize last_image_size_;

    ImageWidget* label_view_;
};
}  // namespace csapex

#endif  // OUTPUT_DISPLAY_ADAPTER_H
