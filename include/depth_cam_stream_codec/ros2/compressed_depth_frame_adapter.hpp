#pragma once

#include "depth_cam_stream_codec/msg/compressed_depth_frame.hpp"
#include "depth_cam_stream_codec/codec/compressed_depth_frame.hpp"

namespace depth_cam_stream_codec::ros2 {

depth_cam_stream_codec::msg::CompressedDepthFrame
convert_compressed_depth_frame_to_ros(const codec::CompressedDepthFrame& frame);

}  // namespace depth_cam_stream_codec::ros2
