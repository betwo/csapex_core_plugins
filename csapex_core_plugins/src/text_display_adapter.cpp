/// HEADER
#include "text_display_adapter.h"

/// PROJECT
#include <csapex/msg/input.h>
#include <csapex/utility/register_node_adapter.h>

using namespace csapex;

CSAPEX_REGISTER_NODE_ADAPTER(TextDisplayAdapter, csapex::TextDisplay)


TextDisplayAdapter::TextDisplayAdapter(NodeWorker *worker, TextDisplay *node, WidgetController* widget_ctrl)
    : NodeAdapter(worker, widget_ctrl), wrapped_(node)
{
    // translate to UI thread via Qt signal
    node->display_request.connect(boost::bind(&TextDisplayAdapter::displayRequest, this, _1));
}


void TextDisplayAdapter::setupUi(QBoxLayout* layout)
{
    txt_ = new QLabel;
    layout->addWidget(txt_);

    connect(this, SIGNAL(displayRequest(std::string)), this, SLOT(display(std::string)));
}

void TextDisplayAdapter::display(const std::string& txt)
{
    txt_->setMaximumWidth(txt_->parentWidget()->width());
    txt_->setText(QString::fromStdString(txt));
}
