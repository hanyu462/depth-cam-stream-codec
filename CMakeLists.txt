cmake_minimum_required(VERSION 3.16)
project(depth_cam_stream_codec CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp      REQUIRED)
find_package(sensor_msgs REQUIRED)
# Debian/Ubuntu multi-arch 설치 경로를 cmake 탐색 경로에 추가
list(APPEND CMAKE_PREFIX_PATH /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE})
find_package(realsense2  REQUIRED)
find_package(yaml-cpp    REQUIRED)

# ── core library ─────────────────────────────────────────────────────────────
add_library(depth_cam_stream_codec_core
    src/camera/realsense_color_capture.cpp
    src/camera/realsense_color_config.cpp
    src/config/realsense_color_yaml.cpp
    src/camera/realsense_depth_capture.cpp
    src/camera/realsense_depth_config.cpp
    src/config/realsense_depth_yaml.cpp
    src/camera/realsense_pipeline_config.cpp
    src/camera/realsense_pipeline.cpp
    src/config/realsense_pipeline_yaml.cpp
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
    src/ros2/depth_frame_adapter.cpp
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

add_executable(te-2 test/te-2.cpp)
target_link_libraries(te-2 PRIVATE depth_cam_stream_codec_ros2)

add_executable(te-3 test/te-3.cpp)
target_link_libraries(te-3 PRIVATE depth_cam_stream_codec_ros2)

install(TARGETS te-1 te-2 te-3
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()
