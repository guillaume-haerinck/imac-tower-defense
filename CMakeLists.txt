# Requirement
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)

# Project configuration
project(imac-tower-defense)
file(
    GLOB_RECURSE
    MY_SOURCES
    src/*
)

# Translation unit
find_package(OpenGL REQUIRED)
if (NOT WIN32)
    find_package(SDL2 REQUIRED)
else()
    set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/lib/SDL2-2.0.9/include)
    set(SDL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x86/SDL2.lib;${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x86/SDL2main.lib)
endif()
add_library(GLAD STATIC "lib/glad/src/glad.c")
add_library(STB_IMAGE STATIC "lib/stb_image/stb_image.cpp")
include_directories( # Search directories for .h files
    ${OPENGL_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    lib/glad/include
    lib/entt/src
    lib
    src
)

# Build path and Linker operations
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
add_executable(${PROJECT_NAME} ${MY_SOURCES})
if (NOT WIN32) # Add static library .lib files
    target_link_libraries(
        ${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        ${SDL2_LIBRARIES}
        STB_IMAGE
        GLAD -ldl
    )
else()
    target_link_libraries(
        ${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        ${SDL2_LIBRARIES}
        STB_IMAGE
        GLAD
    ) # Copy dynamic library .dll files to build folder
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x86/SDL2.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()
