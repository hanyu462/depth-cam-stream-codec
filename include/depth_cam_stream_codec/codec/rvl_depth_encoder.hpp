#pragma once

#include <cstddef>
#include <cstdint>

#include "depth_cam_stream_codec/codec/compressed_depth_frame.hpp"
#include "depth_cam_stream_codec/common/depth_frame.hpp"

namespace depth_cam_stream_codec::codec {

class RVLDepthEncoder {
public:
    RVLDepthEncoder() = default;

    CompressedDepthFrame encode(const common::DepthFrame& frame) const;

    static void decode(const uint8_t* compressed, size_t compressed_size,
                       int pixel_count, uint16_t* out_pixels);
};

}  // namespace depth_cam_stream_codec::codec
