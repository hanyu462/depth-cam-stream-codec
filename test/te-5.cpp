// [TE-5] Compressed stream subscriber / decoder with RViz2 visualization
//
// Usage:
//   ./te-5 [config/decoder_pipeline.yaml]
//
// Subscribes to:
//   /camera/color/image_compressed  (sensor_msgs/CompressedImage, H.264)
//   /camera/depth/image_compressed  (depth_cam_stream_codec/CompressedDepthFrame, RVL)
//
// Publishes decoded frames for RViz2:
//   /camera/color/h264_recovered  (sensor_msgs/Image, bgr8)
//   /camera/depth/rvl_recovered   (sensor_msgs/Image, 16UC1)

#include <atomic>
#include <exception>
#include <memory>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>

#include "depth_cam_stream_codec/config/decoder_pipeline_yaml.hpp"
#include "depth_cam_stream_codec/decoder/decoder_pipeline.hpp"
#include "depth_cam_stream_codec/ros2/color_frame_adapter.hpp"
#include "depth_cam_stream_codec/ros2/depth_frame_adapter.hpp"

using namespace depth_cam_stream_codec;

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    try {
        const std::string path = (argc > 1) ? argv[1] : "config/decoder_pipeline.yaml";
        const auto cfg = config::load_decoder_pipeline_config(path);

        auto node     = rclcpp::Node::make_shared("te_5");
        auto pipeline = std::make_shared<decoder::DecoderPipeline>(cfg, node);

        auto color_pub = node->create_publisher<sensor_msgs::msg::Image>(
            "/camera/color/h264_recovered", 10);
        auto depth_pub = node->create_publisher<sensor_msgs::msg::Image>(
            "/camera/depth/rvl_recovered", 10);

        std::atomic<bool> running{true};

        std::thread t_color([&] {
            std::uint64_t seq = 0;
            while (running) {
                auto snap = pipeline->color_buf()->wait_for_new(seq);
                if (!snap) continue;
                seq = snap->sequence;
                color_pub->publish(ros2::convert_color_frame_to_ros(*snap->value));
            }
        });

        std::thread t_depth([&] {
            std::uint64_t seq = 0;
            while (running) {
                auto snap = pipeline->depth_buf()->wait_for_new(seq);
                if (!snap) continue;
                seq = snap->sequence;
                depth_pub->publish(ros2::convert_depth_frame_to_ros(*snap->value));
            }
        });

        pipeline->start();
        rclcpp::spin(node);

        running.store(false);
        t_color.join();
        t_depth.join();
        pipeline->stop();

    } catch (const std::exception& e) {
        RCLCPP_FATAL(rclcpp::get_logger("te_5"), "Fatal: %s", e.what());
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown();
    return 0;
}
