add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/QtAdvancedDockingSystem)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/QCodeEditor)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/QHexEdit2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/stlink)

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stlink/inc")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stlink/src")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stlink/src/stlink-lib")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/temp")

target_link_libraries(
  ${EXE_NAME}
  PRIVATE 
  qtadvanceddocking
  QCodeEditor
  QHexEdit2
  Lua
  stlink-static
  C:/CFXS_Projects/CFXS-L0-ARM-Debugger/build/3rdparty/libusb-1.0.25/VS2019/MS64/Release/lib/libusb-1.0.lib
)