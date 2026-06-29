# depth-cam-stream-codec

ROS2 Humble package for streaming Intel RealSense depth camera frames over a network with lightweight compression.

- **Color** — H.264 (Annex-B) via x264
- **Depth** — RVL (Run-Length Variable-Length) lossless

---

## Architecture

![Architecture](docs/depth-cam-stream-codec-architecture.png)

---

## Dependencies

| Package | Purpose | Tx | Rx |
|---------|---------|:--:|:--:|
| ROS2 Humble | middleware | ✓ | ✓ |
| librealsense2 | RealSense SDK | ✓ | |
| x264 | H.264 encoder | ✓ | |
| libavcodec | H.264 decoder | | ✓ |
| libavutil | AVPacket / AVFrame utilities | | ✓ |
| libswscale | YUV420P → BGR8 conversion | | ✓ |
| yaml-cpp | config loading | ✓ | ✓ |

```bash
sudo apt install libx264-dev libavcodec-dev libavutil-dev libswscale-dev libyaml-cpp-dev
```

RealSense SDK: follow the [official install guide](https://github.com/IntelRealSense/librealsense/blob/master/doc/installation.md).

---

## Build

```bash
source /opt/ros/humble/setup.bash
colcon build --packages-select depth_cam_stream_codec --symlink-install
source install/setup.bash
```

---

## Configuration

See [docs/configuration.md](docs/configuration.md) for all config options.

---

