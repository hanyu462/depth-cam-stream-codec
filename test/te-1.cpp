// [TE-1] RealSense RGB → ColorFrame → LatestBuffer → 콘솔 출력 + ROS2 발행
//
// 목적:
//   RealSense에서 RGB 프레임을 받아 ColorFrame으로 변환한 뒤
//   콘솔로 메타데이터를 확인하고 /camera/color/image_raw 로 발행한다.
//
// 실행:
//   export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
//   ./te-1 config/realsense_color.yaml config/color_frame_adapter.yaml
//
// 시각화:
//   ros2 run rqt_image_view rqt_image_view /camera/color/image_raw

#include <chrono>
#include <cstdio>
#include <memory>
#include <thread>

#include <rclcpp/rclcpp.hpp>

#include "depth_cam_stream_codec/camera/realsense_color_capture.hpp"
#include "depth_cam_stream_codec/config/color_frame_adapter_yaml.hpp"
#include "depth_cam_stream_codec/config/realsense_color_yaml.hpp"
#include "depth_cam_stream_codec/ros2/color_frame_adapter_node.hpp"

using namespace depth_cam_stream_codec;

static void print_frame(const common::ColorFrame& f, std::uint64_t seq)
{
    std::printf("[te-1] seq=%-6lu  %dx%d  %zu bytes\n",
        seq, f.width, f.height, f.data.size());
}

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    const std::string rs_path      = (argc > 1) ? argv[1] : "config/realsense_color.yaml";
    const std::string adapter_path = (argc > 2) ? argv[2] : "config/color_frame_adapter.yaml";

    const auto rs_cfg      = config::load_realsense_color_config(rs_path);
    const auto adapter_cfg = config::load_color_frame_adapter_config(adapter_path);

    auto buffer       = std::make_shared<camera::ColorFrameBuffer>();
    auto capture      = std::make_shared<camera::RealSenseColorCapture>(buffer, rs_cfg);
    auto adapter_node = std::make_shared<ros2::ColorFrameAdapterNode>(buffer, adapter_cfg);

    capture->start();

    std::thread spin_thread([&adapter_node] {
        rclcpp::executors::SingleThreadedExecutor exec;
        exec.add_node(adapter_node);
        exec.spin();
    });

    std::uint64_t last_seq = 0;
    while (rclcpp::ok()) {
        if (auto snap = buffer->read_if_new(last_seq)) {
            last_seq = snap->sequence;
            if (last_seq % 30 == 0)
                print_frame(*snap->value, snap->sequence);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    capture->stop();
    spin_thread.join();
    rclcpp::shutdown();
    return 0;
}
