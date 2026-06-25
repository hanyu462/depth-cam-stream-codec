#include "depth_cam_stream_codec/config/realsense_color_yaml.hpp"

#include <stdexcept>
#include <yaml-cpp/yaml.h>

#include "depth_cam_stream_codec/camera/realsense_color_config.hpp"

namespace depth_cam_stream_codec::config {

camera::RealsenseColorConfig load_realsense_color_config(const std::string& path)
{
    YAML::Node root;
    try {
        root = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("failed to load config: " + std::string(e.what()));
    }

    const YAML::Node node = root["realsense_color"];
    if (!node)
        throw std::runtime_error("missing key: realsense_color");

    if (!node["width"])  throw std::runtime_error("missing key: realsense_color.width in "  + path);
    if (!node["height"]) throw std::runtime_error("missing key: realsense_color.height in " + path);
    if (!node["fps"])    throw std::runtime_error("missing key: realsense_color.fps in "    + path);

    camera::RealsenseColorConfig cfg;
    cfg.width  = node["width"].as<int>();
    cfg.height = node["height"].as<int>();
    cfg.fps    = node["fps"].as<int>();

    validate_realsense_color_config(cfg);
    return cfg;
}

}  // namespace depth_cam_stream_codec::config
