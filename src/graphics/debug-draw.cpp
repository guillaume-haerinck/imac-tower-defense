#include "debug-draw.hpp"

#include <iostream>
#include <glad/glad.h>

#include "logger/gl-log-handler.hpp"

DebugDraw::DebugDraw() 
: m_shaderBasic("res/shaders/basic/basic.vert", "res/shaders/basic/basic.frag"),
  m_vertexBufferMaxSize(512),
  m_vb(nullptr, m_vertexBufferMaxSize * sizeof(float), GL_DYNAMIC_DRAW)
{
    // Vertex buffer layout
	VertexBufferLayout vbLayout;
	vbLayout.push<float>(2); // Pos (x, y)

    // Vertex array
	m_va.addBuffer(m_vb, vbLayout);

    // Uniforms
    m_shaderBasic.bind();
    m_shaderBasic.setUniformMat4f("u_mvp", glm::mat4(1.0f));
	m_shaderBasic.setUniform4f("u_color", 1.0f, 0.0f, 0.0f, 1.0f);

    // Cleanup
    m_shaderBasic.unbind();
    m_va.unbind();
    m_vb.unbind();
}

DebugDraw::~DebugDraw() {}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    //std::cout << "draw polygon asked" << std::endl;
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    //std::cout << "draw solid polygon asked" << std::endl;
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
    //std::cout << "draw circle asked" << std::endl;
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
    //std::cout << "draw solid circle asked" << std::endl;
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    //std::cout << "draw segment asked" << std::endl;
}

void DebugDraw::DrawTransform(const b2Transform& xf) {
    // TODO check if size of sent points fits into the m_vertexBufferMaxSize

    //std::cout << "draw transform asked" << std::endl;
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();

    float dataTest[] = {0.0f,  0.0f};
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexBufferMaxSize, &dataTest));
    
    m_shaderBasic.setUniformMat4f("u_mvp", glm::mat4(1.0f));
    m_shaderBasic.setUniform4f("u_color", 1.0f, 0.0f, 0.0f, 1.0f);
    GLCall(glDrawArrays(GL_POINTS, 0, 1));

    m_vb.unbind();
    m_va.unbind();
    m_shaderBasic.unbind();
}

void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
    //std::cout << "draw point asked" << std::endl;
}
