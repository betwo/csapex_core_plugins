#ifndef FILE_IMPORTER_H
#define FILE_IMPORTER_H

/// PROJECT
#include <csapex/model/message_provider.h>
#include <csapex/model/node.h>

namespace csapex
{

class ConnectorOut;

class FileImporter : public Node
{
public:
    FileImporter();
    ~FileImporter();

    void setup();

    virtual QIcon getIcon() const;

    void import();

    void process();

    void tick();

    bool doImport(const QString& path);

private:
    void changeMode();

private:
    MessageProvider::Ptr provider_;

    QString file_;
    ConnectorOut* output_;
};

}

#endif // FILE_IMPORTER_H
