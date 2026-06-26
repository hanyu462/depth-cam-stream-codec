#pragma once

#include <sensor_msgs/msg/compressed_image.hpp>

#include "depth_cam_stream_codec/codec/compressed_color_frame.hpp"

namespace depth_cam_stream_codec::ros2 {

sensor_msgs::msg::CompressedImage
convert_compressed_color_frame_to_ros(const codec::CompressedColorFrame& frame);

}  // namespace depth_cam_stream_codec::ros2
