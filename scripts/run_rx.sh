#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(dirname "$SCRIPT_DIR")"

source /opt/ros/humble/setup.bash
source "$REPO_DIR/install/setup.bash"

exec ros2 run depth_cam_stream_codec decoder_node \
    "$REPO_DIR/config/decoder_pipeline.yaml"
