#include "depth_cam_stream_codec/config/encoder_pipeline_yaml.hpp"

#include <stdexcept>

#include <yaml-cpp/yaml.h>

namespace depth_cam_stream_codec::config {

encoder::EncoderPipelineConfig load_encoder_pipeline_config(const std::string& path)
{
    YAML::Node root;
    try {
        root = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("failed to load config: " + std::string(e.what()));
    }

    const YAML::Node node = root["encoder_pipeline"];
    if (!node)
        throw std::runtime_error("missing key: encoder_pipeline in " + path);

    encoder::EncoderPipelineConfig cfg;

    if (const YAML::Node c = node["color"]) {
        if (!c["topic"])             throw std::runtime_error("missing key: encoder_pipeline.color.topic in "             + path);
        if (!c["frame_id"])          throw std::runtime_error("missing key: encoder_pipeline.color.frame_id in "          + path);
        if (!c["fps"])               throw std::runtime_error("missing key: encoder_pipeline.color.fps in "               + path);
        if (!c["bitrate_kbps"])      throw std::runtime_error("missing key: encoder_pipeline.color.bitrate_kbps in "      + path);
        if (!c["preset"])            throw std::runtime_error("missing key: encoder_pipeline.color.preset in "            + path);
        if (!c["tune"])              throw std::runtime_error("missing key: encoder_pipeline.color.tune in "              + path);
        if (!c["profile"])           throw std::runtime_error("missing key: encoder_pipeline.color.profile in "           + path);
        if (!c["keyframe_interval"]) throw std::runtime_error("missing key: encoder_pipeline.color.keyframe_interval in " + path);

        encoder::EncoderColorConfig color;
        color.topic             = c["topic"].as<std::string>();
        color.frame_id          = c["frame_id"].as<std::string>();
        color.fps               = c["fps"].as<int>();
        color.bitrate_kbps      = c["bitrate_kbps"].as<int>();
        color.preset            = c["preset"].as<std::string>();
        color.tune              = c["tune"].as<std::string>();
        color.profile           = c["profile"].as<std::string>();
        color.keyframe_interval = c["keyframe_interval"].as<int>();
        cfg.color               = color;
    }

    if (const YAML::Node d = node["depth"]) {
        if (!d["topic"])    throw std::runtime_error("missing key: encoder_pipeline.depth.topic in "    + path);
        if (!d["frame_id"]) throw std::runtime_error("missing key: encoder_pipeline.depth.frame_id in " + path);
        if (!d["fps"])      throw std::runtime_error("missing key: encoder_pipeline.depth.fps in "      + path);

        encoder::EncoderDepthConfig depth;
        depth.topic    = d["topic"].as<std::string>();
        depth.frame_id = d["frame_id"].as<std::string>();
        depth.fps      = d["fps"].as<int>();
        cfg.depth      = depth;
    }

    return cfg;
}

}  // namespace depth_cam_stream_codec::config
