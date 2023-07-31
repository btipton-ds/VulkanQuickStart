# CMakeList.txt : CMake project for enerMesh, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10.2)

project(VulkanQuickStart)

include_directories(
"../VulkanQuickStartLib/include"
)

if (UNIX)
	list(APPEND CMAKE_CXX_FLAGS "-pthread -fPIC")
    set(GLSL_VALIDATOR "glslangValidator")
    add_definitions(-DGLFW_EXPOSE_NATIVE_X11 -D_GCC)
    set(GLFW_LIB_DIR "../../ThirdParty/glfw/src")
else(UNIX)

    set(GLSL_VALIDATOR "/VulkanSDK/1.2.170.0/Bin32/glslangValidator.exe")
    add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
    set(GLFW_LIB_DIR "../../ThirdParty/glfw-3.3.2/lib-vc2019")
endif(UNIX)

set(CONFIG "x64-${CMAKE_BUILD_TYPE}")

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BUILD_TYPE})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BUILD_TYPE})

link_directories (
"../VulkanQuickStartLib/${CMAKE_BUILD_TYPE}"
)


add_executable (
VulkanQuickStart
"src/VulkanQuickStart.cxx" 
)

target_link_libraries("VulkanQuickStart" 
PUBLIC VulkanQuickStartLib
)

target_compile_features(VulkanQuickStart PRIVATE cxx_std_17)


file(GLOB_RECURSE GLSL_SOURCE_FILES
    "../VulkanQuickStartLib/shaders/*.frag"
    "../VulkanQuickStartLib/shaders/*.vert"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(RAW_FILE_NAME ${GLSL} NAME)
  string(REPLACE "." "_" FILE_NAME "${RAW_FILE_NAME}")
  set(SPIRV "${PROJECT_BINARY_DIR}/VulkanQuickStartExe/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/VulkanQuickStartExe/shaders/"
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
