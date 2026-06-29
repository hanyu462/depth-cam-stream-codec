#pragma once

#include <optional>
#include <string>

namespace depth_cam_stream_codec::decoder {

struct DecoderColorConfig {
    std::string topic;
};

struct DecoderDepthConfig {
    std::string topic;
};

struct DecoderPipelineConfig {
    std::optional<DecoderColorConfig> color;
    std::optional<DecoderDepthConfig> depth;
};

}  // namespace depth_cam_stream_codec::decoder
