# CMakeList.txt : CMake project for enerMesh, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10.2)

project(VulkanQuickStart)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}")

set (SOURCE_FILES 
    "src/VulkanQuickStart.cxx" 
)

if (UNIX)
    
    find_package(wxWidgets COMPONENTS net gl core base)
    include_directories(
        "../../VulkanQuickStartLib/include"
    )
    
    execute_process(COMMAND wx-config --cxxflags OUTPUT_VARIABLE WX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND wx-config --libs OUTPUT_VARIABLE WX_LIB_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

    list(APPEND CMAKE_CXX_FLAGS "${WX_FLAGS} -fPIC ${WX_LIB_FLAGS}")
    set(GLSL_VALIDATOR "glslangValidator")
    add_definitions(-DGLFW_EXPOSE_NATIVE_X11 -D_GCC)
    set(GLFW_LIB_DIR "../../../ThirdParty/glfw/src")
    set(WX_LIB_NAMES "wx_gtk3u-3.2.so")

    link_directories (
        "../../VulkanQuickStartLib/${CMAKE_BUILD_TYPE}"
        "../../triMesh/${CMAKE_BUILD_TYPE}"
        ${wxWidgets_ROOT_DIR}/lib/vc_x64_lib/
    )

    add_executable (VulkanQuickStart
        ${SOURCE_FILES}
    )

    target_link_libraries(VulkanQuickStart 
	PUBLIC VulkanQuickStartLib
        stlReader
        ${WX_LIB_NAMES}
    )

    target_compile_features(VulkanQuickStart PRIVATE cxx_std_17)
else(UNIX)
    find_package(wxWidgets)
    include(${wxWidgets_USE_FILE})

    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(WX_INCLUDE_CONFIG_PATH "${wxWidgets_DIR}/vc_x64_lib/mswu/")
        set(WX_LIB_NAMES "comctl32.lib Rpcrt4.lib wxmsw32u.lib wxpng.lib wxregexu.lib wxzlib.lib")
    else()
        set(WX_INCLUDE_CONFIG_PATH "${wxWidgets_DIR}/vc_x64_lib/mswud/")
        set(WX_LIB_NAMES "comctl32.lib Rpcrt4.lib wxmsw32ud.lib wxpngd.lib wxregexud.lib wxzlibd.lib")
    endif()

    include_directories(
        "../../ThirdParty/eigen"
        "../../triMesh/triMesh/include"
        "../../ThirdParty/glfw/include"
        "C:/VulkanSDK/1.3.250.1/Include/"
	    "../../VulkanQuickStartLib/include/"
        "${wxWidgets_INCLUDE_DIRS}"
    )

    set(GLSL_VALIDATOR "C:/VulkanSDK/1.3.250.1/Bin/glslangValidator.exe")
    add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
    set(GLFW_LIB_DIR "../../../ThirdParty/glfw-3.3.2/lib-vc2019")

    add_executable (VulkanQuickStart
        WIN32
        ${SOURCE_FILES}
    )
    target_link_directories(VulkanQuickStart 
        PRIVATE ${wxWidgets_ROOT_DIR}/lib/vc_x64_lib/
        PRIVATE "../../triMesh/${CMAKE_BUILD_TYPE}/"
        PRIVATE "../../VulkanQuickStartLib/${CMAKE_BUILD_TYPE}/"
    )

    target_link_libraries(VulkanQuickStart PRIVATE 
        triMesh 
        stlReader
	    VulkanQuickStartLib
        ${WX_LIB_NAMES}
    )

    target_compile_features(VulkanQuickStart PRIVATE cxx_std_17)

    set(SOURCE_LIB "${PROJECT_SOURCE_DIR}/../../VulkanQuickStartLib/${CMAKE_BUILD_TYPE}/VulkanQuickStartLib")
    set(DEST_LIB "${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/VulkanQuickStartLib")
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        add_custom_command(TARGET VulkanQuickStart POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_LIB}.dll" "${DEST_LIB}.dll"
            COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_LIB}.lib" "${DEST_LIB}.lib"
            COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_LIB}.exp" "${DEST_LIB}.exp"
        )
    else() 
        add_custom_command(TARGET VulkanQuickStart POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_LIB}.dll" "${DEST_LIB}.dll"
            COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_LIB}.lib" "${DEST_LIB}.lib"
            COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_LIB}.exp" "${DEST_LIB}.exp"
            COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_LIB}.pdb" "${DEST_LIB}.pdb"
        )
    endif()
endif(UNIX)


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
