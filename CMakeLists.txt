cmake_minimum_required(VERSION 3.16)
project(depth_cam_stream_codec C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp      REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs    REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# Debian/Ubuntu multi-arch 설치 경로를 cmake 탐색 경로에 추가
list(APPEND CMAKE_PREFIX_PATH /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE})
find_package(realsense2 REQUIRED)
find_package(yaml-cpp   REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(X264    REQUIRED x264)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVUTIL  REQUIRED libavutil)
pkg_check_modules(SWSCALE REQUIRED libswscale)

# ── custom messages ───────────────────────────────────────────────────────────
rosidl_generate_interfaces(${PROJECT_NAME}
    "msg/CompressedColorFrame.msg"
    "msg/CompressedDepthFrame.msg"
    DEPENDENCIES std_msgs
)

# ── core library (camera + config, ROS 비의존) ────────────────────────────────
add_library(depth_cam_stream_codec_core
    src/camera/realsense_pipeline_config.cpp
    src/camera/realsense_pipeline.cpp
    src/config/realsense_pipeline_yaml.cpp
    src/config/decoder_pipeline_yaml.cpp
    src/config/encoder_pipeline_yaml.cpp
)

target_include_directories(depth_cam_stream_codec_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(depth_cam_stream_codec_core PUBLIC
    realsense2::realsense2
    yaml-cpp
)

# ── codec library (encoders + decoders, ROS 비의존) ──────────────────────────
add_library(depth_cam_stream_codec_codec
    src/encoder/h264_color_encoder.cpp
    src/encoder/rvl_depth_encoder.cpp
    src/decoder/rvl_depth_decoder.cpp
)

target_include_directories(depth_cam_stream_codec_codec PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${X264_INCLUDE_DIRS}
)

target_link_libraries(depth_cam_stream_codec_codec PUBLIC
    depth_cam_stream_codec_core
    ${X264_LIBRARIES}
)

# ── ros2 library ─────────────────────────────────────────────────────────────
add_library(depth_cam_stream_codec_ros2
    src/ros2/color_frame_adapter.cpp
    src/ros2/depth_frame_adapter.cpp
    src/ros2/h264_color_frame_adapter.cpp
    src/ros2/rvl_depth_frame_adapter.cpp
    src/encoder/encoder_pipeline.cpp
    src/decoder/h264_color_decoder.cpp
    src/decoder/decoder_pipeline.cpp
    src/depth_cam.cpp
)

target_include_directories(depth_cam_stream_codec_ros2 PUBLIC
    ${AVCODEC_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
    ${SWSCALE_INCLUDE_DIRS}
)

target_link_libraries(depth_cam_stream_codec_ros2 PUBLIC
    depth_cam_stream_codec_codec
    ${AVCODEC_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    ${SWSCALE_LIBRARIES}
)

ament_target_dependencies(depth_cam_stream_codec_ros2 PUBLIC
    rclcpp
    sensor_msgs
)

rosidl_get_typesupport_target(cpp_typesupport_target
    ${PROJECT_NAME} "rosidl_typesupport_cpp")
target_link_libraries(depth_cam_stream_codec_ros2 PUBLIC
    "${cpp_typesupport_target}"
)

# ── nodes ────────────────────────────────────────────────────────────────────
add_executable(encoder_node src/ros2/encoder_node.cpp)
target_link_libraries(encoder_node PRIVATE depth_cam_stream_codec_ros2)

add_executable(decoder_node src/ros2/decoder_node.cpp)
target_link_libraries(decoder_node PRIVATE depth_cam_stream_codec_ros2)

install(TARGETS encoder_node decoder_node
    DESTINATION lib/${PROJECT_NAME}
)

# ── tests ────────────────────────────────────────────────────────────────────
add_executable(te-3 test/te-3.cpp)
target_link_libraries(te-3 PRIVATE depth_cam_stream_codec_ros2)

add_executable(te-4 test/te-4.cpp)
target_link_libraries(te-4 PRIVATE depth_cam_stream_codec_ros2)

add_executable(te-5 test/te-5.cpp)
target_link_libraries(te-5 PRIVATE depth_cam_stream_codec_ros2)

add_executable(te-6 test/te-6.cpp)
target_link_libraries(te-6 PRIVATE depth_cam_stream_codec_ros2)

install(TARGETS te-3 te-4 te-5 te-6
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()
