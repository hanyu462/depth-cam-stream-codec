#pragma once

#include <string>

#include "depth_cam_stream_codec/camera/realsense_pipeline_config.hpp"

namespace depth_cam_stream_codec::config {

camera::RealsensePipelineConfig load_realsense_pipeline_config(const std::string& path);

}  // namespace depth_cam_stream_codec::config
