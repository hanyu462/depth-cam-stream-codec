#include "depth_cam_stream_codec/camera/realsense_depth_config.hpp"

#include <stdexcept>

namespace depth_cam_stream_codec::camera {

void validate_realsense_depth_config(const RealsenseDepthConfig& config)
{
    if (config.width <= 0)
        throw std::invalid_argument("realsense_depth.width must be > 0");
    if (config.height <= 0)
        throw std::invalid_argument("realsense_depth.height must be > 0");
    if (config.fps <= 0)
        throw std::invalid_argument("realsense_depth.fps must be > 0");
}

}  // namespace depth_cam_stream_codec::camera
