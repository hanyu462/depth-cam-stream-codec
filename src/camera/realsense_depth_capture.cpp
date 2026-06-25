#include "depth_cam_stream_codec/camera/realsense_depth_capture.hpp"

#include <cstdio>
#include <cstring>
#include <librealsense2/rs.hpp>

namespace depth_cam_stream_codec::camera {

RealSenseDepthCapture::RealSenseDepthCapture(std::shared_ptr<DepthFrameBuffer> buffer,
                                             RealsenseDepthConfig              config)
    : buffer_(std::move(buffer))
    , config_(config)
{}

RealSenseDepthCapture::~RealSenseDepthCapture()
{
    stop();
}

void RealSenseDepthCapture::start()
{
    if (running_) return;
    running_ = true;
    worker_  = std::thread([this] { run(); });
}

void RealSenseDepthCapture::stop()
{
    running_ = false;
    if (worker_.joinable()) worker_.join();
}

void RealSenseDepthCapture::run()
{
    rs2::pipeline pipe;
    rs2::config   cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, config_.width, config_.height,
                      RS2_FORMAT_Z16, config_.fps);
    if (config_.align_to_color)
        cfg.enable_stream(RS2_STREAM_COLOR, config_.width, config_.height,
                          RS2_FORMAT_BGR8, config_.fps);

    pipe.start(cfg);

    rs2::align           align_filter(RS2_STREAM_COLOR);
    rs2::spatial_filter  spatial_filter;
    if (config_.spatial_filter.enabled) {
        spatial_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE,    config_.spatial_filter.magnitude);
        spatial_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, config_.spatial_filter.smooth_alpha);
        spatial_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, config_.spatial_filter.smooth_delta);
    }

    while (running_) {
        rs2::frameset frames;
        if (!pipe.try_wait_for_frames(&frames, 100)) continue;

        rs2::frameset processed = config_.align_to_color
            ? align_filter.process(frames)
            : frames;

        rs2::depth_frame df = processed.get_depth_frame();
        if (df && config_.spatial_filter.enabled)
            df = spatial_filter.process(df);
        if (!df) continue;

        const int w      = df.get_width();
        const int h      = df.get_height();
        const int stride = df.get_stride_in_bytes();
        const int size   = df.get_data_size();

        if (w <= 0 || h <= 0 || stride <= 0 || size == 0) {
            std::fprintf(stderr, "Invalid RealSense depth frame dimensions — skipping\n");
            continue;
        }

        common::DepthFrame frame;
        frame.width        = w;
        frame.height       = h;
        frame.stride_bytes = stride;
        frame.stamp_ns     = static_cast<std::int64_t>(df.get_timestamp() * 1'000'000.0);
        frame.frame_id     = "camera_depth_optical_frame";

        frame.data.resize(size);
        std::memcpy(frame.data.data(), df.get_data(), size);

        buffer_->write(std::move(frame));
    }

    pipe.stop();
}

}  // namespace depth_cam_stream_codec::camera
