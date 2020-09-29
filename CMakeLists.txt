project(tf2vulkan)
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

if(UNIX)
    add_compile_definitions(PLATFORM_POSIX POSIX _POSIX)
    add_compile_definitions(GNUC)  # Not sure what this does, but is required
endif()
if(UNIX AND NOT APPLE AND NOT CYGWIN)
    add_compile_definitions(PLATFORM_LINUX LINUX _LINUX)
endif()
if(APPLE)
    add_compile_definitions(OSX _OSX)
endif()
if(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS_PC PLATFORM_WINDOWS WIN32 _WIN32)
endif()


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_definitions(COMPILER_GCC)  # IDK why but clang == gcc
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_definitions(COMPILER_GCC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(COMPILER_MSVC)

endif()

set(CMAKE_POSITION_INDEPENDENT_CODE True)

set(CMAKE_C_FLAGS_DEBUG_INIT -DDEBUG -D_DEBUG -O0 -ggdb -g3)
set(CMAKE_CXX_FLAGS_DEBUG_INIT -DDEBUG -D_DEBUG -O0 -ggdb -g3)

set(CMAKE_C_FLAGS_RELEASE_INIT -DNDEBUG -O3 -ggdb -g1)
set(CMAKE_CXX_FLAGS_RELEASE_INIT -DNDEBUG -O3 -ggdb -g1)

set(CMAKE_C_FLAGS "-w -m32")
set(CMAKE_CXX_FLAGS "-w -m32")


add_subdirectory(TF2VulkanUtil)
add_subdirectory(stdshader_vulkan)