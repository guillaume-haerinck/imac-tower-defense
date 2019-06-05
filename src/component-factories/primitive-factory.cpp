#include "primitive-factory.hpp"

#include "logger/gl-log-handler.hpp"
#include "graphics/vertex-array.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "graphics/texture-array.hpp"

PrimitiveFactory::PrimitiveFactory() 
: m_shaderBasic("res/shaders/basic/basic.vert", "res/shaders/basic/basic.frag")
{}

cmpt::Primitive PrimitiveFactory::createRect(glm::vec4 color, glm::vec2 displaySize, PivotPoint pivot) {
	const std::array<float, 8> positions = getVertexPositions(displaySize, pivot);
    VertexBuffer vb(positions.data(), positions.size() * sizeof(float), GL_STATIC_DRAW);
	VertexBufferLayout vbLayout;
	vbLayout.push<float>(2); // Pos (x, y)

    /* Vertex array */
    VertexArray va;
	va.addBuffer(vb, vbLayout);

    /* Uniforms */
    m_shaderBasic.bind();
    m_shaderBasic.setUniformMat4f("u_mvp", glm::mat4(1.0f));
	m_shaderBasic.setUniform4f("u_color", color.r, color.g, color.b, color.a);

    /* Cleanup */
    m_shaderBasic.unbind();
    va.unbind();
    vb.unbind();

    cmpt::Primitive rectangle(color, va.getID(), GL_TRIANGLE_STRIP, &m_shaderBasic, 4);
    return rectangle;
}

cmpt::Primitive PrimitiveFactory::createRectOutline(glm::vec4 color, glm::vec2 displaySize, PivotPoint pivot) {
	const std::array<float, 8> positions = getVertexPositions(displaySize, pivot);
	VertexBuffer vb(positions.data(), positions.size() * sizeof(float), GL_STATIC_DRAW);
	VertexBufferLayout vbLayout;
	vbLayout.push<float>(2); // Pos (x, y)

    /* Vertex array */
    VertexArray va;
	va.addBuffer(vb, vbLayout);

    /* Uniforms */
    m_shaderBasic.bind();
    m_shaderBasic.setUniformMat4f("u_mvp", glm::mat4(1.0f));
	m_shaderBasic.setUniform4f("u_color", color.r, color.g, color.b, color.a);

    /* Cleanup */
    m_shaderBasic.unbind();
    va.unbind();
    vb.unbind();

    cmpt::Primitive rectangle(color, va.getID(), GL_LINE_LOOP, &m_shaderBasic, 4);
    return rectangle;
}

std::array<float, 8> PrimitiveFactory::getVertexPositions(glm::vec2 displaySize, PivotPoint pivot) {
	displaySize /= 2;

	switch (pivot) {
	case PivotPoint::MIDDLE_LEFT:
		{
			const std::array<float, 8> vertexPositions = {
				 0,					 displaySize.y, // 0
				 displaySize.x * 2,  displaySize.y,	// 1
				 0,					-displaySize.y, // 2
				 displaySize.x * 2, -displaySize.y	// 3
			};
			return vertexPositions;
			break;
		}

		default:
		// Center
		{
			const std::array<float, 8> vertexPositions = {
				-displaySize.x,  displaySize.y, // 0
				 displaySize.x,  displaySize.y, // 1
				-displaySize.x, -displaySize.y, // 2
				 displaySize.x, -displaySize.y  // 3
			};
			return vertexPositions;
			break;
		}
	}
}

