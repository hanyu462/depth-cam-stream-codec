#pragma once

#include "depth_cam_stream_codec/common/color_frame.hpp"
#include "depth_cam_stream_codec/common/depth_frame.hpp"
#include "depth_cam_stream_codec/common/latest_buffer.hpp"

namespace depth_cam_stream_codec::camera {

using ColorFrameBuffer = common::LatestBuffer<common::ColorFrame>;
using DepthFrameBuffer = common::LatestBuffer<common::DepthFrame>;

}  // namespace depth_cam_stream_codec::camera
