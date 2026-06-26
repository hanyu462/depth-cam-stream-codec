#pragma once

#include <atomic>
#include <memory>
#include <optional>
#include <string>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>

#include "depth_cam_stream_codec/camera/realsense_pipeline.hpp"
#include "depth_cam_stream_codec/camera/realsense_pipeline_config.hpp"
#include "depth_cam_stream_codec/encoder/h264_color_encoder.hpp"
#include "depth_cam_stream_codec/encoder/rvl_depth_encoder.hpp"
#include "depth_cam_stream_codec/common/frame_buffers.hpp"
#include "depth_cam_stream_codec/msg/compressed_depth_frame.hpp"

namespace depth_cam_stream_codec::encoder {

// Owns all threads and components for camera capture, encoding, and publishing.
//
// Threads:
//   T1 (inside RealSensePipeline) : camera capture
//   T2 (t_color_)                 : H.264 encode + publish, event-driven
//   T3 (t_depth_)                 : RVL encode + publish, event-driven
class EncoderPipeline {
public:
    EncoderPipeline(
        const camera::RealsensePipelineConfig& cfg,
        rclcpp::Node::SharedPtr                node);

    ~EncoderPipeline();

    EncoderPipeline(const EncoderPipeline&)            = delete;
    EncoderPipeline& operator=(const EncoderPipeline&) = delete;

    void start();
    void stop();

private:
    void color_loop();
    void depth_loop();

    // Buffers
    std::shared_ptr<camera::ColorFrameBuffer> color_buf_;
    std::shared_ptr<camera::DepthFrameBuffer> depth_buf_;

    // Camera capture (T1)
    std::shared_ptr<camera::RealSensePipeline> rs_pipeline_;

    // Encoders
    std::optional<codec::H264ColorEncoder> h264_enc_;
    codec::RVLDepthEncoder                 rvl_enc_;

    // Publishers
    rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr              color_pub_;
    rclcpp::Publisher<depth_cam_stream_codec::msg::CompressedDepthFrame>::SharedPtr depth_pub_;

    // Encode + publish threads (T2, T3)
    std::atomic<bool> running_{false};
    std::thread       t_color_;
    std::thread       t_depth_;
};

}  // namespace depth_cam_stream_codec::encoder
