#pragma once

#include <sensor_msgs/msg/image.hpp>

#include "depth_cam_stream_codec/common/color_frame.hpp"

namespace depth_cam_stream_codec::ros2 {

sensor_msgs::msg::Image convert_color_frame_to_ros(const common::ColorFrame& frame);
common::ColorFrame      convert_ros_to_color_frame(const sensor_msgs::msg::Image& msg);

}  // namespace depth_cam_stream_codec::ros2
