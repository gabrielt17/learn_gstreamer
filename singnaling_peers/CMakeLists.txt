# webrtcbin/CMakeLists.txt - Versão com ixwebsocket
cmake_minimum_required(VERSION 3.14)
project(GStreamerWebRTCExample CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
find_package(PkgConfig REQUIRED)

# --- Dependências de Sistema ---
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED) # Necessário para WSS (ixwebsocket usa)

# --- Dependências do GStreamer ---
pkg_check_modules(GST REQUIRED 
    gstreamer-1.0
    gstreamer-webrtc-1.0
    gstreamer-sdp-1.0
    gobject-2.0
)

# --- Dependências via FetchContent ---

# nlohmann/json
FetchContent_Declare(
  nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json) 

# ixwebsocket (substitui cpp-httplib)
FetchContent_Declare(
  ixwebsocket GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git GIT_TAG v11.4.3
)
FetchContent_MakeAvailable(ixwebsocket)

# --- Alvo Executável: sender ---
add_executable(sender src/sender.cpp)
target_include_directories(sender PRIVATE ${GST_INCLUDE_DIRS})
target_link_libraries(sender PRIVATE 
    ${GST_LIBRARIES} 
    nlohmann_json::nlohmann_json  # JSON
    ixwebsocket                  # WebSocket client
    Threads::Threads             # Threads
    OpenSSL::SSL                 # SSL para WSS
    OpenSSL::Crypto              # Crypto para WSS
)

# --- Alvo Executável: receiver ---
add_executable(receiver src/receiver.cpp)
target_include_directories(receiver PRIVATE ${GST_INCLUDE_DIRS})
target_link_libraries(receiver PRIVATE 
    ${GST_LIBRARIES} 
    nlohmann_json::nlohmann_json
    ixwebsocket
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

message(STATUS "Dependências nlohmann/json e ixwebsocket configuradas.")
