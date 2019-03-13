#pragma once

#include <glad/glad.h>

class VertexBuffer {
private:
	unsigned int m_rendererID;

public:
	VertexBuffer(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
	~VertexBuffer();

	void bind() const;
	void unbind() const;
	unsigned int getID() const { return m_rendererID; }
};
