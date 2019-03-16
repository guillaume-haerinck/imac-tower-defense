#pragma once

#include <vector>
#include <assert.h>
#include <iostream>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "logger/gl-log-handler.hpp"

struct VertexBufferElement {
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int getSizeOfType(unsigned int type) {
		switch (type) {
			case GL_FLOAT:			return 4;
			case GL_UNSIGNED_INT:	return 4;
			case GL_UNSIGNED_BYTE:	return 1;
		}
		spdlog::error("[Vertex buffer layout] Unknown size of type : getSizeOfType() : vertex-buffer-layout.hpp");
		debug_break();
		return 0;
	}
};

class VertexBufferLayout {
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public:
	VertexBufferLayout()
		: m_Stride(0) {}

	template<typename T>
	void push(unsigned int count) {
		// static_assert(false);
	}

	inline const std::vector<VertexBufferElement>& getElements() const { return m_Elements; }
	inline unsigned int getStride() const { return m_Stride; };
};
	// Here because in-class specialization does not work under g++
	template<> inline
	void VertexBufferLayout::push<float>(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
	}

	template<> inline
	void VertexBufferLayout::push<unsigned int>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
	}

	template<> inline
	void VertexBufferLayout::push<unsigned char>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
	}