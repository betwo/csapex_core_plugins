#ifndef EXTRACTORS_DEFAULT_HPP
#define EXTRACTORS_DEFAULT_HPP

/// COMPONENT
#include <csapex_vision_features/extractor_manager.h>

/// PROJECT
#include <csapex/param/parameter_factory.h>

/// SYSTEM
#include <opencv2/features2d/features2d.hpp>
#if CV_NON_FREE
#include <opencv2/nonfree/nonfree.hpp>
#endif
#include <boost/bind.hpp>
#include <boost/static_assert.hpp>

using namespace csapex;
using namespace param;

/// COMBINATIONS

struct Orb : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
            add(ParameterFactory::declareRange("orb/extractor_threshold", 0, 200, 50, 1));
            add(ParameterFactory::declareRange("orb/levels", 1, 20, 8, 1));
            add(ParameterFactory::declareRange("orb/edgeThreshold", 0, 64, 0, 1));
            add(ParameterFactory::declareRange("orb/first_level", 0, 8, 0, 1));
            add(ParameterFactory::declareRange("orb/WTA_K", 2, 4, 2, 1));
            add(ParameterFactory::declareRange("orb/patch_size", 0, 128, 31, 1));

            add(ParameterFactory::declareRange("orb/scale", 0.5, 2.0, 1.2, 0.05));

            std::map<std::string, int> set;
            set["ORB::HARRIS_SCORE"] = cv::ORB::HARRIS_SCORE;
            set["ORB::FAST_SCORE"] = cv::ORB::FAST_SCORE;
            add(ParameterFactory::declareParameterSet("orb/type", set, (int)cv::ORB::HARRIS_SCORE));
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void keypoint(Extractor* e, const csapex::param::ParameterProvider& param, bool complete)
    {
        int et = params().read<int>(param, "orb/extractor_threshold");
        double scale = params().read<double>(param, "orb/scale");
        int levels = params().read<int>(param, "orb/levels");
        int edge = params().read<int>(param, "orb/edgeThreshold");
        int first_level = params().read<int>(param, "orb/first_level");
        int WTA_K = params().read<int>(param, "orb/WTA_K");
        int patch_size = params().read<int>(param, "orb/patch_size");
        int type = params().read<int>(param, "orb/type");

        e->keypoint = "orb";
        e->has_orientation = true;
#if CV_MAJOR_VERSION == 2
        e->detector = new cv::ORB((200 - et) * 10, scale, levels, edge, first_level, WTA_K, type, patch_size);
#elif CV_MAJOR_VERSION == 3
        e->detector = cv::ORB::create((200 - et) * 10, scale, levels, edge, first_level, WTA_K, type, patch_size);
#elif CV_MAJOR_VERSION > 3
        e->detector = cv::ORB::create((200 - et) * 10, scale, levels, edge, first_level, WTA_K, cv::ORB::ScoreType(type), patch_size);
#endif

        if (complete) {
            e->is_binary = true;
            e->descriptor = "orb";
            e->descriptor_extractor = e->detector;
        }
    }

    static void descriptor(Extractor* e, const csapex::param::ParameterProvider& param)
    {
        e->is_binary = true;
        e->descriptor = "orb";
#if CV_MAJOR_VERSION == 2
        e->descriptor_extractor = new cv::ORB();
#elif CV_MAJOR_VERSION >= 3
        e->descriptor_extractor = cv::ORB::create();
#endif
    }
};
REGISTER_FEATURE_DETECTOR(Orb, ORB)
static_assert(DetectorTraits<Orb>::HasKeypoint);
static_assert(DetectorTraits<Orb>::HasDescriptor);

