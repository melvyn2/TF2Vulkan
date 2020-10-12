PROJECT(stdshader_vulkan)

file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/HLSL/*.hlsl)
foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME_WLE)
        list(APPEND COMPILED_SHADERS  ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/generated/${SHADER_NAME}.h)
endforeach()
add_custom_command( OUTPUT ${COMPILED_SHADERS}
                    COMMAND make
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/HLSL)
add_custom_target(shaders DEPENDS ${COMPILED_SHADERS})


add_library(stdshader_vulkan STATIC)
target_sources(stdshader_vulkan PUBLIC
        src/ShaderDLL.cpp
        src/ShaderBlobs.cpp
        src/ShaderDLL.cpp
        src/ShaderDLLVerification.cpp
        src/shaders/BaseShaderNext.cpp
        src/shaders/BaseShader.cpp
        src/shaders/LightmappedGeneric.cpp
        src/shaders/PlaceholderShader.cpp
        src/shaders/ShaderComponents/BaseShaderComponent.cpp
        src/shaders/ShaderParamNext.cpp
        src/shaders/XLitGeneric.cpp)
set_source_files_properties(src/ShaderDLL.cpp PROPERTIES COMPILE_FLAGS "-std=c++2a")
add_dependencies(stdshader_vulkan shaders)
target_include_directories(stdshader_vulkan PUBLIC include
                                ./include/
                                ../TF2VulkanUtil/include/
                                ../shaderapivulkan/include/
                                ../sdk2013/mp/src/public/)