/// HEADER
#include "pointcloud_validity.h"

/// PROJECT
#include <csapex/model/connector_in.h>
#include <csapex/model/connector_out.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>
#include <csapex_point_cloud/indeces_message.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/model/node_modifier.h>

/// SYSTEM
#include <pcl/point_types.h>
#include <pcl/conversions.h>

CSAPEX_REGISTER_CLASS(csapex::PointCloudValidity, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

PointCloudValidity::PointCloudValidity()
{
}

void PointCloudValidity::process()
{
    PointCloudMessage::Ptr msg(input_->getMessage<PointCloudMessage>());

    boost::apply_visitor (PointCloudMessage::Dispatch<PointCloudValidity>(this), msg->value);
}

void PointCloudValidity::setup()
{
    input_  = modifier_->addInput<PointCloudMessage>("PointCloud");
    mask_   = modifier_->addOutput<CvMatMessage>("Mask");
    index_  = modifier_->addOutput<PointIndecesMessage>("Indeces");
}

namespace {

template <typename Any>
class PointTraits
{
    typedef char Small;
    class Big
    {
        char dummy[2];
    };

    template <typename Class> static Small testx(typeof(&Class::x)) ;
    template <typename Class> static Big testx(...);

    template <typename Class> static Small testy(typeof(&Class::y)) ;
    template <typename Class> static Big testy(...);

    template <typename Class> static Small testz(typeof(&Class::z)) ;
    template <typename Class> static Big testz(...);

public:
    enum { HasX = sizeof(testx<Any>(0)) == sizeof(Small) };
    enum { HasY = sizeof(testy<Any>(0)) == sizeof(Small) };
    enum { HasZ = sizeof(testz<Any>(0)) == sizeof(Small) };
    enum { HasXY  = HasX && HasY && !HasZ};
    enum { HasXYZ = HasX && HasY &&  HasZ};
};

template<typename PointT>
inline static bool invalid(const PointT &p,
                           typename boost::enable_if_c<PointTraits<PointT>::HasXY, PointT>::type* dummy = 0)
{
    bool nan  = p.x != p.x || p.y != p.y;
    bool zero = p.x == 0 && p.y == 0;
    return nan || zero;
}

template<typename PointT>
inline static bool invalid(const PointT &p,
                           typename boost::enable_if_c<PointTraits<PointT>::HasXYZ, PointT>::type* dummy = 0)
{
    bool nan  = p.x != p.x || p.y != p.y || p.z != p.z;
    bool zero = p.x == 0 && p.y == 0 && p.z == 0;
    return nan || zero;

}

template<typename PointT>
struct Vadility {
    inline static void inceces(const typename pcl::PointCloud<PointT>::Ptr &src,
                               std::vector<int> &indeces)
    {

        PointT  *src_ptr = src->points.data();
        unsigned int size = src->size();
        for(unsigned int i(0) ; i < size ; ++i) {
            if(invalid(src_ptr[i]))
                indeces.push_back(i);
        }
    }

    inline static void mask(const typename pcl::PointCloud<PointT>::Ptr &src,
                              cv::Mat &mask)
    {

        PointT *src_ptr = src->points.data();
        int cols = src->width;
        int rows = src->height;
        mask   = cv::Mat(rows, cols, CV_8UC1, 255);
        uchar *mask_ptr = mask.ptr<uchar>();
        for(int y = 0 ; y < rows ; ++y) {
            for(int x = 0 ; x < cols ; ++x) {
                int pos = y * cols + x;
                if(invalid(src_ptr[pos])) {
                    mask_ptr[pos] = 0;
                }
            }
        }
    }

};
}

template <class PointT>
void PointCloudValidity::inputCloud(typename pcl::PointCloud<PointT>::Ptr cloud)
{
    if(mask_->isConnected()) {
        CvMatMessage::Ptr out(new CvMatMessage(enc::mono));
        Vadility<PointT>::mask(cloud, out->value);
        mask_->publish(out);
    }
    if(index_->isConnected()) {
        PointIndecesMessage::Ptr out(new PointIndecesMessage);
        out->value->header = cloud->header;
        Vadility<PointT>::inceces(cloud, out->value->indices);
        index_->publish(out);
    }
}
