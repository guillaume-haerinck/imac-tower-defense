#pragma once

#include "vertex-array.hpp"
#include "index-buffer.hpp"
#include "shader.hpp"

class Renderer {
public:
	void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	// TODO add drawInstance + drawMulti
	void clear() const;
};