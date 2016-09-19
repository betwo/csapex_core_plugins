/// HEADER
#include "make_video.h"

/// PROJECT
#include <csapex/msg/io.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/param/parameter_factory.h>
#include <csapex/signal/slot.h>
#include <csapex/model/node_modifier.h>

/// SYSTEM
#include <opencv2/opencv.hpp>

CSAPEX_REGISTER_CLASS(csapex::MakeVideo, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;


MakeVideo::MakeVideo()
{
}

void MakeVideo::setup(NodeModifier &node_modifier)
{
    input_ = node_modifier.addInput<CvMatMessage>("Input image");
    clear_ = node_modifier.addSlot("clear",
                                   std::bind(&MakeVideo::clear, this));
    write_ = node_modifier.addSlot("write",
                                   std::bind(&MakeVideo::writeBuffer, this));
}

void MakeVideo::setupParameters(Parameterizable &parameters)
{
    parameters.addParameter(param::ParameterFactory::declareFileOutputPath("path",
                                                                           ""),
                            path_);

    std::map<std::string, int> codec_types =
    {
        {"Uncompressed", 0},
        {"FLV1", CV_FOURCC('F', 'L', 'V', '1')},
    //  {"H.263I", CV_FOURCC('I', '2', '6', '3')}, -> currently not supported
        {"H.263", CV_FOURCC('U', '2', '6', '3')},
    //  {"H.264", CV_FOURCC('X','2','6','4')}, -> currently not supported
        {"motion-jpeg", CV_FOURCC('M','J','P','G')},
        {"MPEG-1", CV_FOURCC('P','I','M','1')},
        {"MPEG-4.2", CV_FOURCC('M', 'P', '4', '2')},
        {"MPEG-4.3", CV_FOURCC('D', 'I', 'V', '3')},
        {"MPEG-4", CV_FOURCC('D', 'I', 'V', 'X')},
    };

    file_extensions_ =
    {
        {codec_types["Uncompressed"], ".avi"},
        {codec_types["FLV1"], ".avi"},
     // {codec_types["H.263I"], ".avi"}, -> currently not supported
        {codec_types["H.263"], ".avi"},
     // {codec_types["H.264"], ".avi"}, -> currently not supported
        {codec_types["motion-jpeg"], ".mpg"},
        {codec_types["MPEG-1"], ".mpg"},
        {codec_types["MPEG-4.2"], ".mpg"},
        {codec_types["MPEG-4.3"], ".mpg"},
        {codec_types["MPEG-4"], ".mpg"}
    };

    parameters.addParameter(param::ParameterFactory::declareParameterSet("codec", codec_types, 0),
                            codec_type_);
    parameters.addParameter(param::ParameterFactory::declareRange("fps", 0.1, 100.0, 33.0, 0.1),
                            frame_rate_);
    parameters.addParameter(param::ParameterFactory::declareRange("width", 0, 1920, 0, 1),
                            frame_size_.width);
    parameters.addParameter(param::ParameterFactory::declareRange("height", 0, 1080, 0, 1),
                            frame_size_.height);
    parameters.addParameter(param::ParameterFactory::declareBool("buffer", true),
                            buffer_);

    parameters.addParameter(param::ParameterFactory::declareTrigger("write"),
                            std::bind(&MakeVideo::writeBuffer, this));
}

void MakeVideo::process()
{
    CvMatMessage::ConstPtr msg = msg::getMessage<CvMatMessage>(input_);
    if(buffer_) {
        msg_buffer_.push_back(msg);
    } else {
        writeOnline(msg);
    }
}

void MakeVideo::writeOnline(const CvMatMessage::ConstPtr &msg)
{
    if(frame_size_.width == 0 || frame_size_.height == 0) {
        throw std::runtime_error("Choose a valid frame size when wrinting data out online!");
    }

    if(!vw_.isOpened()) {
        vw_ = cv::VideoWriter(path_ + file_extensions_[codec_type_], codec_type_, frame_rate_, frame_size_, true);
    }

    cv::Mat image = msg->value.clone();
    if(image.type() != CV_8UC1 &&
            image.type() != CV_8UC3) {
            throw std::runtime_error("Need 8UC1 or 8UC3!");
    }
    if(image.type() == CV_8UC1)
        cv::cvtColor(image, image, CV_GRAY2BGR);

    vw_ << image;
}

void MakeVideo::writeBuffer()
{
    if(!buffer_)
        return;

    cv::Size max_size(std::numeric_limits<int>::min(),
                      std::numeric_limits<int>::min());

    for(CvMatMessage::ConstPtr &mat : msg_buffer_) {
        if(mat->value.cols > max_size.width)
            max_size.width = mat->value.cols;
        if(mat->value.rows > max_size.height)
            max_size.height = mat->value.rows;
        if(mat->value.type() != CV_8UC1 &&
                mat->value.type() != CV_8UC3)
            throw std::runtime_error("Need 8UC1 or 8UC3!");
    }

    cv::Size video_frame_size = frame_size_;
    if(video_frame_size.width == 0)
        video_frame_size.width = max_size.width;
    if(video_frame_size.height == 0)
        video_frame_size.height = max_size.height;


    vw_ = cv::VideoWriter(path_ + file_extensions_[codec_type_], codec_type_, frame_rate_, video_frame_size, true);
    for(CvMatMessage::ConstPtr &mat : msg_buffer_) {
        cv::Mat image = mat->value.clone();
        if(image.type() == CV_8UC1)
            cv::cvtColor(image, image, CV_GRAY2BGR);
        vw_ << image;
    }
    vw_.release();
}

void MakeVideo::clear()
{
    if(vw_.isOpened())
        vw_.release();
    msg_buffer_.clear();
}

