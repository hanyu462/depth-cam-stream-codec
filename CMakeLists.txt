cmake_minimum_required(VERSION 3.16)
project(depth_cam_stream_codec CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp      REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(realsense2  REQUIRED)
find_package(yaml-cpp    REQUIRED)

# ── core library ─────────────────────────────────────────────────────────────
add_library(depth_cam_stream_codec_core
    src/camera/realsense_color_capture.cpp
    src/camera/realsense_color_config.cpp
    src/config/realsense_color_yaml.cpp
)

target_include_directories(depth_cam_stream_codec_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(depth_cam_stream_codec_core PUBLIC
    realsense2::realsense2
    yaml-cpp
)

# ── ros2 library ─────────────────────────────────────────────────────────────
add_library(depth_cam_stream_codec_ros2
    src/ros2/color_frame_adapter.cpp
    src/ros2/color_frame_adapter_config.cpp
    src/ros2/color_frame_adapter_node.cpp
    src/config/color_frame_adapter_yaml.cpp
)

target_link_libraries(depth_cam_stream_codec_ros2 PUBLIC
    depth_cam_stream_codec_core
)

ament_target_dependencies(depth_cam_stream_codec_ros2 PUBLIC
    rclcpp
    sensor_msgs
)

# ── tests ────────────────────────────────────────────────────────────────────
add_executable(te-1 test/te-1.cpp)
target_link_libraries(te-1 PRIVATE depth_cam_stream_codec_ros2)

install(TARGETS te-1
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()
