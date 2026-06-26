#pragma once

#include <optional>
#include <string>

#include "depth_cam_stream_codec/encoder/encoder_config.hpp"

namespace depth_cam_stream_codec::camera {

struct PipelineColorConfig {
    int         width;
    int         height;
    int         fps;
    std::string topic;
    std::string frame_id;
    std::optional<codec::H264EncoderConfig> h264;
};

struct PipelineSpatialFilterConfig {
    int   magnitude;
    float smooth_alpha;
    float smooth_delta;
};

struct PipelineDepthConfig {
    int         width;
    int         height;
    int         fps;
    std::string topic;
    std::string frame_id;
    bool        align_to_color;
    std::optional<PipelineSpatialFilterConfig> spatial_filter;
    std::optional<codec::RVLEncoderConfig>     rvl;
};

struct RealsensePipelineConfig {
    std::optional<PipelineColorConfig> color;
    std::optional<PipelineDepthConfig> depth;
};

void validate_realsense_pipeline_config(const RealsensePipelineConfig& config);

}  // namespace depth_cam_stream_codec::camera
