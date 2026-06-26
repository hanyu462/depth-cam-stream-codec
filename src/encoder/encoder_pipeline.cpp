#include "depth_cam_stream_codec/encoder/encoder_pipeline.hpp"

#include <stdexcept>

#include "depth_cam_stream_codec/ros2/h264_color_frame_adapter.hpp"
#include "depth_cam_stream_codec/ros2/rvl_depth_frame_adapter.hpp"

namespace depth_cam_stream_codec::encoder {

EncoderPipeline::EncoderPipeline(
    const camera::RealsensePipelineConfig& cfg,
    rclcpp::Node::SharedPtr                node)
{
    if (!cfg.color)
        throw std::runtime_error("EncoderPipeline: color config required");
    if (!cfg.color->h264)
        throw std::runtime_error("EncoderPipeline: color.h264 config required");
    if (!cfg.depth)
        throw std::runtime_error("EncoderPipeline: depth config required");

    color_buf_   = std::make_shared<camera::ColorFrameBuffer>();
    depth_buf_   = std::make_shared<camera::DepthFrameBuffer>();
    rs_pipeline_ = std::make_shared<camera::RealSensePipeline>(cfg, color_buf_, depth_buf_);
    h264_enc_.emplace(*cfg.color->h264);

    color_pub_ = node->create_publisher<sensor_msgs::msg::CompressedImage>(cfg.color->topic, 10);
    depth_pub_ = node->create_publisher<depth_cam_stream_codec::msg::CompressedDepthFrame>(cfg.depth->topic, 10);
}

EncoderPipeline::~EncoderPipeline()
{
    stop();
}

void EncoderPipeline::start()
{
    if (running_.exchange(true)) return;

    rs_pipeline_->start();
    t_color_ = std::thread([this] { color_loop(); });
    t_depth_ = std::thread([this] { depth_loop(); });
}

void EncoderPipeline::stop()
{
    running_.store(false);

    if (t_color_.joinable()) t_color_.join();
    if (t_depth_.joinable()) t_depth_.join();
    rs_pipeline_->stop();
}

void EncoderPipeline::color_loop()
{
    std::uint64_t seq = 0;

    while (running_) {
        auto snap = color_buf_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        if (auto compressed = h264_enc_->encode(*snap->value))
            color_pub_->publish(ros2::convert_h264_color_frame_to_ros(*compressed));

    }
}

void EncoderPipeline::depth_loop()
{
    std::uint64_t seq = 0;

    while (running_) {
        auto snap = depth_buf_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        depth_pub_->publish(
            ros2::convert_rvl_depth_frame_to_ros(rvl_enc_.encode(*snap->value)));

    }
}

}  // namespace depth_cam_stream_codec::encoder
