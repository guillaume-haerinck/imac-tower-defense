# Requirement
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

# ---------------------------------------- PROJECT FILES
project(imac-tower-defense)
file(GLOB_RECURSE MY_SOURCES src/*)

# ---------------------------------------- FIND DEPENDENCIES
find_package(OpenGL REQUIRED)
set(
    NOESIS_INCLUDE_DIRS 
    ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Src/Packages/App/Display/Include;
    ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Src/Packages/App/Providers/Include;
    ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Src/Packages/Render/GLRenderDevice/Include;
    ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Include;
)

if (WIN32)
    set(SDL2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/include)
    set(SDL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/win_x86/SDL2.lib;${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/win_x86/SDL2main.lib)
    set(NOESIS_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Lib/windows_x86/Noesis.lib;${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Lib/windows_x86/NoesisApp.lib)
else()
    find_package(SDL2 REQUIRED)
    set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Bin/linux_x86_64)
    find_library(NOESIS_LIB Noesis)
    find_library(NOESIS_APP_LIB NoesisApp)
    set(NOESIS_LIBRARIES ${NOESIS_LIB} ${NOESIS_APP_LIB})
endif()

if (SDL2_LIBRARIES) 
    message("Found SDL2: " ${SDL2_LIBRARIES})
endif()
if (NOESIS_LIBRARIES) 
    message("Found Noesis: " ${NOESIS_LIBRARIES})
endif()

# ----------------------------------------- BUILD DEPENDENCIES IF NEEDED
file(GLOB_RECURSE IMGUI_SOURCES lib/imgui/*)
add_library(IMGUI STATIC ${IMGUI_SOURCES})
add_library(STB_IMAGE STATIC "lib/stb_image/stb_image.cpp")
add_library(GLAD STATIC "lib/glad/src/glad.c")

# ----------------------------------------- INCLUDE DIRECTORIES
include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${NOESIS_INCLUDE_DIRS}
    lib/glad/include
    lib/entt-cpp14/src
    lib
    src
)

# ------------------------------------------- BUILD PROJECT
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
add_executable(${PROJECT_NAME} ${MY_SOURCES})
target_link_libraries(
    ${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${NOESIS_LIBRARIES}
    IMGUI
    STB_IMAGE
    GLAD
)

if (UNIX) # Use dynamic linking library
    target_link_libraries(${PROJECT_NAME} -ldl)
endif()

if (WIN32) # Copy .dll to build folder
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/SDL2-2.0.9/lib/win_x86/SDL2.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Bin/windows_x86"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

