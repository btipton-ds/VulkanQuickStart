# CMakeList.txt : CMake project for enerMesh, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10.2)

project(VulkanQuickStart)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}")

find_package(wxWidgets)
include(${wxWidgets_USE_FILE})

message(STATUS "wxWidgets_FOUND: ${wxWidgets_FOUND}")
message(STATUS "wxWidgets_VERSION: ${wxWidgets_VERSION}")
message(STATUS "wxWidgets_VERSION_MAJOR: ${wxWidgets_VERSION_MAJOR}")
message(STATUS "wxWidgets_ROOT_DIR: ${wxWidgets_ROOT_DIR}")
message(STATUS "wxWidgets_DIR: ${wxWidgets_DIR}")
message(STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
message(STATUS "wxWidgets_INCLUDE_DIRS: ${wxWidgets_INCLUDE_DIRS}")
message(STATUS "WX_INCLUDE_CONFIG_PATH: ${WX_INCLUDE_CONFIG_PATH}")
message(STATUS "wxWidgets_LIBRARY_DIRS: ${wxWidgets_LIBRARY_DIRS}")
message(STATUS "wxWidgets_LIBRARIES: ${wxWidgets_LIBRARIES}")

set(wxWidgets_INCLUDE_DIR "${wxWidgets_DIR}/../include/")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(DLL_NAME "wxmsw32u_core_vc14x_x64.dll")
    set(WX_INCLUDE_CONFIG_PATH "${wxWidgets_DIR}/vc_x64_dll/mswu/")
else()
    set(DLL_NAME "wxmsw32ud_core_vc14x_x64.dll")
    set(WX_INCLUDE_CONFIG_PATH "${wxWidgets_DIR}/vc_x64_dll/mswud/")
endif()

if (UNIX)
    
    include_directories(
	    "../../VulkanQuickStartLib/include"
    )
    
	execute_process(COMMAND wx-config --cxxflags OUTPUT_VARIABLE WX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND wx-config --libs OUTPUT_VARIABLE WX_LIB_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    
	list(APPEND CMAKE_CXX_FLAGS "${WX_FLAGS} -DWXUSINGDLL -fPIC ${WX_LIB_FLAGS}")
    set(GLSL_VALIDATOR "glslangValidator")
    add_definitions(-DGLFW_EXPOSE_NATIVE_X11 -D_GCC)
    set(GLFW_LIB_DIR "../../../ThirdParty/glfw/src")
    set(WX_LIB_DIR "")
    set(WX_LIB_NAME "wx_gtk3u-3.2")
else(UNIX)
    include_directories(
	    "../../VulkanQuickStartLib/include/"
        "${wxWidgets_INCLUDE_DIRS}"
    )


    set(GLSL_VALIDATOR "C:/VulkanSDK/1.3.250.1/Bin/glslangValidator.exe")
    add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
    set(GLFW_LIB_DIR "../../../ThirdParty/glfw-3.3.2/lib-vc2019")
endif(UNIX)

set(CONFIG "x64-${CMAKE_BUILD_TYPE}")

link_directories (
    "../../VulkanQuickStartLib/${CMAKE_BUILD_TYPE}"
    "../../triMesh/${CMAKE_BUILD_TYPE}"
)


add_executable (VulkanQuickStart
    WIN32
	"src/VulkanQuickStart.cxx" 
)

target_compile_definitions(VulkanQuickStart PRIVATE WIN32)

target_link_libraries(VulkanQuickStart 
	PUBLIC VulkanQuickStartLib
	stlReader
    ${wxWidgets_LIBRARIES}
)

target_compile_features(VulkanQuickStart PRIVATE cxx_std_17)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "../shaders/*.frag"
    "../shaders/*.vert"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(RAW_FILE_NAME ${GLSL} NAME)
  string(REPLACE "." "_" FILE_NAME "${RAW_FILE_NAME}")
  set(SPIRV "${PROJECT_SOURCE_DIR}/../shaders_spv/${FILE_NAME}.spv")

  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/../shaders_spv/"
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
