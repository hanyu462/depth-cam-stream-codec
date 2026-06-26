#include "depth_cam_stream_codec/config/decoder_pipeline_yaml.hpp"

#include <stdexcept>

#include <yaml-cpp/yaml.h>

namespace depth_cam_stream_codec::config {

decoder::DecoderPipelineConfig load_decoder_pipeline_config(const std::string& path)
{
    YAML::Node root;
    try {
        root = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("failed to load config: " + std::string(e.what()));
    }

    const YAML::Node node = root["decoder_pipeline"];
    if (!node)
        throw std::runtime_error("missing key: decoder_pipeline in " + path);

    decoder::DecoderPipelineConfig cfg;

    if (const YAML::Node c = node["color"]) {
        if (!c["topic"])
            throw std::runtime_error("missing key: decoder_pipeline.color.topic in " + path);
        decoder::DecoderColorConfig color;
        color.topic = c["topic"].as<std::string>();
        cfg.color   = color;
    }

    if (const YAML::Node d = node["depth"]) {
        if (!d["topic"])
            throw std::runtime_error("missing key: decoder_pipeline.depth.topic in " + path);
        decoder::DecoderDepthConfig depth;
        depth.topic = d["topic"].as<std::string>();
        cfg.depth   = depth;
    }

    return cfg;
}

}  // namespace depth_cam_stream_codec::config
