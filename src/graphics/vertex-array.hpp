#pragma once

#include "vertex-buffer.hpp"
#include "vertex-buffer-layout.hpp"

class VertexArray {
private:
	unsigned int m_rendererID;

public:
	VertexArray();
	~VertexArray();

	void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void bind() const;
	void unbind() const;
	void remove() const;

	unsigned int getID() const { return m_rendererID; }
};
