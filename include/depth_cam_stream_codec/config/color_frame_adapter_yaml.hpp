#pragma once

#include <string>
#include "depth_cam_stream_codec/ros2/color_frame_adapter_config.hpp"

namespace depth_cam_stream_codec::config {

ros2::ColorFrameAdapterConfig load_color_frame_adapter_config(const std::string& path);

}  // namespace depth_cam_stream_codec::config
