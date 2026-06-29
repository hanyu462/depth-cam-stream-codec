#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <string>

#include "depth_cam_stream_codec/common/aligned_frame.hpp"

namespace depth_cam_stream_codec {

// Facade for receiving aligned (color + depth) frames over ROS2.
// Hides ROS2 lifecycle management — caller just calls start/stop/next_frame.
//
// Example:
//   DepthCamReceiver rx("config/decoder_pipeline.yaml");
//   rx.start();
//   while (running) {
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
    // No-op if already started. Safe to call before rclcpp::init — will call
    // rclcpp::init(0, nullptr) internally if rclcpp is not yet initialized.
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
