// [TE-1] RealSense color capture → ROS2 publisher
//
// 목적:
//   RealSense에서 RGB 프레임을 캡처해 /camera/color/image_raw 로 발행한다.
//
// 실행:
//   ./te-1 [config/realsense_color.yaml]
//
// 시각화:
//   rviz2 → Add → Image → /camera/color/image_raw

#include <chrono>
#include <cstdio>
#include <exception>
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>

#include "depth_cam_stream_codec/camera/realsense_color_capture.hpp"
#include "depth_cam_stream_codec/config/realsense_color_yaml.hpp"
#include "depth_cam_stream_codec/ros2/color_frame_adapter.hpp"

using namespace depth_cam_stream_codec;

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    try {
        const std::string rs_path = (argc > 1) ? argv[1] : "config/realsense_color.yaml";
        const auto rs_cfg = config::load_realsense_color_config(rs_path);

        auto buffer  = std::make_shared<camera::ColorFrameBuffer>();
        auto capture = std::make_shared<camera::RealSenseColorCapture>(buffer, rs_cfg);

        auto node = rclcpp::Node::make_shared("te_1");
        auto pub  = node->create_publisher<sensor_msgs::msg::Image>(
            "/camera/color/image_raw", 10);

        std::uint64_t last_seq = 0;
        auto timer = node->create_wall_timer(
            std::chrono::milliseconds(33),
            [&]() {
                auto snap = buffer->read_if_new(last_seq);
                if (!snap) return;
                last_seq = snap->sequence;
                if (last_seq % 30 == 0)
                    std::printf("[te-1] seq=%-6lu  %dx%d\n",
                        last_seq, snap->value->width, snap->value->height);
                pub->publish(ros2::convert_color_frame_to_ros(*snap->value));
            });

        capture->start();
        rclcpp::spin(node);
        capture->stop();

    } catch (const std::exception& e) {
        RCLCPP_FATAL(rclcpp::get_logger("te_1"), "Fatal: %s", e.what());
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown();
    return 0;
}
