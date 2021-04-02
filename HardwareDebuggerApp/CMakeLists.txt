set(EXE_NAME HardwareDebuggerApp)
set(ROOT_DIR "${CMAKE_SOURCE_DIR}/${EXE_NAME}")


exec_program(py ARGS "\"${CMAKE_SOURCE_DIR}/Tools/GenerateSources.py\" \"${ROOT_DIR}/src\" \"${ROOT_DIR}/_Sources.cmake\" \"// [source]\"")
include("_Sources.cmake")

add_compile_definitions("_CRT_SECURE_NO_WARNINGS")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/ImGui)

include("_SDL_Options.cmake")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL)

add_executable(${EXE_NAME} ${headers} ${sources})

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
  "src/Utils.hpp"
)

if (WIN32)
  target_compile_definitions(${EXE_NAME} PRIVATE "HWD_PLATFORM_WINDOWS")
elseif(UNIX)
  target_compile_definitions(${EXE_NAME} PRIVATE "HWD_PLATFORM_LINUX")
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

target_include_directories(${EXE_NAME} PRIVATE src)
target_include_directories(${EXE_NAME} SYSTEM PRIVATE vendor)
target_include_directories(${EXE_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/ImGui"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua/src"
)

if(MSVC)
  target_compile_options(${EXE_NAME} PRIVATE "/MP")
endif()

target_link_libraries(${EXE_NAME} PRIVATE project_options project_warnings SDL2-static SDL2main Lua)


string(REGEX REPLACE "/W[3|4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# copy dlls
#add_custom_command(
#    TARGET ${EXE_NAME}
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    "$<TARGET_FILE_DIR:${EXE_NAME}>/vendor/SDL/SDL2d.dll"
#    "$<TARGET_FILE_DIR:${EXE_NAME}>"
#)
