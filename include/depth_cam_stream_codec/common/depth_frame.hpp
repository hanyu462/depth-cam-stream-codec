#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace depth_cam_stream_codec::common {

struct DepthFrame {
    int width        = 0;
    int height       = 0;
    int stride_bytes = 0;

    std::int64_t stamp_ns = 0;
    std::string  frame_id;

    // Z16: 16-bit depth per pixel (mm 단위)
    std::vector<std::uint8_t> data;

    bool empty() const { return data.empty(); }
};

}  // namespace depth_cam_stream_codec::common
