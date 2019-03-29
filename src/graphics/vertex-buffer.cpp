#include "vertex-buffer.hpp"
#include "logger/gl-log-handler.hpp"

VertexBuffer::VertexBuffer(const void* data, unsigned int size, GLenum usage) {
	GLCall(glGenBuffers(1, &m_rendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
}

VertexBuffer::~VertexBuffer() {
	// It is ok to delete a vertex buffer after binding it to vertex array
	// The buffer will not be deleted by opengl until it is no longer in use
	GLCall(glDeleteBuffers(1, &m_rendererID));
}

void VertexBuffer::bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
}

void VertexBuffer::unbind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
