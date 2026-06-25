#include "depth_cam_stream_codec/ros2/color_frame_adapter_config.hpp"

#include <stdexcept>

namespace depth_cam_stream_codec::ros2 {

void validate_color_frame_adapter_config(const ColorFrameAdapterConfig& config)
{
    if (config.topic.empty())
        throw std::invalid_argument("color_frame_adapter.topic must not be empty");
    if (config.queue_size <= 0)
        throw std::invalid_argument("color_frame_adapter.queue_size must be > 0");
    if (config.timer_ms == 0)
        throw std::invalid_argument("color_frame_adapter.timer_ms must be > 0");
}

}  // namespace depth_cam_stream_codec::ros2
