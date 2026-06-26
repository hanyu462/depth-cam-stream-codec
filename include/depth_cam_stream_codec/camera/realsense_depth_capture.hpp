#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "depth_cam_stream_codec/camera/realsense_depth_config.hpp"
#include "depth_cam_stream_codec/common/frame_buffers.hpp"

namespace depth_cam_stream_codec::camera {

class RealSenseDepthCapture {
public:
    RealSenseDepthCapture(std::shared_ptr<DepthFrameBuffer> buffer,
                          RealsenseDepthConfig              config);
    ~RealSenseDepthCapture();

    void start();
    void stop();

private:
    void run();

    std::shared_ptr<DepthFrameBuffer> buffer_;
    RealsenseDepthConfig              config_;
    std::atomic<bool>                 running_{false};
    std::thread                       worker_;
};

}  // namespace depth_cam_stream_codec::camera
