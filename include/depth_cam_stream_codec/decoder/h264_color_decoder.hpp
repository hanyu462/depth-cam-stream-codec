#pragma once

#include <memory>
#include <optional>

#include "depth_cam_stream_codec/common/color_frame.hpp"
#include "depth_cam_stream_codec/common/h264_color_frame.hpp"

namespace depth_cam_stream_codec::codec {

class H264ColorDecoder {
public:
    H264ColorDecoder();
    ~H264ColorDecoder();

    H264ColorDecoder(const H264ColorDecoder&)            = delete;
    H264ColorDecoder& operator=(const H264ColorDecoder&) = delete;

    std::optional<common::ColorFrame> decode(const H264ColorFrame& frame);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace depth_cam_stream_codec::codec
