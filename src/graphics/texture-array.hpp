#pragma once

#include "logger/gl-error-handler.hpp"
#include <string>

class TextureArray {
private:
	unsigned int m_rendererID;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_bpp;

public:
	TextureArray(const std::string& path, GLsizei tileWidth, GLsizei tileHeight);
	~TextureArray();
 
	void bind(unsigned int slot = 0) const;
	void unbind() const;

	unsigned int getID() const { return m_rendererID; }
	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }
};
