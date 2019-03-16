# Requirement
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)

# Project configuration
project(imac-tower-defense)
file(
    GLOB_RECURSE
    MY_SOURCES
    lib/imgui/*
    src/*
)

# Find builded libs
find_package(OpenGL REQUIRED)
if (NOT WIN32)
    find_package(SDL2 REQUIRED)
    set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Bin/linux_x86_64)
    find_library(NOESIS_LIB Noesis)
    find_library(NOESIS_APP_LIB NoesisApp)
else()
    set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/lib/SDL2-2.0.9/include)
    set(SDL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2.lib;${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2main.lib)
    set(NOESIS_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Lib/windows_x86_64/Noesis.lib)
endif()

# Build non-builded libs
add_library(GLAD STATIC "lib/glad/src/glad.c")
add_library(STB_IMAGE STATIC "lib/stb_image/stb_image.cpp")

# Search directories for .h files
include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    lib/NoesisGUI-2.2.0/Include
    lib/glad/include
    lib/entt/src
    lib
    src
)

# Link libraries and Build project
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
add_executable(${PROJECT_NAME} ${MY_SOURCES})
if (NOT WIN32)
    target_link_libraries(
        ${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${NOESIS_LIB}
        ${NOESIS_APP_LIB}
        STB_IMAGE
        GLAD
        -ldl
    )
else()
    target_link_libraries(
        ${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${NOESIS_LIBRARIES}
        STB_IMAGE
        GLAD
    )
    # Copy .dll to build folder
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Bin/windows_x86_64"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()
