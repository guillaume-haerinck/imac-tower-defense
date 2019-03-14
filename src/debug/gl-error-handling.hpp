#pragma once

#define GL_DEBUG
#define ASSERTIONS_ENABLED

#include <glad/glad.h>
#include <GL/glu.h>
#include <csignal>
#include <iostream>

#ifdef _MSC_VER
    #define DEBUG_BREAK __debugbreak()
#else
    #define DEBUG_BREAK raise(SIGTRAP)
#endif

/**
 * @brief Assertion. Stop the execution if the value is false.
 */
#ifdef ASSERTIONS_ENABLED
    #define ASSERT(x) \
    if (x) {} \
    else { DEBUG_BREAK; }
#else
    #define ASSERT(x) x
#endif

/**
 * @brief Assertion and debug handling for opengl functions
 */
#ifdef GL_DEBUG
    #define GLCall(x) glError_clear(); x; ASSERT(glError_doesFunctionWorks(#x, __FILE__, __LINE__))
#else
    #define GLCall(x) x
#endif

/**
 * @brief Empty the OpenGl error buffer
 */
void glError_clear();

/**
 * @brief Print OpenGl errors to the console if any
 * 
 * @param functionName
 * @param filename
 * @param line
 * @return true
 * @return false
 */
bool glError_doesFunctionWorks(const char* functionName, const char* filename, int line);

/**
 * @brief Detailed log of open gl
 * @note You must have glEnable(GL_DEBUG_OUTPUT) and glDebugMessageCallback(debugCallback, nullptr) to get it working.
 * 
 * @param source 
 * @param type 
 * @param id 
 * @param severity 
 * @param length 
 * @param message 
 * @param user_param 
 */
void GLAPIENTRY glError_messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userParam);
