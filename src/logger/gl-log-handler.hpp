#pragma once

#ifndef NDEBUG
    #define GL_DEBUG
#endif

#include <glad/glad.h>
#include <GL/glu.h>
#include <assert.h>
#include <debugbreak/debugbreak.h>
#include <csignal>
#include <iostream>

/**
 * @brief Assertion and logger handling for opengl functions
 */
#ifdef GL_DEBUG
    #define GLCall(x) gllog::clear(); x; assert(gllog::doesFunctionWorks(#x, __FILE__, __LINE__))
#else
    #define GLCall(x) x
#endif

namespace gllog {
/**
 * @brief Empty the OpenGl error buffer
 */
void clear();

/**
 * @brief Print OpenGl errors to the console if any
 * 
 * @param functionName
 * @param filename
 * @param line
 * @return true
 * @return false
 */
bool doesFunctionWorks(const char* functionName, const char* filename, int line);

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
void GLAPIENTRY messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userParam);

}