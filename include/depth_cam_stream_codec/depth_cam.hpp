#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <string>

#include "depth_cam_stream_codec/common/aligned_frame.hpp"

namespace depth_cam_stream_codec {

// Facade for transmitting RealSense frames over ROS2.
// Hides camera capture, encoding, and ROS2 publishing behind start/stop.
//
// Example:
//   DepthCamTrans tx("config/realsense_pipeline.yaml",
//                    "config/encoder_pipeline.yaml");
//   tx.start();
//   // ... rest of your node (IMU, control loop, etc.)
//   tx.stop();
class DepthCamTrans {
public:
    DepthCamTrans(std::string rs_config_path, std::string enc_config_path);
    ~DepthCamTrans();

    DepthCamTrans(const DepthCamTrans&)            = delete;
    DepthCamTrans& operator=(const DepthCamTrans&) = delete;

    // Load configs, create node and pipeline, start camera capture + encoding.
    // Calls rclcpp::init(0, nullptr) internally if rclcpp is not yet initialized.
    void start();

    // Stop all threads and release resources. Idempotent.
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ─────────────────────────────────────────────────────────────────────────────

// Facade for receiving aligned (color + depth) frames from ROS2 topics.
// Hides decoding and ROS2 subscription management behind start/stop/next_frame.
//
// Example:
//   DepthCamReceiver rx("config/decoder_pipeline.yaml");
//   rx.start();
//   while (rclcpp::ok()) {
//       if (auto frame = rx.next_frame())
//           process(frame->color, frame->depth);
//   }
//   rx.stop();
class DepthCamReceiver {
public:
    explicit DepthCamReceiver(std::string config_path);
    ~DepthCamReceiver();

    DepthCamReceiver(const DepthCamReceiver&)            = delete;
    DepthCamReceiver& operator=(const DepthCamReceiver&) = delete;

    // Load config, create node and pipeline, start ROS2 spin thread.
    // Calls rclcpp::init(0, nullptr) internally if rclcpp is not yet initialized.
    void start();

    // Stop spin thread and decoder pipeline. Idempotent.
    void stop();

    // Block up to `timeout` for a new AlignedFrame.
    // Returns nullopt on timeout or before start() is called.
    std::optional<common::AlignedFrame> next_frame(
        std::chrono::milliseconds timeout = std::chrono::milliseconds(100));

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace depth_cam_stream_codec
