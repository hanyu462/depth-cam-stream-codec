#include "depth_cam_stream_codec/decoder/decoder_pipeline.hpp"

#include "depth_cam_stream_codec/ros2/h264_color_frame_adapter.hpp"
#include "depth_cam_stream_codec/ros2/rvl_depth_frame_adapter.hpp"

namespace depth_cam_stream_codec::decoder {

DecoderPipeline::DecoderPipeline(
    const DecoderPipelineConfig& cfg,
    rclcpp::Node::SharedPtr      node)
{
    color_in_    = std::make_shared<common::LatestBuffer<codec::H264ColorFrame>>();
    depth_in_    = std::make_shared<common::LatestBuffer<codec::RVLDepthFrame>>();
    aligned_out_ = std::make_shared<common::LatestBuffer<common::AlignedFrame>>();

    if (cfg.color) {
        h264_dec_.emplace();
        color_sub_ = node->create_subscription<depth_cam_stream_codec::msg::CompressedColorFrame>(
            cfg.color->topic, 10,
            [this](depth_cam_stream_codec::msg::CompressedColorFrame::ConstSharedPtr msg) {
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

void DecoderPipeline::try_pair()
{
    if (!pending_color_ || !pending_depth_) return;

    if (pending_color_->sequence == pending_depth_->sequence) {
        common::AlignedFrame aligned;
        aligned.stamp_ns = pending_color_->stamp_ns;
        aligned.color    = std::move(*pending_color_);
        aligned.depth    = std::move(*pending_depth_);
        aligned_out_->write(std::move(aligned));
        pending_color_.reset();
        pending_depth_.reset();
        return;
    }

    // Sequences differ — drop the frame with the lower sequence number
    if (pending_color_->sequence < pending_depth_->sequence)
        pending_color_.reset();
    else
        pending_depth_.reset();
}

void DecoderPipeline::color_loop()
{
    std::uint64_t seq = 0;

    while (running_) {
        auto snap = color_in_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        if (auto decoded = h264_dec_->decode(*snap->value)) {
            std::lock_guard<std::mutex> lock(sync_mutex_);
            pending_color_ = std::move(*decoded);
            try_pair();
        }
    }
}

void DecoderPipeline::depth_loop()
{
    std::uint64_t seq = 0;

    while (running_) {
        auto snap = depth_in_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        {
            std::lock_guard<std::mutex> lock(sync_mutex_);
            pending_depth_ = rvl_dec_.decode(*snap->value);
            try_pair();
        }
    }
}

}  // namespace depth_cam_stream_codec::decoder
