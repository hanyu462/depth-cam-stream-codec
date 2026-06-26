// [TE-3] Single RealSense pipeline → color + depth ROS2 publishers
//
// 목적:
//   단일 파이프라인에서 컬러와 뎁스를 동시에 캡처해
//   /camera/color/image_raw 와 /camera/depth/image_raw 로 발행한다.
//
// 실행:
//   ./te-3 [config/realsense_pipeline.yaml]
//
// 시각화:
//   rviz2 → Add → Image → /camera/color/image_raw
//   rviz2 → Add → Image → /camera/depth/image_raw

#include <chrono>
#include <cstdio>
#include <exception>
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>

#include "depth_cam_stream_codec/camera/realsense_pipeline.hpp"
#include "depth_cam_stream_codec/config/realsense_pipeline_yaml.hpp"
#include "depth_cam_stream_codec/ros2/color_frame_adapter.hpp"
#include "depth_cam_stream_codec/ros2/depth_frame_adapter.hpp"

using namespace depth_cam_stream_codec;

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    try {
        const std::string path = (argc > 1) ? argv[1] : "config/realsense_pipeline.yaml";
        const auto cfg = config::load_realsense_pipeline_config(path);

        auto color_buf = std::make_shared<camera::ColorFrameBuffer>();
        auto depth_buf = std::make_shared<camera::DepthFrameBuffer>();

        auto pipeline = std::make_shared<camera::RealSensePipeline>(cfg, color_buf, depth_buf);

        auto node      = rclcpp::Node::make_shared("te_3");
        auto color_pub = node->create_publisher<sensor_msgs::msg::Image>("/camera/color/image_raw", 10);
        auto depth_pub = node->create_publisher<sensor_msgs::msg::Image>("/camera/depth/image_raw", 10);

        std::uint64_t color_seq = 0;
        std::uint64_t depth_seq = 0;

        const int fps    = cfg.color ? cfg.color->fps : cfg.depth->fps;
        const auto timer = node->create_wall_timer(
            std::chrono::milliseconds(1000 / fps),
            [&]() {
                if (auto snap = color_buf->read_if_new(color_seq)) {
                    color_seq = snap->sequence;
                    color_pub->publish(ros2::convert_color_frame_to_ros(*snap->value));
                }
                if (auto snap = depth_buf->read_if_new(depth_seq)) {
                    depth_seq = snap->sequence;
                    if (depth_seq % 30 == 0)
                        std::printf("[te-3] color=%-6lu  depth=%-6lu  %dx%d\n",
                            color_seq, depth_seq,
                            snap->value->width, snap->value->height);
                    depth_pub->publish(ros2::convert_depth_frame_to_ros(*snap->value));
                }
            });

        pipeline->start();
        rclcpp::spin(node);
        pipeline->stop();

    } catch (const std::exception& e) {
        RCLCPP_FATAL(rclcpp::get_logger("te_3"), "Fatal: %s", e.what());
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown();
    return 0;
}
