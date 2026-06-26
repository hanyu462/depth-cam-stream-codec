#include "depth_cam_stream_codec/stream/compressed_stream_pipeline.hpp"

#include <cstdio>
#include <stdexcept>

#include "depth_cam_stream_codec/ros2/compressed_color_frame_adapter.hpp"
#include "depth_cam_stream_codec/ros2/compressed_depth_frame_adapter.hpp"

namespace depth_cam_stream_codec::stream {

CompressedStreamPipeline::CompressedStreamPipeline(
    const camera::RealsensePipelineConfig& cfg,
    rclcpp::Node::SharedPtr                node)
{
    if (!cfg.color)
        throw std::runtime_error("CompressedStreamPipeline: color config required");
    if (!cfg.color->h264)
        throw std::runtime_error("CompressedStreamPipeline: color.h264 config required");
    if (!cfg.depth)
        throw std::runtime_error("CompressedStreamPipeline: depth config required");

    color_buf_   = std::make_shared<camera::ColorFrameBuffer>();
    depth_buf_   = std::make_shared<camera::DepthFrameBuffer>();
    rs_pipeline_ = std::make_shared<camera::RealSensePipeline>(cfg, color_buf_, depth_buf_);
    h264_enc_.emplace(*cfg.color->h264);

    color_pub_ = node->create_publisher<sensor_msgs::msg::CompressedImage>(cfg.color->topic, 10);
    depth_pub_ = node->create_publisher<depth_cam_stream_codec::msg::CompressedDepthFrame>(cfg.depth->topic, 10);
}

CompressedStreamPipeline::~CompressedStreamPipeline()
{
    stop();
}

void CompressedStreamPipeline::start()
{
    if (running_.exchange(true)) return;

    rs_pipeline_->start();
    t_color_ = std::thread([this] { color_loop(); });
    t_depth_ = std::thread([this] { depth_loop(); });
}

void CompressedStreamPipeline::stop()
{
    running_.store(false);

    if (t_color_.joinable()) t_color_.join();
    if (t_depth_.joinable()) t_depth_.join();
    rs_pipeline_->stop();
}

void CompressedStreamPipeline::color_loop()
{
    std::uint64_t seq     = 0;
    std::uint64_t counter = 0;

    while (running_) {
        auto snap = color_buf_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        if (auto compressed = h264_enc_->encode(*snap->value))
            color_pub_->publish(ros2::convert_compressed_color_frame_to_ros(*compressed));

        if (++counter % 30 == 0)
            std::printf("[pipeline] color=%-6lu\n", counter);
    }
}

void CompressedStreamPipeline::depth_loop()
{
    std::uint64_t seq = 0;

    while (running_) {
        auto snap = depth_buf_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        depth_pub_->publish(
            ros2::convert_compressed_depth_frame_to_ros(rvl_enc_.encode(*snap->value)));
    }
}

}  // namespace depth_cam_stream_codec::stream
