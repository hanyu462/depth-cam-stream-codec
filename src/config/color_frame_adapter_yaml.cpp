#include "depth_cam_stream_codec/config/color_frame_adapter_yaml.hpp"

#include <stdexcept>
#include <yaml-cpp/yaml.h>

#include "depth_cam_stream_codec/ros2/color_frame_adapter_config.hpp"

namespace depth_cam_stream_codec::config {

ros2::ColorFrameAdapterConfig load_color_frame_adapter_config(const std::string& path)
{
    YAML::Node root;
    try {
        root = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("failed to load config: " + std::string(e.what()));
    }

    const YAML::Node node = root["color_frame_adapter"];
    if (!node)
        throw std::runtime_error("missing key: color_frame_adapter");

    ros2::ColorFrameAdapterConfig cfg;
    cfg.topic      = node["topic"].as<std::string>(cfg.topic);
    cfg.queue_size = node["queue_size"].as<int>(cfg.queue_size);
    cfg.timer_ms   = node["timer_ms"].as<std::uint32_t>(cfg.timer_ms);

    validate_color_frame_adapter_config(cfg);
    return cfg;
}

}  // namespace depth_cam_stream_codec::config
