#pragma once

#include <string>
#include "depth_cam_stream_codec/camera/realsense_color_config.hpp"

namespace depth_cam_stream_codec::config {

camera::RealsenseColorConfig load_realsense_color_config(const std::string& path);

}  // namespace depth_cam_stream_codec::config
