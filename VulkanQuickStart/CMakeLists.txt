# CMakeList.txt : CMake project for enerMesh, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10.2)

include_directories(
"include"
"vk_base"
"../../ThirdParty/glm-0.9.9.5/glm"
"../../ThirdParty/glfw-3.3.2/include"
"../../ThirdParty/tinyobjloader"
"../../ThirdParty/freetype-2.10.2/include"
"../../ThirdParty/stb"
"/VulkanSDK/1.2.135.0/Include"
"../../triMesh/triMesh/include"
"../../triMesh/stlReader/include"
)

find_package (Vulkan)

string(FIND ${PROJECT_BINARY_DIR} "Release" isRelease)
string(FIND ${PROJECT_BINARY_DIR} "Debug" isDebug)

if (UNIX)
	list(APPEND CMAKE_CXX_FLAGS "-pthread -fPIC")
    set(GLSL_VALIDATOR "glslangValidator")
    add_definitions(-DGLFW_EXPOSE_NATIVE_X11 -D_GCC)
    set(GLFW_LIB_DIR "../../ThirdParty/glfw/src")

    if(NOT (isRelease EQUAL -1))
        set (CONFIG "WSL-GCC-Release")
    elseif(NOT (isDebug EQUAL -1))
        set (CONFIG "WSL-GCC-Debug")
    else()
        message(FATAL_ERROR "unknown PROJECT_BINARY_DIR")
    endif()
else(UNIX)

    set(GLSL_VALIDATOR "/VulkanSDK/1.2.135.0/Bin32/glslangValidator.exe")
    add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
    set(GLFW_LIB_DIR "../../ThirdParty/glfw-3.3.2/lib-vc2019")

    if(NOT (isRelease EQUAL -1))
        set (CONFIG "x64-Release")
        set(FREE_TYPE_LIB_DER "../../ThirdParty/freetype-2.10.2/x64/Release Static")
    elseif(NOT (isDebug EQUAL -1))
        set (CONFIG "x64-Debug")
        set(FREE_TYPE_LIB_DER "../../ThirdParty/freetype-2.10.2/x64/Debug Static")
    else()
        message(FATAL_ERROR "unknown PROJECT_BINARY_DIR")
    endif()
endif(UNIX)

link_directories (
"${GLFW_LIB_DIR}"
"${FREE_TYPE_LIB_DER}"
"../../triMesh/out/build/${CONFIG}/triMesh/"
"../../triMesh/out/build/${CONFIG}/stlReader/"
)

find_package(Vulkan)
#find_package(glfw3 REQUIRED)

add_library (
VulkanQuickStartLib
"vk_base/VulkanTools.cpp"
"src/vk_app.cpp"
"src/vk_buffer.cpp"
"src/vk_computeStep.cpp" 
"src/vk_deviceContext.cpp"
"src/vk_image.cpp"
"src/vk_imageCopier.cpp" 
"src/vk_modelPNC3f.cpp"  
"src/vk_modelPNCT3f.cpp"
"src/vk_offscreenPassBase.cpp"  
"src/vk_pipelineBase.cpp"

"src/vk_pipelinePNC3f.cpp"
"src/vk_pipelineUi.cpp"
"src/vk_pipelinePNCT3f.cpp"
"src/vk_pipelineUboGroup.cpp" 
"src/vk_sceneNodeBase.cpp"
"src/vk_sceneNodePNC3f.cpp"
"src/vk_sceneNodePNCT3f.cpp"
"src/vk_sceneNodeUi.cpp"
"src/vk_shaderPool.cpp"
"src/vk_textureImage.cpp"
"src/vk_vertexTypes.cpp" 
"src/vk_ui_button.cpp" 
"src/vk_ui_window.cpp"
 "src/vk_sceneNodeImageProc.cpp")

target_compile_features(VulkanQuickStartLib PRIVATE cxx_std_17)

add_executable (
VulkanQuickStart
"src/VulkanQuickStart.cxx" 
)

target_link_libraries("VulkanQuickStart" 
PUBLIC VulkanQuickStartLib
PUBLIC Vulkan::Vulkan
PUBLIC triMesh
PUBLIC stlReader 
PUBLIC freetype
PUBLIC glfw3
)

target_compile_features(VulkanQuickStart PRIVATE cxx_std_17)


file(GLOB_RECURSE GLSL_SOURCE_FILES
    "shaders/*.frag"
    "shaders/*.vert"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(RAW_FILE_NAME ${GLSL} NAME)
  string(REPLACE "." "_" FILE_NAME "${RAW_FILE_NAME}")
  set(SPIRV "${PROJECT_BINARY_DIR}/VulkanQuickStart/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/VulkanQuickStart/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    VkShaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(VulkanQuickStart VkShaders)

# TODO: Add tests and install targets if needed.
