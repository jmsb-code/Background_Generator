set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_library(vendor INTERFACE)

add_subdirectory(json-develop)


add_subdirectory(zlib-1.3.1)

set(ZLIB_LIBRARY zlibstatic)
set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/zlib-1.3.1")

add_subdirectory(lpng1647)

target_link_libraries(vendor INTERFACE nlohmann_json::nlohmann_json png_static zlibstatic)
target_include_directories(vendor INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CImg)
