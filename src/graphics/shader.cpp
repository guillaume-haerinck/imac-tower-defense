#include "shader.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <spdlog/spdlog.h>

#include "logger/gl-log-handler.hpp"

Shader::Shader(const std::string& filepathVertex, const std::string& filepathFragment)
	: m_FilePathVertex(filepathVertex), m_FilePathFragment(filepathFragment), m_rendererID(0)
{
	std::string vertexShader = parseShader(filepathVertex);
	std::string fragmentShader = parseShader(filepathFragment);
	m_rendererID = createShader(vertexShader, fragmentShader);
}

Shader::~Shader() {
	GLCall(glDeleteProgram(m_rendererID));
}

std::string Shader::parseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	if (!stream) {
		spdlog::critical("[Shader] parseShader(): File {} don't exist !", filepath);
		debug_break();
		return "";
	}

	std::string shader = "";
	std::string tempLine = "";
	while (getline(stream, tempLine)) {
		shader += tempLine + '\n';
	}
	stream.close();
	return shader.c_str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	// Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char *)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		auto const typeString = [type]() {
			switch (type) {
				case GL_VERTEX_SHADER: return "fragment";
				case GL_FRAGMENT_SHADER: return "vertex";
				case GL_TESS_EVALUATION_SHADER: return "tesselation evaluation";
				case GL_TESS_CONTROL_SHADER: return "tesselation control";
				case GL_GEOMETRY_SHADER: return "geometry";
				case GL_COMPUTE_SHADER: return "compute";
				default: return "unknown type";
			}
		}();

		spdlog::error("[Shader] Failed to compile {} shader", typeString);
		spdlog::error("[Shader] {}", message);
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

void Shader::bind() const {
	GLCall(glUseProgram(m_rendererID));
}

void Shader::unbind() const {
	GLCall(glUseProgram(0));
}

void Shader::setUniform1i(const std::string& name, int value) {
	GLCall(glUniform1i(getUniformLocation(name), value));
}

void Shader::setUniform1f(const std::string& name, float v0) {
	GLCall(glUniform1f(getUniformLocation(name), v0));
}

void Shader::setUniform2f(const std::string& name, float v0, float v1) {
	GLCall(glUniform2f(getUniformLocation(name), v0, v1));
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2) {
	GLCall(glUniform3f(getUniformLocation(name), v0, v1, v2));
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniform4f(const std::string& name, glm::vec4 v) {
	GLCall(glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w));
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4 matrix) {
	GLCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::getUniformLocation(const std::string& name) {
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}

	GLCall(int location = glGetUniformLocation(m_rendererID, name.c_str()));
	if (location == -1) {
		spdlog::warn("[Shader] uniform '{}' doesn't exist !", name);
	}
	m_UniformLocationCache[name] = location;
	return location;
}
