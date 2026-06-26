#include "depth_cam_stream_codec/config/realsense_pipeline_yaml.hpp"

#include <stdexcept>
#include <yaml-cpp/yaml.h>

#include "depth_cam_stream_codec/codec/h264_encoder_config.hpp"

namespace depth_cam_stream_codec::config {

camera::RealsensePipelineConfig load_realsense_pipeline_config(const std::string& path)
{
    YAML::Node root;
    try {
        root = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("failed to load config: " + std::string(e.what()));
    }

    const YAML::Node node = root["realsense_pipeline"];
    if (!node)
        throw std::runtime_error("missing key: realsense_pipeline in " + path);

    camera::RealsensePipelineConfig cfg;

    if (const YAML::Node c = node["color"]) {
        if (!c["width"])    throw std::runtime_error("missing key: realsense_pipeline.color.width in "    + path);
        if (!c["height"])   throw std::runtime_error("missing key: realsense_pipeline.color.height in "   + path);
        if (!c["fps"])      throw std::runtime_error("missing key: realsense_pipeline.color.fps in "      + path);
        if (!c["topic"])    throw std::runtime_error("missing key: realsense_pipeline.color.topic in "    + path);
        if (!c["frame_id"]) throw std::runtime_error("missing key: realsense_pipeline.color.frame_id in " + path);

        camera::PipelineColorConfig color;
        color.width    = c["width"].as<int>();
        color.height   = c["height"].as<int>();
        color.fps      = c["fps"].as<int>();
        color.topic    = c["topic"].as<std::string>();
        color.frame_id = c["frame_id"].as<std::string>();

        if (const YAML::Node h = c["h264"]) {
            if (!h["bitrate_kbps"])      throw std::runtime_error("missing key: realsense_pipeline.color.h264.bitrate_kbps in "      + path);
            if (!h["preset"])            throw std::runtime_error("missing key: realsense_pipeline.color.h264.preset in "            + path);
            if (!h["tune"])              throw std::runtime_error("missing key: realsense_pipeline.color.h264.tune in "              + path);
            if (!h["profile"])           throw std::runtime_error("missing key: realsense_pipeline.color.h264.profile in "           + path);
            if (!h["keyframe_interval"]) throw std::runtime_error("missing key: realsense_pipeline.color.h264.keyframe_interval in " + path);

            codec::H264EncoderConfig h264;
            h264.width             = color.width;
            h264.height            = color.height;
            h264.fps               = color.fps;
            h264.bitrate_kbps      = h["bitrate_kbps"].as<int>();
            h264.preset            = h["preset"].as<std::string>();
            h264.tune              = h["tune"].as<std::string>();
            h264.profile           = h["profile"].as<std::string>();
            h264.keyframe_interval = h["keyframe_interval"].as<int>();
            color.h264             = h264;
        }

        cfg.color = color;
    }

    if (const YAML::Node d = node["depth"]) {
        if (!d["width"])          throw std::runtime_error("missing key: realsense_pipeline.depth.width in "          + path);
        if (!d["height"])         throw std::runtime_error("missing key: realsense_pipeline.depth.height in "         + path);
        if (!d["fps"])            throw std::runtime_error("missing key: realsense_pipeline.depth.fps in "            + path);
        if (!d["topic"])          throw std::runtime_error("missing key: realsense_pipeline.depth.topic in "          + path);
        if (!d["frame_id"])       throw std::runtime_error("missing key: realsense_pipeline.depth.frame_id in "       + path);
        if (!d["align_to_color"]) throw std::runtime_error("missing key: realsense_pipeline.depth.align_to_color in " + path);

        camera::PipelineDepthConfig depth;
        depth.width          = d["width"].as<int>();
        depth.height         = d["height"].as<int>();
        depth.fps            = d["fps"].as<int>();
        depth.topic          = d["topic"].as<std::string>();
        depth.frame_id       = d["frame_id"].as<std::string>();
        depth.align_to_color = d["align_to_color"].as<bool>();

        if (const YAML::Node sf = d["spatial_filter"]) {
            if (!sf["magnitude"])    throw std::runtime_error("missing key: realsense_pipeline.depth.spatial_filter.magnitude in "    + path);
            if (!sf["smooth_alpha"]) throw std::runtime_error("missing key: realsense_pipeline.depth.spatial_filter.smooth_alpha in " + path);
            if (!sf["smooth_delta"]) throw std::runtime_error("missing key: realsense_pipeline.depth.spatial_filter.smooth_delta in " + path);

            camera::PipelineSpatialFilterConfig spatial;
            spatial.magnitude    = sf["magnitude"].as<int>();
            spatial.smooth_alpha = sf["smooth_alpha"].as<float>();
            spatial.smooth_delta = sf["smooth_delta"].as<float>();
            depth.spatial_filter = spatial;
        }

        cfg.depth = depth;
    }

    validate_realsense_pipeline_config(cfg);
    return cfg;
}

}  // namespace depth_cam_stream_codec::config
