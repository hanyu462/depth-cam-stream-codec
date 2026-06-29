# Using depth_cam_stream_codec.hpp from an External C++ Repository

`depth_cam_stream_codec.hpp` provides two facades in a single header:

| Class | Role |
|-------|------|
| `DepthCamTransmitter` | Capture RealSense → encode → publish over ROS2 |
| `DepthCamReceiver` | Subscribe ROS2 topics → decode → `AlignedFrame` |

---

## Setup

### 1. Add this package as a dependency

**`package.xml`**:
```xml
<depend>depth_cam_stream_codec</depend>
```

**`CMakeLists.txt`**:
```cmake
find_package(depth_cam_stream_codec REQUIRED)
target_link_libraries(your_target PRIVATE
    depth_cam_stream_codec::depth_cam_stream_codec_ros2
)
```

---

## DepthCamTransmitter

```cpp
#include "depth_cam_stream_codec/depth_cam_stream_codec.hpp"
using namespace depth_cam_stream_codec;

DepthCamTransmitter tx("config/realsense_pipeline.yaml", "config/encoder_pipeline.yaml");
tx.start();

// Camera capture + encoding + publishing all run in background threads.
// Your main loop continues freely.

tx.stop();
```

| Method | Description |
|--------|-------------|
| `DepthCamTransmitter(rs_config, enc_config)` | Load both config YAMLs |
| `start()` | Open camera, start encode/publish threads |
| `stop()` | Stop all threads and release camera. Idempotent |

---

## DepthCamReceiver

```cpp
#include "depth_cam_stream_codec/depth_cam_stream_codec.hpp"
using namespace depth_cam_stream_codec;

DepthCamReceiver rx("config/decoder_pipeline.yaml");
rx.start();

while (rclcpp::ok()) {
    auto frame = rx.next_frame(std::chrono::milliseconds(200));
    if (!frame) continue;

    // zero-copy wrap into cv::Mat
    cv::Mat color(frame->color.height, frame->color.width,
                  CV_8UC3, frame->color.data.data());
    cv::Mat depth(frame->depth.height, frame->depth.width,
                  CV_16UC1, frame->depth.data.data());

    // ... inference, visualization, etc.
}

rx.stop();
```

| Method | Description |
|--------|-------------|
| `DepthCamReceiver(config_path)` | Load decoder config YAML |
| `start()` | Subscribe to ROS2 topics, begin decoding |
| `stop()` | Unsubscribe, release resources. Idempotent |
| `next_frame(timeout_ms)` | Block up to `timeout` for a new `AlignedFrame`; returns `std::nullopt` on timeout |

### AlignedFrame fields

| Field | Type | Description |
|-------|------|-------------|
| `stamp_ns` | `int64_t` | Frame timestamp (nanoseconds) |
| `color.data` | `std::vector<uint8_t>` | BGR8 pixels |
| `color.width` / `color.height` | `int` | Dimensions |
| `color.stride_bytes` | `int` | Row stride in bytes |
| `depth.data` | `std::vector<uint8_t>` | Z16 (uint16) depth pixels |
| `depth.width` / `depth.height` | `int` | Dimensions |
| `depth.depth_scale` | `float` | Meters per depth unit (typically 0.001) |

---

## Notes

- Both classes call `rclcpp::init(0, nullptr)` internally if rclcpp is not yet initialized.
  If your node calls `rclcpp::init()` first, that takes precedence.
- `next_frame()` always returns the **latest** decoded pair. Frames are not queued — if your loop is slower than the camera, older frames are silently dropped.
- `stop()` is idempotent and is called automatically in the destructor.
- Config YAMLs must match between Tx and Rx (topics, resolution, fps). See [configuration.md](configuration.md).
