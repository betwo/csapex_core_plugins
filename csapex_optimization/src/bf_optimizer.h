#ifndef BF_OPTIMIZER_H
#define BF_OPTIMIZER_H

/// COMPONENT
#include <csapex_optimization/optimizer.h>

/// SYSTEM

namespace csapex
{
class BFOptimizer : public Optimizer
{
    friend class BFOptimizerAdapter;

public:
    BFOptimizer();

    int stepsNecessary();
    void reset() override;

public:
    slim_signal::Signal<void(int)> step;

protected:
    bool generateNextParameterSet() override;

private:
    void doStop() override;

    bool increaseParameter(std::size_t i);

private:
    std::vector<int> current_index_;

    int step_;
};

}  // namespace csapex

#endif  // BF_OPTIMIZER_H
