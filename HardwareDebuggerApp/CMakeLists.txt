set(EXE_NAME HardwareDebuggerApp)
set(ROOT_DIR "${CMAKE_SOURCE_DIR}/${EXE_NAME}")
set(CMAKE_PREFIX_PATH "C:\\Qt\\6.2.0\\msvc2019_64\\lib\\cmake")

#set(CMAKE_UNITY_BUILD true)
#set(CMAKE_UNITY_BUILD_BATCH_SIZE 16)

include("_Sources.cmake")

add_compile_definitions("_CRT_SECURE_NO_WARNINGS")

add_compile_definitions("CFXS_HWD_VERSION_STRING=\"v0.1-dev\"")
add_compile_definitions("CFXS_HWD_PROGRAM_NAME=\"CFXS Hardware Debugger \" CFXS_HWD_VERSION_STRING")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/QtAdvancedDockingSystem)

set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Qml Quick QuickControls2 Gui)

add_executable(${EXE_NAME} ${headers} ${sources} ${UI_files} ${QRC_files}
"${ROOT_DIR}/vendor/llvm/lib/Demangle/Demangle.cpp"
"${ROOT_DIR}/vendor/llvm/lib/Demangle/ItaniumDemangle.cpp")

target_precompile_headers(
  ${EXE_NAME}
  PRIVATE
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
  <filesystem>
  <cstdint>
  <cstddef>
  <cstdlib>
  "src/Compiler.hpp"
  "src/Assert.hpp"
  "src/Log/Log.hpp"
  "src/Utils.hpp")

if(WIN32)
  target_compile_definitions(${EXE_NAME} PRIVATE "CFXS_PLATFORM_WINDOWS")
elseif(UNIX)
  target_compile_definitions(${EXE_NAME} PRIVATE "CFXS_PLATFORM_LINUX")
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

if("$ENV{USERNAME}" STREQUAL "CFXS")
  target_compile_definitions(${EXE_NAME} PRIVATE "DEV_PC_CFXS")
endif()

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(${EXE_NAME} SYSTEM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/llvm/include")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/QtAdvancedDockingSystem/src")

if(MSVC)
  target_compile_options(${EXE_NAME} PRIVATE "/MP")
endif()

target_link_libraries(
  ${EXE_NAME}
  PRIVATE project_options
          project_warnings
          Lua
          qtadvanceddocking
          Qt::Core
          Qt::Widgets
          Qt::Network
          Qt::Qml
          Qt::Quick
          Qt::QuickControls2
          Qt::Gui)

if(BUILD_PROFILER)
  target_link_options(${EXE_NAME} PRIVATE "/PROFILE")
endif()

string(REGEX REPLACE "/W[3|4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# copy dlls add_custom_command( TARGET ${EXE_NAME} POST_BUILD COMMAND
# ${CMAKE_COMMAND} -E copy_if_different
# "$<TARGET_FILE_DIR:${EXE_NAME}>/vendor/SDL/SDL2d.dll"
# "$<TARGET_FILE_DIR:${EXE_NAME}>"
# )



message("Creating file copy commands for required Qt libraries and binaries to output directory....")
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\..\\..\\bin\\Qt6Core.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\..\\..\\bin\\Qt6Cored.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\..\\..\\bin\\Qt6Gui.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\..\\..\\bin\\Qt6Guid.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\..\\..\\bin\\Qt6Widgets.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\..\\..\\bin\\Qt6Widgetsd.dll $<TARGET_FILE_DIR:${EXE_NAME}>)

add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\..\\..\\plugins\\platforms\\qwindows.dll $<TARGET_FILE_DIR:${EXE_NAME}>\\platforms\\qwindows.dll)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\..\\..\\plugins\\platforms\\qwindowsd.dll $<TARGET_FILE_DIR:${EXE_NAME}>\\platforms\\qwindowsd.dll)