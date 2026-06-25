#pragma once

namespace depth_cam_stream_codec::camera {

struct SpatialFilterConfig {
    bool  enabled;
    int   magnitude;
    float smooth_alpha;
    float smooth_delta;
};

struct RealsenseDepthConfig {
    int                width;
    int                height;
    int                fps;
    bool               align_to_color;
    SpatialFilterConfig spatial_filter;
};

void validate_realsense_depth_config(const RealsenseDepthConfig& config);

}  // namespace depth_cam_stream_codec::camera
