#include "shader.hpp"
#include "debug/gl-error-handling.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

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
		std::cerr << "[Error] parseShader: File " << filepath << " don't exist !" << std::endl;
		DEBUG_BREAK;
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
		std::cerr << "[Error] Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader") << std::endl;
		std::cout << message << std::endl;
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

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
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
		std::cout << "Warning: uniform '" << name << "' doesn't exist !" << std::endl;
	}
	m_UniformLocationCache[name] = location;
	return location;
}
