project(shaderapivulkan)
set(CMAKE_CXX_STANDARD 20)

add_library(shaderapivulkan STATIC)
target_sources(shaderapivulkan PUBLIC
        ../sdk2013/mp/src/public/tier0/memoverride.cpp
        src/interface/internal/IShaderAPITexture.cpp
        src/interface/internal/IVulkanCommandBuffer.cpp
        include/TF2Vulkan/TextureData.cpp
        src/stdafx.cpp
        src/TF2Vulkan/BufferPool.cpp
        src/TF2Vulkan/DebugTextureInfo.cpp
        src/TF2Vulkan/Exceptions.cpp
        src/TF2Vulkan/FormatConverter.cpp
        src/TF2Vulkan/FormatInfo.cpp
        src/TF2Vulkan/GraphicsPipeline.cpp
        src/interface/internal/IVulkanQueue.cpp
        src/TF2Vulkan/IShaderAPI/IShaderAPI_MeshManager.cpp
        src/TF2Vulkan/IShaderAPI/IShaderAPI_TextureManager.cpp
        src/TF2Vulkan/MaterialSystemHardwareConfig.cpp
        src/TF2Vulkan/PixScope.cpp
        src/TF2Vulkan/IShaderAPI/ShaderAPI.cpp
        src/TF2Vulkan/RenderDocAPI.cpp
        src/TF2Vulkan/ShaderDevice.cpp
        src/TF2Vulkan/ShaderDeviceMgr.cpp
        src/TF2Vulkan/ShaderNextFactory.cpp
        src/TF2Vulkan/shaders/VulkanShaderManager.cpp
        src/TF2Vulkan/StateManagerVulkan.cpp
        src/TF2Vulkan/VBAllocTracker.cpp
        src/TF2Vulkan/ResourceBlob.cpp
        src/TF2Vulkan/VulkanFactories.cpp
        src/TF2Vulkan/StateManagerStatic.cpp
        src/TF2Vulkan/IShaderAPI/IShaderAPI_StateManagerDynamic.cpp
        src/TF2Vulkan/VertexFormat.cpp
        src/TF2Vulkan/vk_mem_alloc.cpp
        src/TF2Vulkan/meshes/VulkanMesh.cpp
        src/TF2Vulkan/VulkanUtil.cpp)

target_include_directories(shaderapivulkan PUBLIC include
        ./include/
        ../TF2VulkanUtil/include/
        ../stdshader_vulkan/include/
        ../sdk2013/mp/src/public/)
target_link_libraries(shaderapivulkan PRIVATE Vulkan::Vulkan)