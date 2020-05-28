# CMakeList.txt : CMake project for enerMesh, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10.2)

include_directories(
include 
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

    if(NOT (isRelease EQUAL -1))
        set (CONFIG "WSL-GCC-Release")
    elseif(NOT (isDebug EQUAL -1))
        set (CONFIG "WSL-GCC-Debug")
    else()
        message(FATAL_ERROR "unknown PROJECT_BINARY_DIR")
    endif()
else(UNIX)
    set(GLSL_VALIDATOR "/VulkanSDK/1.2.135.0/Bin32/glslangValidator.exe")
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
"../../ThirdParty/glfw-3.3.2/lib-vc2019"
"${FREE_TYPE_LIB_DER}"
"../../triMesh/out/build/${CONFIG}/triMesh/"
"../../triMesh/out/build/${CONFIG}/stlReader/"
)

find_package(Vulkan)
find_package(glfw3 REQUIRED)

add_executable (
VulkanQuickStart
"src/vk_app.cpp"
"src/vk_buffer.cpp"
"src/vk_deviceContext.cpp"
"src/vk_image.cpp"
"src/vk_model.cpp"  
"src/vk_modelObj.cpp"
"src/vk_pipeline.cpp"
"src/vk_pipeline3D.cpp"
"src/vk_pipelineUi.cpp"
"src/vk_scene.cpp"
"src/vk_sceneNode.cpp"
"src/vk_sceneNodeGroup.cpp"
"src/vk_sceneNode3d.cpp"
"src/vk_shaderPool.cpp"
"src/vk_textureImage.cpp"
"src/vk_ui_button.cpp" 
"src/vk_ui_window.cpp"
"src/VulkanQuickStart.cxx"
 "src/vk_sceneNode3DWTexture.cpp" "src/vk_pipeline3DWithTexture.cpp")

target_link_libraries("VulkanQuickStart" 
PUBLIC freetype
PUBLIC glfw3
PUBLIC triMesh
PUBLIC stlReader 
Vulkan::Vulkan
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
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(VulkanQuickStart Shaders)

# TODO: Add tests and install targets if needed.
