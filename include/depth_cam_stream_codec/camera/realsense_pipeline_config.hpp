#pragma once

#include <optional>

namespace depth_cam_stream_codec::camera {

struct PipelineColorConfig {
    int width;
    int height;
    int fps;
};

struct PipelineSpatialFilterConfig {
    int   magnitude;
    float smooth_alpha;
    float smooth_delta;
};

struct PipelineDepthConfig {
    int  width;
    int  height;
    int  fps;
    bool align_to_color;
    std::optional<PipelineSpatialFilterConfig> spatial_filter;
};

struct RealsensePipelineConfig {
    std::optional<PipelineColorConfig> color;
    std::optional<PipelineDepthConfig> depth;
};

void validate_realsense_pipeline_config(const RealsensePipelineConfig& config);

}  // namespace depth_cam_stream_codec::camera
