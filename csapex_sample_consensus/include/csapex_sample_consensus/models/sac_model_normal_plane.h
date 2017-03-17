#ifndef SAC_MODEL_NORMAL_PLANE_H
#define SAC_MODEL_NORMAL_PLANE_H

/// PROJECT
#include "sac_model_from_normals.h"

/// SYSTEM
#include <pcl/point_types.h>
#include <pcl/common/common.h>

namespace csapex_sample_consensus {
namespace models {
template<typename PointT, typename NormalT>
class NormalPlane : public ModelFromNormals<PointT, NormalT> {
public:
    using PointCloud = pcl::PointCloud<PointT>;
    using NormalCloud = pcl::PointCloud<NormalT>;
    using Base = Model<PointT>;

    NormalPlane(const typename PointCloud::ConstPtr &pointcloud,
                     const typename NormalCloud::ConstPtr &normalcloud,
                     const float normal_distance_weight = 0.f);

    virtual typename Base::Ptr clone() const override;

    virtual bool isModelValid() const override;

    virtual bool validateSamples(const std::vector<int> &indices) const override;

    virtual std::size_t getModelDimension() const override;

    virtual double getDistanceToModel(const int &index) const override;

    virtual void getDistancesToModel(const std::vector<int> &indices,
                                     std::vector<float> &distances) const override;

protected:
    virtual bool doComputeModelCoefficients(const std::vector<int> &indices) override;
    inline float dot(const PointT &p) const;
};
}
}




#endif // SAC_MODEL_NORMAL_PLANE_H
