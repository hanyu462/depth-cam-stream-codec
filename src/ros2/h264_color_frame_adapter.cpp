#include "depth_cam_stream_codec/ros2/h264_color_frame_adapter.hpp"

namespace depth_cam_stream_codec::ros2 {

sensor_msgs::msg::CompressedImage
convert_h264_color_frame_to_ros(const codec::H264ColorFrame& frame)
{
    sensor_msgs::msg::CompressedImage msg;

    msg.header.stamp.sec     = static_cast<int32_t>(frame.stamp_ns / 1'000'000'000LL);
    msg.header.stamp.nanosec = static_cast<uint32_t>(frame.stamp_ns % 1'000'000'000LL);
    msg.header.frame_id      = frame.frame_id;
    msg.format               = "h264";
    msg.data                 = frame.data;

    return msg;
}

codec::H264ColorFrame
convert_ros_to_h264_color_frame(const sensor_msgs::msg::CompressedImage& msg)
{
    codec::H264ColorFrame frame;

    frame.stamp_ns  = static_cast<int64_t>(msg.header.stamp.sec) * 1'000'000'000LL
                    + static_cast<int64_t>(msg.header.stamp.nanosec);
    frame.frame_id  = msg.header.frame_id;
    frame.data      = msg.data;

    return frame;
}

}  // namespace depth_cam_stream_codec::ros2
