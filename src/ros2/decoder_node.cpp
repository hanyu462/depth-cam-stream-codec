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

        auto node     = rclcpp::Node::make_shared("decoder_node");
        auto pipeline = std::make_shared<decoder::DecoderPipeline>(cfg, node);

        auto color_pub = node->create_publisher<sensor_msgs::msg::Image>(
            "/camera/color/image_decoded", 10);
        auto depth_pub = node->create_publisher<sensor_msgs::msg::Image>(
            "/camera/depth/image_decoded", 10);

        std::atomic<bool> running{true};

        std::thread t_pub([&] {
            std::uint64_t seq = 0;
            while (running) {
                auto snap = pipeline->aligned_buf()->wait_for_new(seq);
                if (!snap) continue;
                seq = snap->sequence;
                color_pub->publish(ros2::convert_color_frame_to_ros(snap->value->color));
                depth_pub->publish(ros2::convert_depth_frame_to_ros(snap->value->depth));
            }
        });

        pipeline->start();
        rclcpp::spin(node);

        running.store(false);
        t_pub.join();
        pipeline->stop();

    } catch (const std::exception& e) {
        RCLCPP_FATAL(rclcpp::get_logger("decoder_node"), "Fatal: %s", e.what());
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown();
    return 0;
}
