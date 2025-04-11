set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_subdirectory(json-develop)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE nlohmann_json::nlohmann_json)
target_include_directories(vendor INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CImg)
