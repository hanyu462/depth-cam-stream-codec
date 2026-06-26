#include "depth_cam_stream_codec/ros2/compressed_color_frame_adapter.hpp"

namespace depth_cam_stream_codec::ros2 {

sensor_msgs::msg::CompressedImage
convert_compressed_color_frame_to_ros(const codec::CompressedColorFrame& frame)
{
    sensor_msgs::msg::CompressedImage msg;

    msg.header.stamp.sec     = static_cast<int32_t>(frame.stamp_ns / 1'000'000'000LL);
    msg.header.stamp.nanosec = static_cast<uint32_t>(frame.stamp_ns % 1'000'000'000LL);
    msg.header.frame_id      = frame.frame_id;
    msg.format               = "h264";
    msg.data                 = frame.data;

    return msg;
}

}  // namespace depth_cam_stream_codec::ros2
