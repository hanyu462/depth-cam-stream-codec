#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "depth_cam_stream_codec/camera/realsense_pipeline_config.hpp"
#include "depth_cam_stream_codec/common/frame_buffers.hpp"

namespace depth_cam_stream_codec::camera {

class RealSensePipeline {
public:
    RealSensePipeline(RealsensePipelineConfig           config,
                      std::shared_ptr<ColorFrameBuffer> color_buffer,
                      std::shared_ptr<DepthFrameBuffer> depth_buffer);
    ~RealSensePipeline();

    void start();
    void stop();

private:
    void run();

    RealsensePipelineConfig           config_;
    std::shared_ptr<ColorFrameBuffer> color_buffer_;
    std::shared_ptr<DepthFrameBuffer> depth_buffer_;
    std::atomic<bool>                 running_{false};
    std::thread                       worker_;
};

}  // namespace depth_cam_stream_codec::camera
