# Using DepthCamReceiver from an External C++ Repository

`DepthCamReceiver` is a facade that hides all ROS2 and pipeline boilerplate.
An external package only needs to include one header and call three methods.

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

### 2. Start the encoder on the robot (Tx)

```bash
ros2 run depth_cam_stream_codec encoder_node \
    config/realsense_pipeline.yaml \
    config/encoder_pipeline.yaml
```

---

## API

```cpp
#include "depth_cam_stream_codec/depth_cam_receiver.hpp"

using namespace depth_cam_stream_codec;
```

| Method | Description |
|--------|-------------|
| `DepthCamReceiver(config_path)` | Load decoder config YAML |
| `start()` | Subscribe to ROS2 topics, begin decoding |
| `stop()` | Unsubscribe, release resources |
| `next_frame(timeout_ms)` | Block up to `timeout` for a new `AlignedFrame`; returns `std::nullopt` on timeout |

`AlignedFrame` fields:

| Field | Type | Description |
|-------|------|-------------|
| `stamp_ns` | `int64_t` | Frame timestamp (nanoseconds) |
| `color.data` | `std::vector<uint8_t>` | BGR8 pixels |
| `color.width` | `int` | Width in pixels |
| `color.height` | `int` | Height in pixels |
| `color.stride_bytes` | `int` | Row stride in bytes |
| `depth.data` | `std::vector<uint8_t>` | Z16 (uint16) depth pixels |
| `depth.width` | `int` | Width in pixels |
| `depth.height` | `int` | Height in pixels |
| `depth.depth_scale` | `float` | Meters per depth unit (typically 0.001) |

---

## Minimal example

```cpp
#include <rclcpp/rclcpp.hpp>
#include "depth_cam_stream_codec/depth_cam_receiver.hpp"

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    depth_cam_stream_codec::DepthCamReceiver rx("config/decoder_pipeline.yaml");
    rx.start();

    while (rclcpp::ok()) {
        auto frame = rx.next_frame(std::chrono::milliseconds(200));
        if (!frame) continue;

        // zero-copy wrap into cv::Mat (BGR8)
        cv::Mat color(frame->color.height, frame->color.width,
                      CV_8UC3, frame->color.data.data());

        // zero-copy wrap into cv::Mat (Z16)
        cv::Mat depth(frame->depth.height, frame->depth.width,
                      CV_16UC1, frame->depth.data.data());

        // ... process color and depth ...
    }

    rx.stop();
    rclcpp::shutdown();
    return 0;
}
```

---

## Notes

- `start()` calls `rclcpp::init(0, nullptr)` internally if rclcpp is not yet initialized.
  If your node calls `rclcpp::init()` first, that takes precedence — both paths are safe.
- `next_frame()` always returns the **latest** decoded pair; frames are not queued.
  If your processing loop is slower than the camera frame rate, older frames are silently dropped.
- `stop()` is idempotent and is called automatically in the destructor.
- The config YAML must match the topics published by the encoder. See [configuration.md](configuration.md).
