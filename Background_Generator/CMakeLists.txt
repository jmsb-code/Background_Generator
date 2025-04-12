add_executable(Background_Generator
    Background_Generator.cpp
    Background_Generator.h
)

target_include_directories(Background_Generator PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/zlib-1.3.1
    ${CMAKE_BINARY_DIR}/vendor/zlib-1.3.1
)

set_property(TARGET Background_Generator PROPERTY CXX_STANDARD 20)