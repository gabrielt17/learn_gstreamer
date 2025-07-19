# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-src"
  "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build"
  "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-subbuild/civetweb-populate-prefix"
  "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-subbuild/civetweb-populate-prefix/tmp"
  "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src/civetweb-populate-stamp"
  "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src"
  "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src/civetweb-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src/civetweb-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src/civetweb-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
