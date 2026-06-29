# Configuration

## Transmitter

### `config/realsense_pipeline.yaml`

Camera hardware settings. Resolution and FPS must match what the connected RealSense device supports.

| Key | Type | Description |
|-----|------|-------------|
| `color.width` | int | Capture width in pixels |
| `color.height` | int | Capture height in pixels |
| `color.fps` | int | Capture frame rate |
| `depth.width` | int | Capture width in pixels |
| `depth.height` | int | Capture height in pixels |
| `depth.fps` | int | Capture frame rate |
| `depth.align_to_color` | bool | Align depth frame to color frame coordinate |
| `depth.spatial_filter` | optional | Enable spatial smoothing filter |
| `depth.spatial_filter.magnitude` | int | Number of filter passes (1 ~ 5) |
| `depth.spatial_filter.smooth_alpha` | float | Spatial weight for neighboring pixels (0.0 ~ 1.0) |
| `depth.spatial_filter.smooth_delta` | float | Depth difference tolerance in mm |

---

### `config/encoder_pipeline.yaml`

ROS2 publishing and codec settings. `fps` is used as the encoder's reference frame rate for rate control and GOP structure — this does not have to match the camera's actual capture rate exactly.

| Key | Type | Description |
|-----|------|-------------|
| `color.topic` | string | ROS2 topic to publish compressed color frames |
| `color.frame_id` | string | TF frame ID embedded in the message header |
| `color.fps` | int | Encoder reference frame rate |
| `color.bitrate_kbps` | int | Target bitrate in kbps (ABR mode) |
| `color.preset` | string | x264 speed/compression preset (`ultrafast` ~ `placebo`) |
| `color.tune` | string | x264 content tuning (`zerolatency`, `film`, `animation`, ...) |
| `color.profile` | string | H.264 profile (`baseline`, `main`, `high`) |
| `color.keyframe_interval` | int | IDR keyframe interval in frames |
| `depth.topic` | string | ROS2 topic to publish compressed depth frames |
| `depth.frame_id` | string | TF frame ID embedded in the message header |
| `depth.fps` | int | Encoder reference frame rate |

**`color.preset` options**

| Value | Description |
|-------|-------------|
| `ultrafast` / `superfast` | Lowest CPU, recommended for real-time streaming |
| `veryfast` ~ `medium` | Balanced |
| `slow` ~ `placebo` | Highest compression, not suitable for real-time |

**`color.tune` options**

| Value | Description |
|-------|-------------|
| `zerolatency` | Real-time streaming — removes B-frames and encoder delay |
| `film` | Live-action video |
| `animation` | Flat colors and sharp edges |
| `fastdecode` | Optimizes for decoder CPU usage |

**`color.profile` options**

| Value | Description |
|-------|-------------|
| `baseline` | Widest compatibility, no B-frames (recommended for streaming) |
| `main` | Adds B-frames |
| `high` | Adds CABAC and additional compression tools |

---

## Receiver

### `config/decoder_pipeline.yaml`

ROS2 subscription settings. Topics must match those set in `encoder_pipeline.yaml`.

| Key | Type | Description |
|-----|------|-------------|
| `color.topic` | string | ROS2 topic to subscribe for compressed color frames |
| `depth.topic` | string | ROS2 topic to subscribe for compressed depth frames |
