cmake_minimum_required(VERSION 3.25)
project(DerTondehrGSD VERSION 0.1.4)

# Development builds stay in the build output. Deployment is performed only
# by the explicit installer step to the canonical Program Files VST3 folder;
# otherwise iPlug's default local deployment creates a second plugin with the
# same VST3 class ID and hosts can load either copy.
set(IPLUG_DEPLOY_PLUGINS OFF CACHE BOOL "Deploy built plugins to system directories" FORCE)

if(NOT DEFINED IPLUG2_DIR)
  set(IPLUG2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/iPlug2" CACHE PATH "iPlug2 root directory")
endif()

include(${IPLUG2_DIR}/iPlug2.cmake)
find_package(iPlug2 REQUIRED)

# Embed the font inside the VST3 bundle. Without it, NanoVG asserts as soon
# as the host opens the editor because the default text style has no font.
if(WIN32)
  set_source_files_properties(resources/main.rc PROPERTIES
    COMPILE_FLAGS "/I\"${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts\" /I\"${CMAKE_CURRENT_SOURCE_DIR}/resources\"")
endif()

# Independent circuit-inspired overdrive VST3.
iplug_add_plugin(${PROJECT_NAME}
  SOURCES
    TSCProbe.cpp
    TSCProbe.h
    resources/resource.h
    resources/main.rc
  RESOURCES
    resources/fonts/Roboto-Regular.ttf
  FORMATS
    VST3
)

# Keep the CMake target identifier shell-friendly while giving the produced
# VST3 module the exact public product name.
if(TARGET DerTondehrGSD-vst3)
  set_target_properties(DerTondehrGSD-vst3 PROPERTIES OUTPUT_NAME "Der Tondehr GSD")
endif()
