#pragma once

#include <string>

#include "depth_cam_stream_codec/encoder/encoder_pipeline_config.hpp"

namespace depth_cam_stream_codec::config {

encoder::EncoderPipelineConfig load_encoder_pipeline_config(const std::string& path);

}  // namespace depth_cam_stream_codec::config
