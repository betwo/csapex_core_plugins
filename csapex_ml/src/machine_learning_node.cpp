#include "machine_learning_node.h"
using namespace csapex;
using namespace connection_types;

MachineLearningNode::MachineLearningNode()
{

}


void MachineLearningNode::setup(NodeModifier& node_modifier)
{
    CollectionNode<connection_types::FeaturesMessage>::setup(node_modifier);
}

void MachineLearningNode::setupParameters(Parameterizable& parameters)
{
    CollectionNode<FeaturesMessage>::setupParameters(parameters);

    parameters.addParameter(param::ParameterFactory::declareBool("perform_classification",
                                                                 param::ParameterDescription("Train a classification or a regression problem.\n"
                                                                                             "Set true for classification, false for regression problem"),
                                                                 true),
                            is_classification_);


    parameters.addParameter(csapex::param::ParameterFactory::declareFileOutputPath
                            ("file_name", "ml_config.yaml"),
                            file_name_);
}
