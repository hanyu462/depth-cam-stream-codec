#include "depth_cam_stream_codec/camera/realsense_color_capture.hpp"

#include <cstring>
#include <librealsense2/rs.hpp>
#include <stdexcept>

namespace depth_cam_stream_codec::camera {

RealSenseColorCapture::RealSenseColorCapture(std::shared_ptr<ColorFrameBuffer> buffer,
                                             RealsenseColorConfig         config)
    : buffer_(std::move(buffer))
    , config_(config)
{}

RealSenseColorCapture::~RealSenseColorCapture()
{
    stop();
}

void RealSenseColorCapture::start()
{
    if (running_) return;
    running_ = true;
    worker_  = std::thread([this] { run(); });
}

void RealSenseColorCapture::stop()
{
    running_ = false;
    if (worker_.joinable()) worker_.join();
}

void RealSenseColorCapture::run()
{
    rs2::pipeline pipe;
    rs2::config   cfg;
    cfg.enable_stream(RS2_STREAM_COLOR, config_.width, config_.height,
                      RS2_FORMAT_BGR8, config_.fps);

    pipe.start(cfg);

    while (running_) {
        rs2::frameset frames;
        if (!pipe.try_wait_for_frames(&frames, 100)) continue;

        rs2::video_frame vf = frames.get_color_frame();
        if (!vf) continue;

        const int w      = vf.get_width();
        const int h      = vf.get_height();
        const int stride = vf.get_stride_in_bytes();
        const int size   = vf.get_data_size();

        if (w <= 0 || h <= 0 || stride <= 0 || size == 0) {
            std::fprintf(stderr, "Invalid RealSense color frame dimensions — skipping\n");
            continue;
        }

        common::ColorFrame frame;
        frame.width        = w;
        frame.height       = h;
        frame.stride_bytes = stride;
        frame.stamp_ns     = static_cast<std::int64_t>(vf.get_timestamp() * 1'000'000.0);
        frame.frame_id     = "camera_color_optical_frame";

        frame.data.resize(size);
        std::memcpy(frame.data.data(), vf.get_data(), size);

        buffer_->write(std::move(frame));
    }

    pipe.stop();
}

}  // namespace depth_cam_stream_codec::camera
