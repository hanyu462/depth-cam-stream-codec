#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace depth_cam_stream_codec::common {

struct ColorFrame {
    int width        = 0;
    int height       = 0;
    int stride_bytes = 0;

    std::uint64_t sequence = 0;
    std::int64_t  stamp_ns = 0;
    std::string   frame_id;

    // BGR8 고정
    std::vector<std::uint8_t> data;

    bool empty() const { return data.empty(); }
};

}  // namespace depth_cam_stream_codec::common
