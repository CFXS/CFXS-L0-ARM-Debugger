cmake_minimum_required(VERSION 3.15)

# MSVC policy for static runtime
cmake_policy(SET CMP0091 NEW)
option(STATIC_RUNTIME "Build with static runtime" ON)

if(STATIC_RUNTIME)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(HardwareDebuggerProject CXX)

include("CMake/Settings.cmake")

# Link "library" to set C++ standard / compile options
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

# Link "library" to use warnings specified in CMake/CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# Default compiler warnings
include("CMake/CompilerWarnings.cmake")
set_project_warnings(project_warnings)

# Sanitizer options if supported by compiler
include("CMake/Sanitizers.cmake")
enable_sanitizers(project_options)

option(ENABLE_PCH "Use Precompiled Headers" ON)
if(ENABLE_PCH)
    target_precompile_headers(
        project_options
        INTERFACE
        <iostream>
        <vector>
        <string>
        <map>
        <utility>
        <memory>
        <algorithm>
        <unordered_map>
        <chrono>
        <array>
        <ostream>
        <sstream>
        <unordered_set>
    )
endif()

add_subdirectory(HardwareDebuggerApp)
