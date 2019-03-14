#include "gl-error-handling.hpp"
#include "rang/rang.hpp"

void glError_clear() {
	while (glGetError() != GL_NO_ERROR);
}

bool glError_doesFunctionWorks(const char* functionName, const char* filename, int line) {
    GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << rang::bg::red << "[OpenGL Error] " << gluErrorString(error) << ": " << functionName << " " << filename << " : " << line << rang::bg::reset << std::endl;
		return false;
	}
	return true;
}

void GLAPIENTRY glError_messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userParam) {
	std::cout << "[";
	switch (source) {
		case GL_DEBUG_SOURCE_API: std::cout << "Api"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cout << "Window system"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Shader compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: std::cout << "Third party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: std::cout << "Application"; break;
		case GL_DEBUG_SOURCE_OTHER: std::cout << "Other"; break;
	}
	std::cout << " ";

	switch (type) {
		case GL_DEBUG_TYPE_ERROR: std::cout << "error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "depreciated behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "undefined behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY: std::cout << "portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "performance"; break;
		case GL_DEBUG_TYPE_MARKER: std::cout << "marker"; break;
		case GL_DEBUG_TYPE_OTHER: std::cout << "other"; break;
	}
	std::cout << "]";

	switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << rang::fg::green; break;
		case GL_DEBUG_SEVERITY_LOW: std::cout << rang::fg::cyan; break;
		case GL_DEBUG_SEVERITY_MEDIUM: std::cout << rang::fg::magenta; break;
		case GL_DEBUG_SEVERITY_HIGH: std::cout << rang::fg::red; break;
	}
	std::cout << " " << id << " : " << message << rang::fg::reset << std::endl;
}
