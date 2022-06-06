cmake_minimum_required(VERSION 3.15)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
# MSVC policy for static runtime
cmake_policy(SET CMP0091 NEW)

option(STATIC_RUNTIME "Build with static runtime" OFF)
option(BUILD_PROFILER "Build with msvc profiler flag" OFF)

if(STATIC_RUNTIME)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(HardwareDebuggerProject CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

add_subdirectory(CFXS_L0_Debugger)
