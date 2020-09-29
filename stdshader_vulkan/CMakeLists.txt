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

