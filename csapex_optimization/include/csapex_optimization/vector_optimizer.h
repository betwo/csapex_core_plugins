#ifndef VECTOR_OPTIMIZER_H
#define VECTOR_OPTIMIZER_H

#include <condition_variable>
#include <mutex>
#include <thread>
/// PROJECT
#include <csapex/model/node.h>
#include <csapex/model/node_handle.h>
#include <csapex/model/node_modifier.h>
#include <csapex/model/token.h>
#include <csapex/msg/end_of_sequence_message.h>
#include <csapex/msg/generic_vector_message.hpp>
#include <csapex/msg/io.h>
#include <csapex/param/parameter_factory.h>
#include <csapex/signal/event.h>
#include <csapex/utility/exceptions.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex_optimization/optimization_params.h>
#include <random>

namespace csapex
{
class VectorOptimizer : public Node
{
public:
    VectorOptimizer();
    virtual ~VectorOptimizer() = default;

    void setup(csapex::NodeModifier& modifier) override;
    void setupParameters(csapex::Parameterizable& params) override;

    virtual void start() = 0;
    virtual void stop() = 0;

    void createStart();
    void tearDown() override;
    bool canProcess() const override;
    void process() override;

    void residualCb(const TokenPtr& value);
    void gradCb(const TokenPtr& grad);

    void startParamCb(const TokenPtr& msg);
    void nextParams();
    bool canNotifiy();

    double realMinfunc(const std::vector<double>& x, std::vector<double>& grad);

protected:
    void publishOptimalParameters();
    void publishMinimumResidual();
    void setDone();

protected:
    bool is_running_;
    bool has_new_parameter_;
    bool has_residual_;
    bool has_grad_;
    bool trigger_immediately_;
    bool use_grad_;
    int wait_time_ms_;
    std::size_t fitness_calls_;
    std::size_t problem_dim_;
    Slot* in_;
    Slot* in_grad_;
    Slot* in_start_;
    Event* evaluate_;
    Event* solution_;
    Event* min_residual_;
    Output* out_;
    double residual_;
    double minimum_residual_;
    std::thread worker_;
    std::mutex data_available_mutex_;
    std::condition_variable data_available_;
    std::vector<double> to_optimize_;
    std::vector<double> gradient_;
    std::vector<double> ext_start_params_;
    OptimizationParams params_;
};
}  // namespace csapex
#endif  // VECTOR_OPTIMIZER_H
