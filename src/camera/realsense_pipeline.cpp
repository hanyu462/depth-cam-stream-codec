#include "depth_cam_stream_codec/camera/realsense_pipeline.hpp"

#include <cstdio>
#include <cstring>
#include <optional>

#include <librealsense2/rs.hpp>

namespace depth_cam_stream_codec::camera {

RealSensePipeline::RealSensePipeline(RealsensePipelineConfig           config,
                                     std::shared_ptr<ColorFrameBuffer> color_buffer,
                                     std::shared_ptr<DepthFrameBuffer> depth_buffer)
    : config_(std::move(config))
    , color_buffer_(std::move(color_buffer))
    , depth_buffer_(std::move(depth_buffer))
{}

RealSensePipeline::~RealSensePipeline()
{
    stop();
}

void RealSensePipeline::start()
{
    if (running_) return;
    running_ = true;
    worker_  = std::thread([this] { run(); });
}

void RealSensePipeline::stop()
{
    running_ = false;
    if (worker_.joinable()) worker_.join();
}

void RealSensePipeline::run()
{
    rs2::pipeline pipe;
    rs2::config   cfg;

    if (config_.color)
        cfg.enable_stream(RS2_STREAM_COLOR,
                          config_.color->width, config_.color->height,
                          RS2_FORMAT_BGR8, config_.color->fps);

    if (config_.depth) {
        cfg.enable_stream(RS2_STREAM_DEPTH,
                          config_.depth->width, config_.depth->height,
                          RS2_FORMAT_Z16, config_.depth->fps);
        // align_to_color 이지만 color 스트림이 설정에 없는 경우 — align 대상으로만 오픈
        if (config_.depth->align_to_color && !config_.color)
            cfg.enable_stream(RS2_STREAM_COLOR,
                              config_.depth->width, config_.depth->height,
                              RS2_FORMAT_BGR8, config_.depth->fps);
    }

    rs2::pipeline_profile profile = pipe.start(cfg);

    float depth_scale = 0.001f;
    if (config_.depth) {
        try {
            depth_scale = profile.get_device()
                              .first<rs2::depth_sensor>()
                              .get_depth_scale();
        } catch (...) {}
    }

    std::optional<rs2::align> align_filter;
    if (config_.depth && config_.depth->align_to_color)
        align_filter.emplace(RS2_STREAM_COLOR);

    std::optional<rs2::spatial_filter> spatial_filter;
    if (config_.depth && config_.depth->spatial_filter) {
        const auto& sf = *config_.depth->spatial_filter;
        spatial_filter.emplace();
        spatial_filter->set_option(RS2_OPTION_FILTER_MAGNITUDE,    static_cast<float>(sf.magnitude));
        spatial_filter->set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, sf.smooth_alpha);
        spatial_filter->set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, sf.smooth_delta);
    }

    while (running_) {
        rs2::frameset frames;
        if (!pipe.try_wait_for_frames(&frames, 100)) continue;

        const uint64_t sequence = frameset_sequence_.fetch_add(1);
        rs2::frameset processed = align_filter ? align_filter->process(frames) : frames;

        if (color_buffer_ && config_.color) {
            rs2::video_frame vf = processed.get_color_frame();
            if (vf) {
                const int w      = vf.get_width();
                const int h      = vf.get_height();
                const int stride = vf.get_stride_in_bytes();
                const int size   = vf.get_data_size();

                if (w <= 0 || h <= 0 || stride <= 0 || size == 0) {
                    std::fprintf(stderr, "Invalid RealSense color frame dimensions — skipping\n");
                } else {
                    common::ColorFrame frame;
                    frame.width        = w;
                    frame.height       = h;
                    frame.stride_bytes = stride;
                    frame.sequence     = sequence;
                    frame.stamp_ns     = static_cast<std::int64_t>(vf.get_timestamp() * 1'000'000.0);
                    frame.frame_id     = config_.color->frame_id;
                    frame.data.resize(size);
                    std::memcpy(frame.data.data(), vf.get_data(), size);
                    color_buffer_->write(std::move(frame));
                }
            }
        }

        if (depth_buffer_ && config_.depth) {
            rs2::depth_frame df = processed.get_depth_frame();
            if (df && spatial_filter)
                df = spatial_filter->process(df);
            if (df) {
                const int w      = df.get_width();
                const int h      = df.get_height();
                const int stride = df.get_stride_in_bytes();
                const int size   = df.get_data_size();

                if (w <= 0 || h <= 0 || stride <= 0 || size == 0) {
                    std::fprintf(stderr, "Invalid RealSense depth frame dimensions — skipping\n");
                } else {
                    common::DepthFrame frame;
                    frame.width        = w;
                    frame.height       = h;
                    frame.stride_bytes = stride;
                    frame.sequence     = sequence;
                    frame.stamp_ns     = static_cast<std::int64_t>(df.get_timestamp() * 1'000'000.0);
                    frame.frame_id     = config_.depth->frame_id;
                    frame.depth_scale  = depth_scale;
                    frame.data.resize(size);
                    std::memcpy(frame.data.data(), df.get_data(), size);
                    depth_buffer_->write(std::move(frame));
                }
            }
        }
    }

    pipe.stop();
}

}  // namespace depth_cam_stream_codec::camera
