#pragma once

#include "depth_cam_stream_codec/common/color_frame.hpp"
#include "depth_cam_stream_codec/common/depth_frame.hpp"

namespace depth_cam_stream_codec::common {

struct AlignedFrame {
    int64_t    stamp_ns = 0;
    ColorFrame color;
    DepthFrame depth;

    bool empty() const { return color.empty() || depth.empty(); }
};

}  // namespace depth_cam_stream_codec::common
