#pragma once

#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>

#include "depth_cam_stream_codec/common/color_frame.hpp"
#include "depth_cam_stream_codec/common/latest_buffer.hpp"
#include "depth_cam_stream_codec/ros2/color_frame_adapter_config.hpp"

namespace depth_cam_stream_codec::ros2 {

using ColorFrameBuffer = common::LatestBuffer<common::ColorFrame>;

class ColorFrameAdapterNode : public rclcpp::Node {
public:
    ColorFrameAdapterNode(std::shared_ptr<ColorFrameBuffer>  buffer,
                          const ColorFrameAdapterConfig& config);

private:
    void publish_latest();

    std::shared_ptr<ColorFrameBuffer> buffer_;
    std::uint64_t                last_seq_{0};

    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr                          timer_;
};

}  // namespace depth_cam_stream_codec::ros2