struct Brisk : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
            add(ParameterFactory::declareRange("brisk/extractor_threshold", 0, 1000, 50, 1));
            add(ParameterFactory::declareRange("brisk/octaves", 0, 10, 4, 1));
            add(ParameterFactory::declareRange("brisk/pattern_scale", 0.2, 10.0, 2.0, 0.1));
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void keypoint(Extractor* e, const csapex::param::ParameterProvider& param, bool complete)
    {
        int et = params().read<int>(param, "brisk/extractor_threshold");
        int octaves = params().read<int>(param, "brisk/octaves");
        double scale = params().read<double>(param, "brisk/pattern_scale");

        e->keypoint = "brisk";
        e->has_orientation = true;
#if CV_MAJOR_VERSION == 2
        e->detector = new cv::BRISK(et, octaves, scale);
#elif CV_MAJOR_VERSION >= 3
        e->descriptor_extractor = cv::BRISK::create(et, octaves, scale);
#endif
        if (complete) {
            e->is_binary = true;
            e->descriptor = "brisk";
            e->descriptor_extractor = e->detector;
        }
    }

    static void descriptor(Extractor* e, const csapex::param::ParameterProvider& param)
    {
        int et = params().read<int>(param, "brisk/extractor_threshold");
        int octaves = params().read<int>(param, "brisk/octaves");

        e->is_binary = true;
        e->descriptor = "brisk";
#if CV_MAJOR_VERSION == 2
        e->descriptor_extractor = new cv::BRISK(et, octaves);
#elif CV_MAJOR_VERSION >= 3
        e->descriptor_extractor = cv::BRISK::create(et, octaves);
#endif
    }
};
REGISTER_FEATURE_DETECTOR(Brisk, BRISK)
static_assert(DetectorTraits<Brisk>::HasKeypoint);
static_assert(DetectorTraits<Brisk>::HasDescriptor);

#if CV_NON_FREE
struct Sift : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
            add(ParameterFactory::declareRange("sift/extractor_threshold", 0, 1000, 50, 1));
            add(ParameterFactory::declareRange("sift/nOctaveLayers", 0, 5, 3, 1));
            add(ParameterFactory::declareRange("sift/contrastThreshold", 0.0, 1.0, 0.04, 0.005));
            add(ParameterFactory::declareRange("sift/edgeThreshold", 0.0, 100.0, 10.0, 1.0));
            add(ParameterFactory::declareRange("sift/sigma", 0.0, 5.0, 1.6, 0.01));
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }
    static void keypoint(Extractor* e, const csapex::param::ParameterProvider& param, bool complete)
    {
        int et = params().read<int>(param, "sift/extractor_threshold");
        int nOctaveLayers = params().read<int>(param, "sift/nOctaveLayers");
        double contrastThreshold = params().read<double>(param, "sift/contrastThreshold");
        double edgeThreshold = params().read<double>(param, "sift/edgeThreshold");
        double sigma = params().read<double>(param, "sift/sigma");
        e->keypoint = "sift";
        e->has_orientation = true;
        e->detector = new cv::SiftFeatureDetector(et * 0.002 / 3, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);

        if (complete) {
            e->is_binary = false;
            e->descriptor = "sift";
            e->descriptor_extractor = e->detector;
        }
    }

    static void descriptor(Extractor* e, const csapex::param::ParameterProvider& param)
    {
        e->is_binary = false;
        e->descriptor = "sift";
        e->descriptor_extractor = new cv::SiftDescriptorExtractor();
    }
};
REGISTER_FEATURE_DETECTOR(Sift, SIFT);
static_assert(DetectorTraits<Sift>::HasKeypoint);
static_assert(DetectorTraits<Sift>::HasDescriptor);

struct Surf : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
            add(ParameterFactory::declareRange("surf/extractor_threshold", 0, 5000, 1000, 1));
            add(ParameterFactory::declareRange("surf/nOctaves", 0, 10, 4, 1));
            add(ParameterFactory::declareRange("surf/nOctaveLayers", 0, 10, 2, 1));
            add(ParameterFactory::declareBool("surf/extended", true));
            add(ParameterFactory::declareBool("surf/upright", false));
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void keypoint(Extractor* e, const csapex::param::ParameterProvider& param, bool complete)
    {
        int et = params().read<int>(param, "surf/extractor_threshold");
        int nOctaves = params().read<int>(param, "surf/nOctaves");
        int nOctaveLayers = params().read<int>(param, "surf/nOctaveLayers");
        bool extended = params().read<bool>(param, "surf/extended");
        bool upright = params().read<bool>(param, "surf/upright");

        e->keypoint = "surf";
        e->has_orientation = true;
        e->detector = new cv::SurfFeatureDetector(et, nOctaves, nOctaveLayers, extended, upright);

        if (complete) {
            e->is_binary = false;
            e->descriptor = "surf";
            e->descriptor_extractor = e->detector;
        }
    }

    static void descriptor(Extractor* e, const csapex::param::ParameterProvider& param)
    {
        e->is_binary = false;
        e->descriptor = "surf";
        e->descriptor_extractor = new cv::SurfDescriptorExtractor();
    }
};
REGISTER_FEATURE_DETECTOR(Surf, SURF);
static_assert(DetectorTraits<Surf>::HasKeypoint);
static_assert(DetectorTraits<Surf>::HasDescriptor);
#endif

