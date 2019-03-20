#include "texture-array.hpp"

#include <iostream>
#include <spdlog/spdlog.h>
#include "stb_image/stb_image.h"

TextureArray::TextureArray(const std::string& path, GLsizei tileWidth, GLsizei tileHeight)
	: m_rendererID(0), m_filePath(path), m_localBuffer(nullptr),
	m_width(0), m_height(0), m_bpp(0)
{
	// Because the UV map is inverted for texture arrays
	stbi_set_flip_vertically_on_load(0);
	m_localBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_bpp, STBI_rgb_alpha);
	if (!m_localBuffer) {
		spdlog::critical("[Texture array] Unable to open texture atlas {}", path);
		debug_break();
	}

	GLsizei tilesX = m_width / tileWidth;
	GLsizei tilesY = m_height / tileHeight;
	GLsizei tileCount = tilesX * tilesY;
	
	GLCall(glGenTextures(1, &m_rendererID));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_rendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tileWidth, tileHeight, tileCount));
	
	{
		GLuint tempTexture = 0;
		GLCall(glGenTextures(1, &tempTexture));
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, tempTexture));
		GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height));
		GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer));

		for (GLsizei i = 0; i < tileCount; ++i) {
			GLint x = (i % tilesX) * tileWidth, y = (i / tilesX) * tileHeight;
			GLCall(glCopyImageSubData(tempTexture, GL_TEXTURE_2D, 0, x, y, 0, m_rendererID, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tileWidth, tileHeight, 1));
		}
		GLCall(glDeleteTextures(1, &tempTexture));
	}

	unbind();

	if (m_localBuffer) {
		stbi_image_free(m_localBuffer);
	}
}

TextureArray::~TextureArray() {
	// GLCall(glDeleteTextures(1, &m_rendererID)); // Deleted in sprite factory
}

void TextureArray::bind(unsigned int slot) const {
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_rendererID));
}

void TextureArray::unbind() const {
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
}
