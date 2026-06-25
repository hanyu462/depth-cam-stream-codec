#include "depth_cam_stream_codec/camera/realsense_color_config.hpp"

#include <stdexcept>

namespace depth_cam_stream_codec::camera {

void validate_realsense_color_config(const RealsenseColorConfig& config)
{
    if (config.width <= 0)
        throw std::invalid_argument("realsense_color.width must be > 0");
    if (config.height <= 0)
        throw std::invalid_argument("realsense_color.height must be > 0");
    if (config.fps <= 0)
        throw std::invalid_argument("realsense_color.fps must be > 0");
}

}  // namespace depth_cam_stream_codec::camera
