cmake_minimum_required(VERSION 3.14)

find_package(ZLIB REQUIRED)

# Now libpng will see zlib target because it's already in the project
set(PNG_SHARED OFF CACHE BOOL "" FORCE)
set(PNG_STATIC ON CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(lpng1647)

add_subdirectory(json-develop)

# Optional: CImg
set(VENDOR_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng1647
    ${CMAKE_CURRENT_SOURCE_DIR}/CImg
    ${CMAKE_CURRENT_SOURCE_DIR}/json-develop
    CACHE INTERNAL "Vendor include directories"
)

# Create one target to hold everything
add_library(vendor_dependencies INTERFACE)
target_link_libraries(vendor_dependencies INTERFACE ZLIB::ZLIB)
target_link_libraries(vendor_dependencies INTERFACE nlohmann_json::nlohmann_json png_static)
target_include_directories(vendor_dependencies INTERFACE ${VENDOR_INCLUDE_DIRS})