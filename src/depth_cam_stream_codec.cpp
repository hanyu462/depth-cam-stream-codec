#include "depth_cam_stream_codec/depth_cam_stream_codec.hpp"

#include <thread>

#include <rclcpp/executors/single_threaded_executor.hpp>
#include <rclcpp/rclcpp.hpp>

#include "depth_cam_stream_codec/config/decoder_pipeline_yaml.hpp"
#include "depth_cam_stream_codec/config/encoder_pipeline_yaml.hpp"
#include "depth_cam_stream_codec/config/realsense_pipeline_yaml.hpp"
#include "depth_cam_stream_codec/decoder/decoder_pipeline.hpp"
#include "depth_cam_stream_codec/encoder/encoder_pipeline.hpp"

namespace depth_cam_stream_codec {

// ─── DepthCamTransmitter ─────────────────────────────────────────────────────

struct DepthCamTransmitter::Impl {
    std::string rs_config_path;
    std::string enc_config_path;

    std::shared_ptr<rclcpp::Node>             node;
    std::shared_ptr<encoder::EncoderPipeline> pipeline;
    rclcpp::executors::SingleThreadedExecutor executor;
    std::thread                               spin_thread;
};

DepthCamTransmitter::DepthCamTransmitter(std::string rs_config_path, std::string enc_config_path)
    : impl_(std::make_unique<Impl>())
{
    impl_->rs_config_path  = std::move(rs_config_path);
    impl_->enc_config_path = std::move(enc_config_path);
}

DepthCamTransmitter::~DepthCamTransmitter()
{
    stop();
}

void DepthCamTransmitter::start()
{
    if (impl_->spin_thread.joinable()) return;

    if (!rclcpp::ok())
        rclcpp::init(0, nullptr);

    const auto rs_cfg  = config::load_realsense_pipeline_config(impl_->rs_config_path);
    const auto enc_cfg = config::load_encoder_pipeline_config(impl_->enc_config_path);

    impl_->node     = rclcpp::Node::make_shared("depth_cam_transmitter");
    impl_->pipeline = std::make_shared<encoder::EncoderPipeline>(rs_cfg, enc_cfg, impl_->node);

    impl_->executor.add_node(impl_->node);
    impl_->pipeline->start();

    impl_->spin_thread = std::thread([this] { impl_->executor.spin(); });
}

void DepthCamTransmitter::stop()
{
    if (!impl_->spin_thread.joinable()) return;

    impl_->pipeline->stop();
    impl_->executor.cancel();
    impl_->spin_thread.join();

    impl_->executor.remove_node(impl_->node);
    impl_->pipeline.reset();
    impl_->node.reset();
}

// ─── DepthCamReceiver ────────────────────────────────────────────────────────

struct DepthCamReceiver::Impl {
    std::string config_path;

    std::shared_ptr<rclcpp::Node>             node;
    std::shared_ptr<decoder::DecoderPipeline> pipeline;
    rclcpp::executors::SingleThreadedExecutor executor;
    std::thread                               spin_thread;

    std::uint64_t last_seq{0};
};

DepthCamReceiver::DepthCamReceiver(std::string config_path)
    : impl_(std::make_unique<Impl>())
{
    impl_->config_path = std::move(config_path);
}

DepthCamReceiver::~DepthCamReceiver()
{
    stop();
}

void DepthCamReceiver::start()
{
    if (impl_->spin_thread.joinable()) return;

    if (!rclcpp::ok())
        rclcpp::init(0, nullptr);

    const auto cfg  = config::load_decoder_pipeline_config(impl_->config_path);
    impl_->node     = rclcpp::Node::make_shared("depth_cam_receiver");
    impl_->pipeline = std::make_shared<decoder::DecoderPipeline>(cfg, impl_->node);

    impl_->executor.add_node(impl_->node);
    impl_->pipeline->start();

    impl_->spin_thread = std::thread([this] { impl_->executor.spin(); });
}

void DepthCamReceiver::stop()
{
    if (!impl_->spin_thread.joinable()) return;

    impl_->executor.cancel();
    impl_->spin_thread.join();

    impl_->pipeline->stop();
    impl_->executor.remove_node(impl_->node);
    impl_->pipeline.reset();
    impl_->node.reset();
    impl_->last_seq = 0;
}

std::optional<common::AlignedFrame> DepthCamReceiver::next_frame(
    std::chrono::milliseconds timeout)
{
    if (!impl_->pipeline) return std::nullopt;

    auto snap = impl_->pipeline->aligned_buf()->wait_for_new(impl_->last_seq, timeout);
    if (!snap) return std::nullopt;

    impl_->last_seq = snap->sequence;
    return *snap->value;
}

}  // namespace depth_cam_stream_codec
