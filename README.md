# depth-cam-stream-codec

ROS2 Humble package for streaming Intel RealSense depth camera frames over a network with lightweight compression.

- **Color** — H.264 (Annex-B) via x264
- **Depth** — RVL (Run-Length Variable-Length) lossless

---

## Architecture

```
[Jetson / sender]                          [PC / receiver]

RealSense SDK
    │
    ├─ ColorFrame (BGR8)                   CompressedImage ──► H264ColorDecoder
    │       └─► H264ColorEncoder ──────►  /camera/color/image_compressed
    │
    └─ DepthFrame (Z16)                    CompressedDepthFrame ──► RVLDepthDecoder
            └─► RVLDepthEncoder ────────► /camera/depth/image_compressed
```

Frame boundaries are guaranteed by ROS2 message boundaries — no container or parser library needed on the receiver side.

---

## Dependencies

### Sender (Jetson)

| Package | Purpose |
|---------|---------|
| ROS2 Humble | middleware |
| librealsense2 | RealSense SDK |
| x264 | H.264 encoder |
| yaml-cpp | config loading |

```bash
sudo apt install libx264-dev libyaml-cpp-dev
```

RealSense SDK: follow the [official install guide](https://github.com/IntelRealSense/librealsense/blob/master/doc/installation.md).

### Receiver (PC)

| Package | Purpose |
|---------|---------|
| ROS2 Humble | middleware |
| libavcodec | H.264 decoder |
| libavutil | AVPacket / AVFrame utilities |
| libswscale | YUV420P → BGR8 conversion |

```bash
sudo apt install libavcodec-dev libavutil-dev libswscale-dev
```

---

## Build

```bash
source /opt/ros/humble/setup.bash
colcon build --packages-select depth_cam_stream_codec
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

## Tests

| Executable | Description |
|------------|-------------|
| `te-4` | Encoder pipeline — captures from RealSense and publishes compressed frames |

```bash
ros2 run depth_cam_stream_codec te-4 config/realsense_pipeline.yaml
```

Verify topics:

```bash
ros2 topic hz /camera/color/image_compressed
ros2 topic hz /camera/depth/image_compressed
ros2 topic bw /camera/color/image_compressed
```

---

## ROS2 Messages

### `/camera/color/image_compressed`
`sensor_msgs/CompressedImage` — H.264 Annex-B bitstream. `header.stamp` carries the original capture timestamp.

### `/camera/depth/image_compressed`
`depth_cam_stream_codec/CompressedDepthFrame` — RVL-compressed Z16 depth. Includes `width`, `height`, and `depth_scale` (meters per unit).
