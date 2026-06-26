#include "depth_cam_stream_codec/config/realsense_pipeline_yaml.hpp"

#include <stdexcept>
#include <yaml-cpp/yaml.h>

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
        if (!c["width"])  throw std::runtime_error("missing key: realsense_pipeline.color.width in "  + path);
        if (!c["height"]) throw std::runtime_error("missing key: realsense_pipeline.color.height in " + path);
        if (!c["fps"])    throw std::runtime_error("missing key: realsense_pipeline.color.fps in "    + path);

        camera::PipelineColorConfig color;
        color.width  = c["width"].as<int>();
        color.height = c["height"].as<int>();
        color.fps    = c["fps"].as<int>();
        cfg.color    = color;
    }

    if (const YAML::Node d = node["depth"]) {
        if (!d["width"])          throw std::runtime_error("missing key: realsense_pipeline.depth.width in "          + path);
        if (!d["height"])         throw std::runtime_error("missing key: realsense_pipeline.depth.height in "         + path);
        if (!d["fps"])            throw std::runtime_error("missing key: realsense_pipeline.depth.fps in "            + path);
        if (!d["align_to_color"]) throw std::runtime_error("missing key: realsense_pipeline.depth.align_to_color in " + path);

        camera::PipelineDepthConfig depth;
        depth.width          = d["width"].as<int>();
        depth.height         = d["height"].as<int>();
        depth.fps            = d["fps"].as<int>();
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
