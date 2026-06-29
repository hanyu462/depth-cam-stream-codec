#pragma once

#include "depth_cam_stream_codec/common/h264_color_frame.hpp"
#include "depth_cam_stream_codec/msg/compressed_color_frame.hpp"

namespace depth_cam_stream_codec::ros2 {

depth_cam_stream_codec::msg::CompressedColorFrame
convert_h264_color_frame_to_ros(const codec::H264ColorFrame& frame);

codec::H264ColorFrame
convert_ros_to_h264_color_frame(const depth_cam_stream_codec::msg::CompressedColorFrame& msg);

}  // namespace depth_cam_stream_codec::ros2
