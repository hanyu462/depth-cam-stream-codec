#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace depth_cam_stream_codec::codec {

struct CompressedDepthFrame {
    int          width       = 0;
    int          height      = 0;
    int64_t      stamp_ns    = 0;
    std::string  frame_id;
    float        depth_scale = 0.001f;  // mm → m
    std::vector<uint8_t> data;          // RVL encoded

    bool empty() const { return data.empty(); }
};

}  // namespace depth_cam_stream_codec::codec
