#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

#include <rclcpp/rclcpp.hpp>

#include "depth_cam_stream_codec/common/aligned_frame.hpp"
#include "depth_cam_stream_codec/common/color_frame.hpp"
#include "depth_cam_stream_codec/common/depth_frame.hpp"
#include "depth_cam_stream_codec/common/h264_color_frame.hpp"
#include "depth_cam_stream_codec/common/latest_buffer.hpp"
#include "depth_cam_stream_codec/common/rvl_depth_frame.hpp"
#include "depth_cam_stream_codec/decoder/decoder_config.hpp"
#include "depth_cam_stream_codec/decoder/h264_color_decoder.hpp"
#include "depth_cam_stream_codec/decoder/rvl_depth_decoder.hpp"
#include "depth_cam_stream_codec/msg/compressed_color_frame.hpp"
#include "depth_cam_stream_codec/msg/compressed_depth_frame.hpp"

namespace depth_cam_stream_codec::decoder {

// Subscribes to compressed ROS2 topics, decodes each stream, and emits
// temporally aligned (color + depth) pairs to aligned_buf().
//
// Threads:
//   ROS2 executor  : subscription callbacks write to compressed input buffers
//   t_color_       : H.264 decode, event-driven on color_in_
//   t_depth_       : RVL decode, event-driven on depth_in_
//   (sync)         : try_pair() runs under sync_mutex_ inside both loops
class DecoderPipeline {
public:
    DecoderPipeline(const DecoderPipelineConfig& cfg, rclcpp::Node::SharedPtr node);
    ~DecoderPipeline();

    DecoderPipeline(const DecoderPipeline&)            = delete;
    DecoderPipeline& operator=(const DecoderPipeline&) = delete;

    void start();
    void stop();

    std::shared_ptr<common::LatestBuffer<common::AlignedFrame>> aligned_buf() { return aligned_out_; }

private:
    void color_loop();
    void depth_loop();
    void try_pair();  // call under sync_mutex_

    // Compressed input buffers (written by ROS2 subscription callbacks)
    std::shared_ptr<common::LatestBuffer<codec::H264ColorFrame>> color_in_;
    std::shared_ptr<common::LatestBuffer<codec::RVLDepthFrame>>  depth_in_;

    // Decoded output buffer (temporally aligned pairs)
    std::shared_ptr<common::LatestBuffer<common::AlignedFrame>> aligned_out_;

    // Sync state shared between color_loop and depth_loop
    std::mutex                    sync_mutex_;
    std::optional<common::ColorFrame> pending_color_;
    std::optional<common::DepthFrame> pending_depth_;

    // Decoders
    std::optional<codec::H264ColorDecoder> h264_dec_;
    codec::RVLDepthDecoder                 rvl_dec_;

    // Subscriptions
    rclcpp::Subscription<depth_cam_stream_codec::msg::CompressedColorFrame>::SharedPtr color_sub_;
    rclcpp::Subscription<depth_cam_stream_codec::msg::CompressedDepthFrame>::SharedPtr depth_sub_;

    std::atomic<bool> running_{false};
    std::thread       t_color_;
    std::thread       t_depth_;
};

}  // namespace depth_cam_stream_codec::decoder
