cmake_minimum_required(VERSION 3.14)
project(Background_Generator)

# Platform-specific settings
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (UNIX)
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "-fstack-protector")
    add_definitions("-D_RELATIVE_PATH=\"../\"")
elseif(WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "/GS")
    add_definitions("-D_RELATIVE_PATH=\"../../../../\"")
endif()

add_subdirectory(vendor)
add_subdirectory(Background_Generator)

target_include_directories(Background_Generator PRIVATE ${VENDOR_INCLUDE_DIRS})
target_link_libraries(Background_Generator PRIVATE vendor_dependencies)