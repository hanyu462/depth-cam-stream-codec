#include "depth_cam_stream_codec/camera/realsense_pipeline_config.hpp"

#include <stdexcept>

namespace depth_cam_stream_codec::camera {

void validate_realsense_pipeline_config(const RealsensePipelineConfig& config)
{
    if (!config.color && !config.depth)
        throw std::invalid_argument("realsense_pipeline: at least one stream (color or depth) must be enabled");

    if (config.color) {
        if (config.color->width  <= 0) throw std::invalid_argument("realsense_pipeline.color.width must be > 0");
        if (config.color->height <= 0) throw std::invalid_argument("realsense_pipeline.color.height must be > 0");
        if (config.color->fps    <= 0) throw std::invalid_argument("realsense_pipeline.color.fps must be > 0");
    }

    if (config.depth) {
        if (config.depth->width  <= 0) throw std::invalid_argument("realsense_pipeline.depth.width must be > 0");
        if (config.depth->height <= 0) throw std::invalid_argument("realsense_pipeline.depth.height must be > 0");
        if (config.depth->fps    <= 0) throw std::invalid_argument("realsense_pipeline.depth.fps must be > 0");
    }
}

}  // namespace depth_cam_stream_codec::camera
