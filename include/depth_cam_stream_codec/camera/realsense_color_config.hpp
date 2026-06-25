#pragma once

namespace depth_cam_stream_codec::camera {

struct RealsenseColorConfig {
    int width  = 1280;
    int height = 720;
    int fps    = 30;
};

void validate_realsense_color_config(const RealsenseColorConfig& config);

}  // namespace depth_cam_stream_codec::camera
