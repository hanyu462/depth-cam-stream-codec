#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

#include "depth_cam_stream_codec/camera/realsense_color_config.hpp"
#include "depth_cam_stream_codec/common/frame_buffers.hpp"

namespace depth_cam_stream_codec::camera {

class RealSenseColorCapture {
public:
    RealSenseColorCapture(std::shared_ptr<ColorFrameBuffer> buffer,
                          RealsenseColorConfig         config);
    ~RealSenseColorCapture();

    void start();
    void stop();

private:
    void run();

    std::shared_ptr<ColorFrameBuffer> buffer_;
    RealsenseColorConfig         config_;
    std::atomic<bool>            running_{false};
    std::thread                  worker_;
};

}  // namespace depth_cam_stream_codec::camera
