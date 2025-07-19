# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-src")
  file(MAKE_DIRECTORY "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-src")
endif()
file(MAKE_DIRECTORY
  "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-build"
  "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-subbuild/civetweb-populate-prefix"
  "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-subbuild/civetweb-populate-prefix/tmp"
  "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src/civetweb-populate-stamp"
  "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src"
  "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src/civetweb-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src/civetweb-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/gabrielt/Documentos/Códigos/learn_gstreamer/build/_deps/civetweb-subbuild/civetweb-populate-prefix/src/civetweb-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
