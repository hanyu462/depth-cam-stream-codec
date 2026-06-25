#pragma once

#include <cstdint>
#include <string>

namespace depth_cam_stream_codec::ros2 {

struct ColorFrameAdapterConfig {
    std::string   topic      = "/camera/color/image_raw";
    int           queue_size = 10;
    std::uint32_t timer_ms   = 33;
};

void validate_color_frame_adapter_config(const ColorFrameAdapterConfig& config);

}  // namespace depth_cam_stream_codec::ros2
