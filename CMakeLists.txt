project(tf2vulkan)
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

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

add_compile_definitions(RAD_TELEMETRY_DISABLED)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
#add_compile_definitions(_PREFAST_)
add_compile_definitions(TF2VULKAN)
add_compile_definitions(SUPPRESS_INVALID_PARAMETER_NO_INFO)
add_compile_definitions(VERSION_SAFE_STEAM_API_INTERFACES)
add_compile_definitions(CLIENT_DLL)
add_compile_definitions(TF2_SDK)
add_compile_definitions(TF_CLIENT_DLL)
add_compile_definitions(GLOWS_ENABLE)
add_compile_definitions(NO_PCH)
add_compile_definitions(_X86_)
add_compile_definitions(MOVE_CONSTRUCTOR_SUPPORT)
add_compile_definitions(USES_ECON_ITEMS)
add_compile_definitions(NO_MALLOC_OVERRIDE)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_definitions(COMPILER_GCC)  # IDK why but clang == gcc
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_definitions(COMPILER_GCC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(COMPILER_MSVC)

endif()

set(CMAKE_POSITION_INDEPENDENT_CODE True)

set(CMAKE_C_FLAGS_DEBUG_INIT -DDEBUG -D_DEBUG -O0 -ggdb -g3 -w -m32)
set(CMAKE_CXX_FLAGS_DEBUG_INIT -DDEBUG -D_DEBUG -O0 -ggdb -g3 -w -m32)

set(CMAKE_C_FLAGS_RELEASE_INIT -DNDEBUG -O3 -ggdb -g1 -w -m32)
set(CMAKE_CXX_FLAGS_RELEASE_INIT -DNDEBUG -O3 -ggdb -g1 -w -m32)

add_compile_options(-DDEBUG -D_DEBUG -O0 -ggdb -g3 -w -m32)

find_package(Vulkan REQUIRED)

add_subdirectory(TF2VulkanUtil)
#add_subdirectory(stdshader_vulkan)
add_subdirectory(shaderapivulkan)
