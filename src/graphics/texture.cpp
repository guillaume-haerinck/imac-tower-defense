#include "texture.hpp"

#include <iostream>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <stb_image/stb_image.h>

Texture::Texture(const std::string& path)
	: m_rendererID(0), m_filePath(path), m_localBuffer(nullptr),
	m_width(0), m_height(0), m_bpp(0)
{
	// Because 0,0 is bottom left in OpenGL
	stbi_set_flip_vertically_on_load(true);
	m_localBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_bpp, 4);
	if (!m_localBuffer) {
		spdlog::critical("[Texture] Unable to open texture {}", path);
		debug_break();
	}
	stbi_set_flip_vertically_on_load(false);

	GLCall(glGenTextures(1, &m_rendererID));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer));
	unbind();

	if (m_localBuffer) {
		stbi_image_free(m_localBuffer);
	}
}

Texture::~Texture() {
	// GLCall(glDeleteTextures(1, &m_rendererID));  // Deleted in sprite factory
}

void Texture::bind(unsigned int slot) const {
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererID));
}

void Texture::unbind() const {
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
