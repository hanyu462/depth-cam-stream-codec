#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace depth_cam_stream_codec::codec {

struct CompressedColorFrame {
    int          width       = 0;
    int          height      = 0;
    int64_t      stamp_ns    = 0;
    std::string  frame_id;
    bool         is_keyframe = false;
    std::vector<uint8_t> data;  // Annex B H.264 NAL units

    bool empty() const { return data.empty(); }
};

}  // namespace depth_cam_stream_codec::codec
