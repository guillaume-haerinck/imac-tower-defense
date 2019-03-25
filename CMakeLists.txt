# Requirement
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

# ---------------------------------------- PROJECT FILES
project(imac-tower-defense)
file(GLOB_RECURSE MY_SOURCES src/*)

# ---------------------------------------- FIND DEPENDENCIES
# Common on all platform
find_package(OpenGL REQUIRED)
set(
    NOESIS_INCLUDE_DIRS 
    ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Src/Packages/App/Display/Include;
    ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Src/Packages/App/Providers/Include;
    ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Src/Packages/Render/GLRenderDevice/Include;
    ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Include;
)
set(FMOD_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/lib/fmod-2.0.0/api/core/inc)
set(BOX2D_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/lib/box2d-2.3.0/include)

if (WIN32) # On windows
    set(SDL2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/include)
    set(SDL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/windows_x86/SDL2.lib;${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/windows_x86/SDL2main.lib)
    set(NOESIS_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Lib/windows_x86/Noesis.lib;${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Lib/windows_x86/NoesisApp.lib)
    set(FMOD_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/fmod-2.0.0/api/core/lib/windows_x86/fmod_vc.lib)
    set(BOX2D_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/box2d-2.3.0/lib/windows_x86/Debug/Box2D.lib)
else() # On linux
    find_package(SDL2 REQUIRED)
    set(
        CMAKE_PREFIX_PATH 
        ${CMAKE_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Bin/linux_x86_64;
        ${CMAKE_SOURCE_DIR}/lib/fmod-2.0.0/api/core/lib/linux_x86_64;
        ${CMAKE_SOURCE_DIR}/lib/box2d-2.3.0/lib/linux_x86_64/Debug
    )
    find_library(NOESIS_LIB Noesis)
    find_library(NOESIS_APP_LIB NoesisApp)
    set(NOESIS_LIBRARIES ${NOESIS_LIB} ${NOESIS_APP_LIB})
    find_library(FMOD_LIBRARIES fmod)
    find_library(BOX2D_LIBRARIES Box2D)
endif()

if (SDL2_LIBRARIES)
    message("Found SDL2: " ${SDL2_LIBRARIES})
endif()
if (NOESIS_LIBRARIES)
    message("Found Noesis: " ${NOESIS_LIBRARIES})
endif()
if (FMOD_LIBRARIES)
    message("Found FMOD: " ${FMOD_LIBRARIES})
endif()
if (BOX2D_LIBRARIES)
    message("Found BOX2D: " ${FMOD_LIBRARIES})
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
    ${FMOD_INCLUDE_DIRS}
    ${BOX2D_INCLUDE_DIRS}
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
    ${FMOD_LIBRARIES}
    ${BOX2D_LIBRARIES}
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
        "${PROJECT_SOURCE_DIR}/lib/SDL2-2.0.9/lib/windows_x86/SDL2.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/lib/NoesisGUI-2.2.0/Bin/windows_x86"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/fmod-2.0.0/api/core/lib/windows_x86/fmod.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

