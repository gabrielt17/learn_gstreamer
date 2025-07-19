# webrtcbin/CMakeLists.txt - Versão Final
cmake_minimum_required(VERSION 3.14)
project(GStreamerWebRTCExample CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-webrtc-1.0 gstreamer-sdp-1.0 gobject-2.0)

# Baixa nlohmann/json
FetchContent_Declare(
  nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3
)
# Baixa civetweb
FetchContent_Declare(
  civetweb GIT_REPOSITORY https://github.com/civetweb/civetweb.git GIT_TAG v1.16
)
FetchContent_MakeAvailable(nlohmann_json civetweb)

# --- Sender ---
add_executable(sender src/sender.cpp)
target_include_directories(sender PRIVATE ${GST_INCLUDE_DIRS})
target_link_libraries(sender PRIVATE ${GST_LIBRARIES} nlohmann_json::nlohmann_json civetweb Threads::Threads)

# --- Receiver ---
add_executable(receiver src/receiver.cpp)
target_include_directories(receiver PRIVATE ${GST_INCLUDE_DIRS})
target_link_libraries(receiver PRIVATE ${GST_LIBRARIES} nlohmann_json::nlohmann_json civetweb Threads::Threads)

message(STATUS "Dependências nlohmann/json e civetweb configuradas.")