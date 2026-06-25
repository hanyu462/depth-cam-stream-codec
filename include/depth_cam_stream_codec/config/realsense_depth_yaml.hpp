#pragma once

#include <string>

#include "depth_cam_stream_codec/camera/realsense_depth_config.hpp"

namespace depth_cam_stream_codec::config {

camera::RealsenseDepthConfig load_realsense_depth_config(const std::string& path);

}  // namespace depth_cam_stream_codec::config
