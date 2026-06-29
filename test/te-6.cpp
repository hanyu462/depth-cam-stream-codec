// [TE-6] aligned_buf API usage — RViz2 visualization
//
// DecoderPipeline을 라이브러리로 직접 사용하는 예제.
// aligned_buf()에서 AlignedFrame을 꺼내 sensor_msgs/Image로 변환하여 발행한다.
//
// Usage:
//   ./te-6 [config/decoder_pipeline.yaml]
//
// Publishes:
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

        auto node     = rclcpp::Node::make_shared("te_6");
        auto pipeline = std::make_shared<decoder::DecoderPipeline>(cfg, node);

        auto color_pub = node->create_publisher<sensor_msgs::msg::Image>(
            "/camera/color/h264_recovered", 10);
        auto depth_pub = node->create_publisher<sensor_msgs::msg::Image>(
            "/camera/depth/rvl_recovered", 10);

        std::atomic<bool> running{true};

        // aligned_buf에서 꺼낸 AlignedFrame을 sensor_msgs/Image로 변환하여 발행
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
        RCLCPP_FATAL(rclcpp::get_logger("te_6"), "Fatal: %s", e.what());
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown();
    return 0;
}
