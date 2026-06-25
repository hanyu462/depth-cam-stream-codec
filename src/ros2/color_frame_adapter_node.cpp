#include "depth_cam_stream_codec/ros2/color_frame_adapter_node.hpp"

#include "depth_cam_stream_codec/ros2/color_frame_adapter.hpp"

namespace depth_cam_stream_codec::ros2 {

ColorFrameAdapterNode::ColorFrameAdapterNode(std::shared_ptr<ColorFrameBuffer>   buffer,
                                             const ColorFrameAdapterConfig& config)
    : rclcpp::Node("color_frame_adapter_node")
    , buffer_(std::move(buffer))
{
    pub_ = create_publisher<sensor_msgs::msg::Image>(config.topic, config.queue_size);

    timer_ = create_wall_timer(
        std::chrono::milliseconds(config.timer_ms),
        [this] { publish_latest(); });
}

void ColorFrameAdapterNode::publish_latest()
{
    auto snap = buffer_->read_if_new(last_seq_);
    if (!snap) return;

    last_seq_ = snap->sequence;
    pub_->publish(to_ros(*snap->value));
}

}  // namespace depth_cam_stream_codec::ros2
