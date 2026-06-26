#pragma once

#include "depth_cam_stream_codec/common/depth_frame.hpp"
#include "depth_cam_stream_codec/common/rvl_depth_frame.hpp"

namespace depth_cam_stream_codec::codec {

class RVLDepthDecoder {
public:
    RVLDepthDecoder() = default;

    common::DepthFrame decode(const RVLDepthFrame& frame) const;
};

}  // namespace depth_cam_stream_codec::codec
