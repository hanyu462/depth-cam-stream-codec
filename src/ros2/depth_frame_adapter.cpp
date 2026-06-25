#include "depth_cam_stream_codec/ros2/depth_frame_adapter.hpp"

namespace depth_cam_stream_codec::ros2 {

sensor_msgs::msg::Image convert_depth_frame_to_ros(const common::DepthFrame& frame)
{
    sensor_msgs::msg::Image msg;

    msg.header.stamp.sec     = static_cast<int32_t>(frame.stamp_ns / 1'000'000'000LL);
    msg.header.stamp.nanosec = static_cast<uint32_t>(frame.stamp_ns % 1'000'000'000LL);
    msg.header.frame_id      = frame.frame_id;

    msg.width        = static_cast<uint32_t>(frame.width);
    msg.height       = static_cast<uint32_t>(frame.height);
    msg.step         = static_cast<uint32_t>(frame.stride_bytes);
    msg.encoding     = "16UC1";
    msg.is_bigendian = 0;
    msg.data         = frame.data;

    return msg;
}

}  // namespace depth_cam_stream_codec::ros2
