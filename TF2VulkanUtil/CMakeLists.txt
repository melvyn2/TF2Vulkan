project(tf2vulkanutil)
set(CMAKE_CXX_STANDARD 20)

add_library(tf2vulkanutil STATIC)

target_sources(tf2vulkanutil PUBLIC
        include/TF2Vulkan/Util/ImageManip.cpp
        include/TF2Vulkan/Util/Threads.cpp
        src/TF2Vulkan/Util/Macros.cpp
        src/TF2Vulkan/Util/interface.cpp
        src/TF2Vulkan/Util/KeyValues.cpp
        src/TF2Vulkan/Util/std_string.cpp
        src/TF2Vulkan/Util/std_utility.cpp
        src/TF2Vulkan/Util/utlsymbol.cpp)

target_include_directories(tf2vulkanutil PUBLIC include ../sdk2013/mp/src/public/)