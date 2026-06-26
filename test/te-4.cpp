// [TE-4] Compressed RealSense stream publisher
//
// Usage:
//   ./te-4 [config/realsense_pipeline.yaml]
//
// Topics:
//   /camera/color/image_compressed  (sensor_msgs/CompressedImage, format=h264)
//   /camera/depth/image_compressed  (depth_cam_stream_codec/CompressedDepthFrame, format=rvl)

#include <exception>
#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "depth_cam_stream_codec/config/realsense_pipeline_yaml.hpp"
#include "depth_cam_stream_codec/encoder/encoder_pipeline.hpp"

using namespace depth_cam_stream_codec;

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    try {
        const std::string path = (argc > 1) ? argv[1] : "config/realsense_pipeline.yaml";
        const auto cfg = config::load_realsense_pipeline_config(path);

        auto node     = rclcpp::Node::make_shared("te_4");
        auto pipeline = std::make_shared<encoder::EncoderPipeline>(cfg, node);

        pipeline->start();
        rclcpp::spin(node);
        pipeline->stop();

    } catch (const std::exception& e) {
        RCLCPP_FATAL(rclcpp::get_logger("te_4"), "Fatal: %s", e.what());
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown();
    return 0;
}
