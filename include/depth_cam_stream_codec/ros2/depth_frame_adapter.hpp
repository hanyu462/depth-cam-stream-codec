#pragma once

#include <sensor_msgs/msg/image.hpp>

#include "depth_cam_stream_codec/common/depth_frame.hpp"

namespace depth_cam_stream_codec::ros2 {

sensor_msgs::msg::Image convert_depth_frame_to_ros(const common::DepthFrame& frame);

}  // namespace depth_cam_stream_codec::ros2
