#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <x264.h>

#include "depth_cam_stream_codec/codec/compressed_color_frame.hpp"
#include "depth_cam_stream_codec/codec/h264_encoder_config.hpp"
#include "depth_cam_stream_codec/common/color_frame.hpp"

namespace depth_cam_stream_codec::codec {

class H264ColorEncoder {
public:
    explicit H264ColorEncoder(const H264EncoderConfig& cfg);
    ~H264ColorEncoder();

    H264ColorEncoder(const H264ColorEncoder&)            = delete;
    H264ColorEncoder& operator=(const H264ColorEncoder&) = delete;

    std::optional<CompressedColorFrame> encode(const common::ColorFrame& frame);
    void flush();
    void reset(const H264EncoderConfig& new_cfg);

private:
    void open(const H264EncoderConfig& cfg);
    void close();
    void bgr8_to_i420(const uint8_t* bgr, int width, int height, int stride_bgr);

    H264EncoderConfig cfg_;
    x264_t*           encoder_ = nullptr;
    x264_picture_t    pic_in_{};
    x264_picture_t    pic_out_{};
    int64_t           frame_count_ = 0;
};

}  // namespace depth_cam_stream_codec::codec
