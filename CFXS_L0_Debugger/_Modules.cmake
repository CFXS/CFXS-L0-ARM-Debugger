add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/QtAdvancedDockingSystem)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/QCodeEditor)

set(LLVM_TARGETS_TO_BUILD "ARM" CACHE INTERNAL "")
set(LLVM_ENABLE_PROJECTS_USED ON CACHE INTERNAL "")
set(LLVM_ENABLE_PROJECTS "clang" CACHE INTERNAL "")
set(LLVM_INSTALL_TOOLCHAIN_ONLY OFF CACHE BOOL "")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/LLVM/llvm)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/LLVM")

find_package(LLVM REQUIRED CONFIG)

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include")

llvm_map_components_to_libnames(llvm_libs support core demangle)

target_link_libraries(
  ${EXE_NAME}
  PRIVATE 
  qtadvanceddocking
  QCodeEditor
  Lua
  ${llvm_libs}
  libclang
)