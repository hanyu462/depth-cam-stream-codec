#pragma once

#include "depth_cam_stream_codec/msg/compressed_depth_frame.hpp"
#include "depth_cam_stream_codec/common/rvl_depth_frame.hpp"

namespace depth_cam_stream_codec::ros2 {

depth_cam_stream_codec::msg::CompressedDepthFrame
convert_rvl_depth_frame_to_ros(const codec::RVLDepthFrame& frame);

codec::RVLDepthFrame
convert_ros_to_rvl_depth_frame(const depth_cam_stream_codec::msg::CompressedDepthFrame& msg);

}  // namespace depth_cam_stream_codec::ros2
