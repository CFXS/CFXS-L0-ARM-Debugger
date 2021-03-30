set(EXE_NAME HardwareDebuggerApp)
set(ROOT_DIR "${CMAKE_SOURCE_DIR}/${EXE_NAME}")

exec_program(py ARGS "\"${CMAKE_SOURCE_DIR}/Tools/GenerateSources.py\" \"${ROOT_DIR}/src\" \"${ROOT_DIR}/_Sources.cmake\" \"// [source]\"")
include("_Sources.cmake")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/ImGui)

set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
set(SDL_STATIC_ENABLED_BY_DEFAULT ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL)

add_executable(${EXE_NAME} ${headers} ${sources})

target_include_directories(${EXE_NAME} PRIVATE src)
target_include_directories(${EXE_NAME} SYSTEM PRIVATE vendor)
target_include_directories(${EXE_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/ImGui"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua/src"
)

if(MSVC)
  target_compile_options(${EXE_NAME} PRIVATE "/MP")
endif()


target_link_libraries(${EXE_NAME} PRIVATE project_options project_warnings SDL2-static SDL2main Lua)

# copy dlls
#add_custom_command(
#    TARGET ${EXE_NAME}
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    "$<TARGET_FILE_DIR:${EXE_NAME}>/vendor/SDL/SDL2d.dll"
#    "$<TARGET_FILE_DIR:${EXE_NAME}>"
#)
