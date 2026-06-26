#include "depth_cam_stream_codec/decoder/decoder_pipeline.hpp"

#include "depth_cam_stream_codec/ros2/h264_color_frame_adapter.hpp"
#include "depth_cam_stream_codec/ros2/rvl_depth_frame_adapter.hpp"

namespace depth_cam_stream_codec::decoder {

DecoderPipeline::DecoderPipeline(
    const DecoderPipelineConfig& cfg,
    rclcpp::Node::SharedPtr      node)
{
    color_in_  = std::make_shared<common::LatestBuffer<codec::H264ColorFrame>>();
    depth_in_  = std::make_shared<common::LatestBuffer<codec::RVLDepthFrame>>();
    color_out_ = std::make_shared<common::LatestBuffer<common::ColorFrame>>();
    depth_out_ = std::make_shared<common::LatestBuffer<common::DepthFrame>>();

    if (cfg.color) {
        h264_dec_.emplace();
        color_sub_ = node->create_subscription<sensor_msgs::msg::CompressedImage>(
            cfg.color->topic, 10,
            [this](sensor_msgs::msg::CompressedImage::ConstSharedPtr msg) {
                color_in_->write(ros2::convert_ros_to_h264_color_frame(*msg));
            });
    }

    if (cfg.depth) {
        depth_sub_ = node->create_subscription<depth_cam_stream_codec::msg::CompressedDepthFrame>(
            cfg.depth->topic, 10,
            [this](depth_cam_stream_codec::msg::CompressedDepthFrame::ConstSharedPtr msg) {
                depth_in_->write(ros2::convert_ros_to_rvl_depth_frame(*msg));
            });
    }
}

DecoderPipeline::~DecoderPipeline()
{
    stop();
}

void DecoderPipeline::start()
{
    if (running_.exchange(true)) return;

    if (h264_dec_)
        t_color_ = std::thread([this] { color_loop(); });
    if (depth_sub_)
        t_depth_ = std::thread([this] { depth_loop(); });
}

void DecoderPipeline::stop()
{
    running_.store(false);
    if (t_color_.joinable()) t_color_.join();
    if (t_depth_.joinable()) t_depth_.join();
}

void DecoderPipeline::color_loop()
{
    std::uint64_t seq = 0;

    while (running_) {
        auto snap = color_in_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        if (auto decoded = h264_dec_->decode(*snap->value))
            color_out_->write(std::move(*decoded));
    }
}

void DecoderPipeline::depth_loop()
{
    std::uint64_t seq = 0;

    while (running_) {
        auto snap = depth_in_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        depth_out_->write(rvl_dec_.decode(*snap->value));
    }
}

}  // namespace depth_cam_stream_codec::decoder
