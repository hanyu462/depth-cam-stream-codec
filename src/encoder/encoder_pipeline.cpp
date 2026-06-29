#include "depth_cam_stream_codec/encoder/encoder_pipeline.hpp"

#include <stdexcept>

#include "depth_cam_stream_codec/encoder/encoder_config.hpp"
#include "depth_cam_stream_codec/ros2/h264_color_frame_adapter.hpp"
#include "depth_cam_stream_codec/ros2/rvl_depth_frame_adapter.hpp"

namespace depth_cam_stream_codec::encoder {

EncoderPipeline::EncoderPipeline(
    const camera::RealsensePipelineConfig& rs_cfg,
    const EncoderPipelineConfig&           enc_cfg,
    rclcpp::Node::SharedPtr                node)
{
    if (!rs_cfg.color)
        throw std::runtime_error("EncoderPipeline: color config required");
    if (!enc_cfg.color)
        throw std::runtime_error("EncoderPipeline: encoder color config required");
    if (!rs_cfg.depth)
        throw std::runtime_error("EncoderPipeline: depth config required");

    enc_cfg_     = enc_cfg;
    color_buf_   = std::make_shared<camera::ColorFrameBuffer>();
    depth_buf_   = std::make_shared<camera::DepthFrameBuffer>();
    rs_pipeline_ = std::make_shared<camera::RealSensePipeline>(rs_cfg, color_buf_, depth_buf_);

    codec::H264EncoderConfig h264_cfg;
    h264_cfg.width             = rs_cfg.color->width;
    h264_cfg.height            = rs_cfg.color->height;
    h264_cfg.fps               = rs_cfg.color->fps;
    h264_cfg.bitrate_kbps      = enc_cfg.color->bitrate_kbps;
    h264_cfg.preset            = enc_cfg.color->preset;
    h264_cfg.tune              = enc_cfg.color->tune;
    h264_cfg.profile           = enc_cfg.color->profile;
    h264_cfg.keyframe_interval = enc_cfg.color->keyframe_interval;
    h264_enc_.emplace(h264_cfg);

    color_pub_ = node->create_publisher<depth_cam_stream_codec::msg::CompressedColorFrame>(enc_cfg.color->topic, 10);
    depth_pub_ = node->create_publisher<depth_cam_stream_codec::msg::CompressedDepthFrame>(enc_cfg.depth->topic, 10);
}

EncoderPipeline::~EncoderPipeline()
{
    stop();
}

void EncoderPipeline::start()
{
    if (running_.exchange(true)) return;

    rs_pipeline_->start();
    t_color_ = std::thread([this] { color_loop(); });
    t_depth_ = std::thread([this] { depth_loop(); });
}

void EncoderPipeline::stop()
{
    running_.store(false);

    if (t_color_.joinable()) t_color_.join();
    if (t_depth_.joinable()) t_depth_.join();
    rs_pipeline_->stop();
}

void EncoderPipeline::color_loop()
{
    std::uint64_t seq = 0;

    while (running_) {
        auto snap = color_buf_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        common::ColorFrame frame = *snap->value;
        frame.frame_id = enc_cfg_.color->frame_id;
        if (auto compressed = h264_enc_->encode(frame))
            color_pub_->publish(ros2::convert_h264_color_frame_to_ros(*compressed));
    }
}

void EncoderPipeline::depth_loop()
{
    std::uint64_t seq = 0;

    while (running_) {
        auto snap = depth_buf_->wait_for_new(seq);
        if (!snap) continue;

        seq = snap->sequence;
        common::DepthFrame frame = *snap->value;
        frame.frame_id = enc_cfg_.depth->frame_id;
        depth_pub_->publish(ros2::convert_rvl_depth_frame_to_ros(rvl_enc_.encode(frame)));
    }
}

}  // namespace depth_cam_stream_codec::encoder
