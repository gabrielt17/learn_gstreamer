# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/workspaces/learn_gstreamer/webrtcbin/build/third_party/src/check-unit-test-framework"
  "/workspaces/learn_gstreamer/webrtcbin/build/third_party/src/check-unit-test-framework-build"
  "/workspaces/learn_gstreamer/webrtcbin/build/third_party"
  "/workspaces/learn_gstreamer/webrtcbin/build/third_party/tmp"
  "/workspaces/learn_gstreamer/webrtcbin/build/third_party/src/check-unit-test-framework-stamp"
  "/workspaces/learn_gstreamer/webrtcbin/build/third_party/src"
  "/workspaces/learn_gstreamer/webrtcbin/build/third_party/src/check-unit-test-framework-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspaces/learn_gstreamer/webrtcbin/build/third_party/src/check-unit-test-framework-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspaces/learn_gstreamer/webrtcbin/build/third_party/src/check-unit-test-framework-stamp${cfgdir}") # cfgdir has leading slash
endif()
