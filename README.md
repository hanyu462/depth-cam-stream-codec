# depth-cam-stream-codec

ROS2 Humble package for streaming Intel RealSense depth camera frames over a network with lightweight compression.

- **Color** — H.264 (Annex-B) via x264
- **Depth** — RVL (Run-Length Variable-Length) lossless

---

## Architecture

![Architecture](docs/depth-cam-stream-codec-architecture.png)

Frame boundaries are guaranteed by ROS2 message boundaries — no container or parser library needed on the receiver side.

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

Edit `config/realsense_pipeline.yaml` before running.

```yaml
color:
  topic:    /camera/color/image_compressed
  frame_id: camera_color_optical_frame
  width:  1280
  height:  720
  fps:      30
  h264:
    bitrate_kbps:      2000
    preset:            veryfast
    tune:              zerolatency
    profile:           baseline
    keyframe_interval: 30

depth:
  topic:    /camera/depth/image_compressed
  frame_id: camera_depth_optical_frame
  width:  1280
  height:  720
  fps:      30
  rvl: {}
```

---

