#include "gl-log-handler.hpp"
#include "spdlog/spdlog.h"

void gllog::clear() {
	while (glGetError() != GL_NO_ERROR);
}

bool gllog::doesFunctionWorks(const char* functionName, const char* filename, int line) {
    GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
        spdlog::error("[OpenGL Error]) {}: {} {} {}", gluErrorString(error), functionName, filename, line);
		return false;
	}
	return true;
}

void GLAPIENTRY gllog::messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param) {
	auto const sourceString = [source]() {
		switch (source) {
			case GL_DEBUG_SOURCE_API: return "Api";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window system";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader compiler";
			case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third party";
			case GL_DEBUG_SOURCE_APPLICATION: return "Application";
			case GL_DEBUG_SOURCE_OTHER: return "Other";
			default: return "";
		}
	}();

	auto const typeString = [type]() {
		switch (type) {
			case GL_DEBUG_TYPE_ERROR: return "error";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "deprecated behavior";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "undefined behavior";
			case GL_DEBUG_TYPE_PORTABILITY: return "portability";
			case GL_DEBUG_TYPE_PERFORMANCE: return "performance";
			case GL_DEBUG_TYPE_MARKER: return "marker";
			case GL_DEBUG_TYPE_OTHER: return "other";
			default: return "";
		}
	}();

	switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			spdlog::info("[{} {}] {} : {}", sourceString, typeString, id, message);
			break;

		case GL_DEBUG_SEVERITY_LOW:
			spdlog::warn("[{} {}] {} : {}", sourceString, typeString, id, message);
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			spdlog::error("[{} {}] {} : {}", sourceString, typeString, id, message);
			break;

		case GL_DEBUG_SEVERITY_HIGH:
			spdlog::critical("[{} {}] {} : {}", sourceString, typeString, id, message);
			break;
	}
}
