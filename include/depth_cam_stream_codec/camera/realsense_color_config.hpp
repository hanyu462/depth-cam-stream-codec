#pragma once

namespace depth_cam_stream_codec::camera {

struct RealsenseColorConfig {
    int width;
    int height;
    int fps;
};

void validate_realsense_color_config(const RealsenseColorConfig& config);

}  // namespace depth_cam_stream_codec::camera
