# webrtcbin/CMakeLists.txt - Versão com linkagem corrigida
cmake_minimum_required(VERSION 3.14)
project(GStreamerWebRTCExample CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(GST REQUIRED 
    gstreamer-1.0
    gstreamer-webrtc-1.0
    gstreamer-sdp-1.0
    gobject-2.0
)

FetchContent_Declare(
  nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# --- Sender ---
add_executable(sender src/sender.cpp)
target_include_directories(sender PRIVATE ${GST_INCLUDE_DIRS})
# CORREÇÃO: Usando o nome correto do alvo 'civetweb-static'
target_link_libraries(sender PRIVATE ${GST_LIBRARIES} nlohmann_json::nlohmann_json Threads::Threads)

# --- Receiver ---
add_executable(receiver src/receiver.cpp)
target_include_directories(receiver PRIVATE ${GST_INCLUDE_DIRS})
# CORREÇÃO: Usando o nome correto do alvo 'civetweb-static'
target_link_libraries(receiver PRIVATE ${GST_LIBRARIES} nlohmann_json::nlohmann_json Threads::Threads)

message(STATUS "Dependências nlohmann/json configuradas.")