/// KEYPOINTS ONLY

struct Fast : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
            add(ParameterFactory::declareRange("fast/extractor_threshold", 0, 200, 50, 1));
            add(ParameterFactory::declareBool("fast/nonmaxSuppression", true));
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void keypoint(Extractor* e, const csapex::param::ParameterProvider& param, bool complete)
    {
        int et = params().read<int>(param, "fast/extractor_threshold");
        bool nonmaxSuppression = params().read<bool>(param, "fast/nonmaxSuppression");
        e->keypoint = "fast";
        e->has_orientation = false;
#if CV_MAJOR_VERSION == 2
        e->detector = new cv::FastFeatureDetector(et, nonmaxSuppression);
#elif CV_MAJOR_VERSION >= 3
        e->detector = cv::FastFeatureDetector::create(et, nonmaxSuppression);
#endif
    }
};
REGISTER_FEATURE_DETECTOR(Fast, FAST)
static_assert(DetectorTraits<Fast>::HasKeypoint);
static_assert(!DetectorTraits<Fast>::HasDescriptor);

struct Mser : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
            add(ParameterFactory::declareRange("mser/delta", 0, 50, 5, 1));
            add(ParameterFactory::declareRange("mser/minArea", 0, 1000, 60, 1));
            add(ParameterFactory::declareRange("mser/maxArea", 0, 30000, 14400, 1));
            add(ParameterFactory::declareRange("mser/maxVariation", 0.0, 1.0, 0.25, 0.01));
            add(ParameterFactory::declareRange("mser/minDiversity", 0.0, 1.0, 0.2, 0.01));
            add(ParameterFactory::declareRange("mser/maxEvolution", 0, 1000, 200, 1));
            add(ParameterFactory::declareRange("mser/areaThreshold", 0.0, 2.0, 1.01, 0.01));
            add(ParameterFactory::declareRange("mser/minMargin", 0.0, 0.1, 0.003, 0.001));
            add(ParameterFactory::declareRange("mser/edgeBlurSize", 0, 50, 5, 1));
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void keypoint(Extractor* e, const csapex::param::ParameterProvider& param, bool complete)
    {
        int delta = params().read<int>(param, "mser/delta");
        int minArea = params().read<int>(param, "mser/minArea");
        int maxArea = params().read<int>(param, "mser/maxArea");
        double maxVariation = params().read<double>(param, "mser/maxVariation");
        double minDiversity = params().read<double>(param, "mser/minDiversity");
        int maxEvolution = params().read<int>(param, "mser/maxEvolution");
        double areaThreshold = params().read<double>(param, "mser/areaThreshold");
        double minMargin = params().read<double>(param, "mser/minMargin");
        int edgeBlurSize = params().read<int>(param, "mser/edgeBlurSize");

        e->keypoint = "mser";
        e->has_orientation = true;

#if CV_MAJOR_VERSION == 2
        e->detector = new cv::MSER(delta, minArea, maxArea, maxVariation, minDiversity, maxEvolution, areaThreshold, minMargin, edgeBlurSize);
#elif CV_MAJOR_VERSION >= 3
        e->detector = cv::MSER::create(delta, minArea, maxArea, maxVariation, minDiversity, maxEvolution, areaThreshold, minMargin, edgeBlurSize);
#endif
    }
};
REGISTER_FEATURE_DETECTOR(Mser, MSER)
static_assert(DetectorTraits<Mser>::HasKeypoint);
static_assert(!DetectorTraits<Mser>::HasDescriptor);

