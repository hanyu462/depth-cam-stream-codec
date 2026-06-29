#pragma once

#include <optional>
#include <string>

namespace depth_cam_stream_codec::encoder {

struct EncoderColorConfig {
    std::string topic;
    std::string frame_id;
    int         fps;
    int         bitrate_kbps;
    std::string preset;
    std::string tune;
    std::string profile;
    int         keyframe_interval;
};

struct EncoderDepthConfig {
    std::string topic;
    std::string frame_id;
    // RVL has no tunable parameters yet
};

struct EncoderPipelineConfig {
    std::optional<EncoderColorConfig> color;
    std::optional<EncoderDepthConfig> depth;
};

}  // namespace depth_cam_stream_codec::encoder
