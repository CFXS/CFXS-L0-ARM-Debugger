add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/QtAdvancedDockingSystem)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/QCodeEditor)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/QHexEdit2)

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/temp")

target_link_libraries(
  ${EXE_NAME}
  PRIVATE 
  qtadvanceddocking
  QCodeEditor
  QHexEdit2
  Lua
)