#if CV_MAJOR_VERSION == 2

struct Star : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
            add(ParameterFactory::declareRange("star/extractor_threshold", 0, 1000, 50, 1));
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void keypoint(Extractor* e, const csapex::param::ParameterProvider& param, bool complete)
    {
        int et = params().read<int>(param, "star/extractor_threshold");

        e->keypoint = "star";
        e->has_orientation = true;

        e->detector = new cv::StarFeatureDetector(16, et);
    }
};
REGISTER_FEATURE_DETECTOR(Star, STAR);
static_assert(DetectorTraits<Star>::HasKeypoint);
static_assert(!DetectorTraits<Star>::HasDescriptor);

#endif

struct Gftt : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
            add(ParameterFactory::declareRange("gftt/extractor_threshold", 0, 1000, 50, 1));
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void keypoint(Extractor* e, const csapex::param::ParameterProvider& param, bool complete)
    {
        int et = params().read<int>(param, "gftt/extractor_threshold");

        e->keypoint = "gftt";
        e->has_orientation = true;

#if CV_MAJOR_VERSION == 2
        e->detector = new cv::GoodFeaturesToTrackDetector(et * 50.0, 0.005);
#elif CV_MAJOR_VERSION >= 3
        e->detector = cv::GFTTDetector::create(et * 50.0, 0.005);
#endif
    }
};
REGISTER_FEATURE_DETECTOR(Gftt, GFTT)
static_assert(DetectorTraits<Gftt>::HasKeypoint);
static_assert(!DetectorTraits<Gftt>::HasDescriptor);

struct GfttHarris : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
            add(ParameterFactory::declareRange("gftth/extractor_threshold", 0, 1000, 50, 1));
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void keypoint(Extractor* e, const csapex::param::ParameterProvider& param, bool complete)
    {
        int et = params().read<int>(param, "gftth/extractor_threshold");

        e->keypoint = "gftt_harris";
        e->has_orientation = true;

#if CV_MAJOR_VERSION == 2
        e->detector = new cv::GoodFeaturesToTrackDetector(et * 50.0, 0.005, 2, 3, true);
#elif CV_MAJOR_VERSION >= 3
        e->detector = cv::GFTTDetector::create(et * 50.0, 0.005, 2, 3, true);
#endif
    }
};
REGISTER_FEATURE_DETECTOR(GfttHarris, GFTT_HARRIS)
static_assert(DetectorTraits<GfttHarris>::HasKeypoint);
static_assert(!DetectorTraits<GfttHarris>::HasDescriptor);

/// DESCRIPTORS ONLY

#if CV_MAJOR_VERSION == 2

struct Brief : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void descriptor(Extractor* e, const csapex::param::ParameterProvider& param)
    {
        e->is_binary = true;
        e->descriptor = "brief";

#if CV_MAJOR_VERSION == 2
        e->descriptor_extractor = new cv::BriefDescriptorExtractor(64);
#elif CV_MAJOR_VERSION >= 3
        e->descriptor_extractor = cv::BriefDescriptorExtractor(64);
#endif
    }
};
REGISTER_FEATURE_DETECTOR(Brief, BRIEF);
static_assert(!DetectorTraits<Brief>::HasKeypoint);
static_assert(DetectorTraits<Brief>::HasDescriptor);

struct Freak : public ExtractorManager::ExtractorInitializer
{
    EXTRACTOR_IMPLEMENTATION

    struct KeyParams : public ExtractorManager::Params
    {
        KeyParams()
        {
        }
    };
    static KeyParams& params()
    {
        static KeyParams p;
        return p;
    }
    static std::vector<Parameter::Ptr> usedParameters()
    {
        return params().params;
    }

    static void descriptor(Extractor* e, const csapex::param::ParameterProvider& param)
    {
        e->is_binary = true;
        e->descriptor = "freak";
        e->descriptor_extractor = new cv::FREAK();
    }
};
REGISTER_FEATURE_DETECTOR(Freak, FREAK);
static_assert(!DetectorTraits<Freak>::HasKeypoint);
static_assert(DetectorTraits<Freak>::HasDescriptor);

#endif
#endif  // EXTRACTORS_DEFAULT_HPP
