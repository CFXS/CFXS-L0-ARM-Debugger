set(EXE_NAME HardwareDebuggerApp)
set(ROOT_DIR "${CMAKE_SOURCE_DIR}/${EXE_NAME}")

exec_program(py ARGS "\"${CMAKE_SOURCE_DIR}/Tools/GenerateSources.py\" \"${ROOT_DIR}/src\" \"${ROOT_DIR}/_Sources.cmake\" \"// [source]\"")
include("_Sources.cmake")

add_executable(${EXE_NAME} ${headers} ${sources})

if(MSVC)
  target_compile_options(${EXE_NAME} PRIVATE "/MP")
endif()

target_include_directories(${EXE_NAME} PRIVATE src)
target_include_directories(${EXE_NAME} SYSTEM PRIVATE vendor)

target_link_libraries(${EXE_NAME} PRIVATE project_options project_warnings)

