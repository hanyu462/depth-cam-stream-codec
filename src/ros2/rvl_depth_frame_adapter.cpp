#include "depth_cam_stream_codec/ros2/rvl_depth_frame_adapter.hpp"

namespace depth_cam_stream_codec::ros2 {

depth_cam_stream_codec::msg::CompressedDepthFrame
convert_rvl_depth_frame_to_ros(const codec::RVLDepthFrame& frame)
{
    depth_cam_stream_codec::msg::CompressedDepthFrame msg;

    msg.header.stamp.sec     = static_cast<int32_t>(frame.stamp_ns / 1'000'000'000LL);
    msg.header.stamp.nanosec = static_cast<uint32_t>(frame.stamp_ns % 1'000'000'000LL);
    msg.header.frame_id      = frame.frame_id;
    msg.sequence             = frame.sequence;
    msg.width                = static_cast<uint32_t>(frame.width);
    msg.height               = static_cast<uint32_t>(frame.height);
    msg.format               = "rvl";
    msg.depth_scale          = frame.depth_scale;
    msg.data                 = frame.data;

    return msg;
}

codec::RVLDepthFrame
convert_ros_to_rvl_depth_frame(const depth_cam_stream_codec::msg::CompressedDepthFrame& msg)
{
    codec::RVLDepthFrame frame;

    frame.stamp_ns    = static_cast<int64_t>(msg.header.stamp.sec) * 1'000'000'000LL
                      + static_cast<int64_t>(msg.header.stamp.nanosec);
    frame.frame_id    = msg.header.frame_id;
    frame.sequence    = msg.sequence;
    frame.width       = static_cast<int>(msg.width);
    frame.height      = static_cast<int>(msg.height);
    frame.depth_scale = msg.depth_scale;
    frame.data        = msg.data;

    return frame;
}

}  // namespace depth_cam_stream_codec::ros2
