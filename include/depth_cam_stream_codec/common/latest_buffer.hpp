#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <utility>

namespace depth_cam_stream_codec::common {

template <typename T>
class LatestBuffer {
public:
    struct Snapshot {
        std::shared_ptr<const T> value;
        std::uint64_t sequence = 0;
    };

    void write(T value)
    {
        auto next = std::make_shared<T>(std::move(value));
        {
            std::lock_guard<std::mutex> lock(mutex_);
            latest_ = std::move(next);
            ++sequence_;
        }
        cv_.notify_all();
    }

    std::optional<Snapshot> read() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!latest_) return std::nullopt;
        return Snapshot{latest_, sequence_};
    }

    std::optional<Snapshot> read_if_new(std::uint64_t last_sequence) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!latest_ || sequence_ == last_sequence) return std::nullopt;
        return Snapshot{latest_, sequence_};
    }

    // Blocks until a frame newer than last_sequence arrives or timeout elapses.
    // Returns nullopt on timeout (use to check running_ and retry).
    std::optional<Snapshot> wait_for_new(
        std::uint64_t last_sequence,
        std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        const bool has_new = cv_.wait_for(lock, timeout,
            [&] { return sequence_ != last_sequence; });
        if (!has_new || !latest_) return std::nullopt;
        return Snapshot{latest_, sequence_};
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return !latest_;
    }

private:
    mutable std::mutex              mutex_;
    mutable std::condition_variable cv_;
    std::shared_ptr<const T>        latest_;
    std::uint64_t                   sequence_ = 0;
};

}  // namespace depth_cam_stream_codec::common
