#pragma once

#include <string>

namespace depth_cam_stream_codec::codec {

struct H264EncoderConfig {
    int         width;
    int         height;
    int         fps;
    int         bitrate_kbps;
    std::string preset;
    std::string tune;
    std::string profile;
    int         keyframe_interval;
};

}  // namespace depth_cam_stream_codec::codec
