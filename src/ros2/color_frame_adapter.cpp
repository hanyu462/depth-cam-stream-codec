#include "depth_cam_stream_codec/ros2/color_frame_adapter.hpp"

#include <stdexcept>

namespace depth_cam_stream_codec::ros2 {

sensor_msgs::msg::Image convert_color_frame_to_ros(const common::ColorFrame& frame)
{
    sensor_msgs::msg::Image msg;

    msg.header.stamp.sec     = static_cast<int32_t>(frame.stamp_ns / 1'000'000'000LL);
    msg.header.stamp.nanosec = static_cast<uint32_t>(frame.stamp_ns % 1'000'000'000LL);
    msg.header.frame_id      = frame.frame_id;

    msg.width       = static_cast<uint32_t>(frame.width);
    msg.height      = static_cast<uint32_t>(frame.height);
    msg.step        = static_cast<uint32_t>(frame.stride_bytes);
    msg.encoding    = "bgr8";
    msg.is_bigendian = 0;
    msg.data        = frame.data;

    return msg;
}

common::ColorFrame convert_ros_to_color_frame(const sensor_msgs::msg::Image& msg)
{
    if (msg.encoding != "bgr8")
        throw std::runtime_error("color_frame_adapter: unsupported encoding: " + msg.encoding);

    common::ColorFrame frame;
    frame.width        = static_cast<int>(msg.width);
    frame.height       = static_cast<int>(msg.height);
    frame.stride_bytes = static_cast<int>(msg.step);
    frame.stamp_ns     = static_cast<std::int64_t>(msg.header.stamp.sec) * 1'000'000'000LL
                       + static_cast<std::int64_t>(msg.header.stamp.nanosec);
    frame.frame_id     = msg.header.frame_id;
    frame.data         = msg.data;

    return frame;
}

}  // namespace depth_cam_stream_codec::ros2
