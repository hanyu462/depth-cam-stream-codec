#pragma once

#include <string>

#include "depth_cam_stream_codec/decoder/decoder_pipeline_config.hpp"

namespace depth_cam_stream_codec::config {

decoder::DecoderPipelineConfig load_decoder_pipeline_config(const std::string& path);

}  // namespace depth_cam_stream_codec::config
