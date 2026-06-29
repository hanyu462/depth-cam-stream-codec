#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(dirname "$SCRIPT_DIR")"

source /opt/ros/humble/setup.bash
source "$REPO_DIR/install/setup.bash"

exec ros2 run depth_cam_stream_codec te-4 \
    "$REPO_DIR/config/realsense_pipeline.yaml" \
    "$REPO_DIR/config/encoder_pipeline.yaml"
