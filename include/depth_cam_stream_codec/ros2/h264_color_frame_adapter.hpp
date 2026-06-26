#pragma once

#include <sensor_msgs/msg/compressed_image.hpp>

#include "depth_cam_stream_codec/common/h264_color_frame.hpp"

namespace depth_cam_stream_codec::ros2 {

sensor_msgs::msg::CompressedImage
convert_h264_color_frame_to_ros(const codec::H264ColorFrame& frame);

codec::H264ColorFrame
convert_ros_to_h264_color_frame(const sensor_msgs::msg::CompressedImage& msg);

}  // namespace depth_cam_stream_codec::ros2
