set(EXE_NAME HardwareDebuggerApp)
set(ROOT_DIR "${CMAKE_SOURCE_DIR}/${EXE_NAME}")

exec_program(
  py ARGS
  "\"${CMAKE_SOURCE_DIR}/Tools/GenerateSources.py\" \"${ROOT_DIR}/src\" \"${ROOT_DIR}/_Sources.cmake\" \"// [source]\""
)

include("_Sources.cmake")

add_compile_definitions("_CRT_SECURE_NO_WARNINGS")

add_compile_definitions("CFXS_HWD_WINDOW_TITLE_NO_PREFIX=\"`\"")
add_compile_definitions("CFXS_HWD_WINDOW_TITLE_NO_PREFIX_TAG='`'")
add_compile_definitions("CFXS_HWD_WINDOW_TITLE_PREFIX=\"[CFXS HWD] \"")

add_compile_definitions("CFXS_HWD_VERSION_STRING=\"v0.1-dev\"")
add_compile_definitions(
  "CFXS_HWD_PROGRAM_NAME=\"CFXS Hardware Debugger \" CFXS_HWD_VERSION_STRING")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Network Qml Quick Gui REQUIRED)


set(UI_files "${ROOT_DIR}/src/UI/MainWindow/MainWindow.ui")

add_executable(
  ${EXE_NAME} ${headers} ${sources} ${UI_files})


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
  "src/Compiler.hpp"
  "src/Assert.hpp"
  "src/Log/Log.hpp"
  "src/Utils.hpp")

if(WIN32)
  target_compile_definitions(${EXE_NAME} PRIVATE "HWD_PLATFORM_WINDOWS")
elseif(UNIX)
  target_compile_definitions(${EXE_NAME} PRIVATE "HWD_PLATFORM_LINUX")
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

target_include_directories(${EXE_NAME} PRIVATE src)
target_include_directories(${EXE_NAME} SYSTEM PRIVATE vendor)
target_include_directories(
  ${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include")

if(MSVC)
  target_compile_options(${EXE_NAME} PRIVATE "/MP")
endif()

target_link_libraries(
  ${EXE_NAME}
  PRIVATE project_options
          project_warnings
          Lua
          Qt::Core
          Qt::Widgets
          Qt::Network
          Qt::Qml
          Qt::Quick
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
set(CMAKE_PREFIX_PATH "C:\\Qt\\5.15.2\\msvc2019_64")
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\bin\\Qt5Core.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\bin\\Qt5Cored.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\bin\\Qt5Gui.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\bin\\Qt5Guid.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\bin\\Qt5Widgets.dll $<TARGET_FILE_DIR:${EXE_NAME}>)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\bin\\Qt5Widgetsd.dll $<TARGET_FILE_DIR:${EXE_NAME}>)

add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\plugins\\platforms\\qwindows.dll $<TARGET_FILE_DIR:${EXE_NAME}>\\platforms\\qwindows.dll)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PREFIX_PATH}\\plugins\\platforms\\qwindowsd.dll $<TARGET_FILE_DIR:${EXE_NAME}>\\platforms\\qwindowsd.dll)