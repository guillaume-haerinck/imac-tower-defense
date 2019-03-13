#include "renderer.hpp"
#include "debug/gl-error-handling.hpp"

#include <iostream>
#include <glad/glad.h>

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
	shader.bind();
	va.bind();
	ib.bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::clear() const {
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}