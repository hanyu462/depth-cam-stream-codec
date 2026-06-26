#pragma once

#include <cstddef>
#include <cstdint>

#include "depth_cam_stream_codec/common/rvl_depth_frame.hpp"
#include "depth_cam_stream_codec/common/depth_frame.hpp"

namespace depth_cam_stream_codec::codec {

class RVLDepthEncoder {
public:
    RVLDepthEncoder() = default;

    RVLDepthFrame encode(const common::DepthFrame& frame) const;
};

}  // namespace depth_cam_stream_codec::codec
