#include "depth_cam_stream_codec/ros2/compressed_depth_frame_adapter.hpp"

namespace depth_cam_stream_codec::ros2 {

depth_cam_stream_codec::msg::CompressedDepthFrame
convert_compressed_depth_frame_to_ros(const codec::CompressedDepthFrame& frame)
{
    depth_cam_stream_codec::msg::CompressedDepthFrame msg;

    msg.header.stamp.sec     = static_cast<int32_t>(frame.stamp_ns / 1'000'000'000LL);
    msg.header.stamp.nanosec = static_cast<uint32_t>(frame.stamp_ns % 1'000'000'000LL);
    msg.header.frame_id      = frame.frame_id;
    msg.width                = static_cast<uint32_t>(frame.width);
    msg.height               = static_cast<uint32_t>(frame.height);
    msg.format               = "rvl";
    msg.depth_scale          = frame.depth_scale;
    msg.data                 = frame.data;

    return msg;
}

}  // namespace depth_cam_stream_codec::ros2
