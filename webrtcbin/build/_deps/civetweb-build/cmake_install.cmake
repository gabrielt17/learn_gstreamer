# Install script for directory: /workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "civetweb-cmake-config" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/civetweb/civetweb-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/civetweb/civetweb-targets.cmake"
         "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build/CMakeFiles/Export/2f29a88fd316589aa7fcea0e48061985/civetweb-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/civetweb/civetweb-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/civetweb/civetweb-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/civetweb" TYPE FILE FILES "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build/CMakeFiles/Export/2f29a88fd316589aa7fcea0e48061985/civetweb-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/civetweb" TYPE FILE FILES "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build/CMakeFiles/Export/2f29a88fd316589aa7fcea0e48061985/civetweb-targets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" TYPE FILE FILES "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build/civetweb.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" TYPE FILE FILES "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build/civetweb-cpp.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "civetweb-cmake-config" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/civetweb" TYPE FILE FILES
    "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build/civetweb-config.cmake"
    "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build/civetweb-config-version.cmake"
    "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-src/cmake/FindLibDl.cmake"
    "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-src/cmake/FindLibRt.cmake"
    "/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-src/cmake/FindWinSock.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build/src/cmake_install.cmake")
  include("/workspaces/learn_gstreamer/webrtcbin/build/_deps/civetweb-build/unittest/cmake_install.cmake")

endif()

