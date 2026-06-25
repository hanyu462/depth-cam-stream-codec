#include "depth_cam_stream_codec/config/realsense_depth_yaml.hpp"

#include <stdexcept>
#include <yaml-cpp/yaml.h>

#include "depth_cam_stream_codec/camera/realsense_depth_config.hpp"

namespace depth_cam_stream_codec::config {

camera::RealsenseDepthConfig load_realsense_depth_config(const std::string& path)
{
    YAML::Node root;
    try {
        root = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("failed to load config: " + std::string(e.what()));
    }

    const YAML::Node node = root["realsense_depth"];
    if (!node)
        throw std::runtime_error("missing key: realsense_depth in " + path);

    if (!node["width"])          throw std::runtime_error("missing key: realsense_depth.width in "          + path);
    if (!node["height"])         throw std::runtime_error("missing key: realsense_depth.height in "         + path);
    if (!node["fps"])            throw std::runtime_error("missing key: realsense_depth.fps in "            + path);
    if (!node["align_to_color"]) throw std::runtime_error("missing key: realsense_depth.align_to_color in " + path);

    const YAML::Node sf = node["spatial_filter"];
    if (!sf)                   throw std::runtime_error("missing key: realsense_depth.spatial_filter in "              + path);
    if (!sf["enabled"])        throw std::runtime_error("missing key: realsense_depth.spatial_filter.enabled in "      + path);
    if (!sf["magnitude"])      throw std::runtime_error("missing key: realsense_depth.spatial_filter.magnitude in "    + path);
    if (!sf["smooth_alpha"])   throw std::runtime_error("missing key: realsense_depth.spatial_filter.smooth_alpha in " + path);
    if (!sf["smooth_delta"])   throw std::runtime_error("missing key: realsense_depth.spatial_filter.smooth_delta in " + path);

    camera::RealsenseDepthConfig cfg;
    cfg.width          = node["width"].as<int>();
    cfg.height         = node["height"].as<int>();
    cfg.fps            = node["fps"].as<int>();
    cfg.align_to_color = node["align_to_color"].as<bool>();

    cfg.spatial_filter.enabled      = sf["enabled"].as<bool>();
    cfg.spatial_filter.magnitude    = sf["magnitude"].as<int>();
    cfg.spatial_filter.smooth_alpha = sf["smooth_alpha"].as<float>();
    cfg.spatial_filter.smooth_delta = sf["smooth_delta"].as<float>();

    validate_realsense_depth_config(cfg);
    return cfg;
}

}  // namespace depth_cam_stream_codec::config
