#include "debug-draw.hpp"

#include <iostream>
#include <glad/glad.h>
#include <vector>

#include "logger/gl-log-handler.hpp"

DebugDraw::DebugDraw(glm::mat4 viewMat, glm::mat4 projMat)
: m_shaderBasic("res/shaders/basic/basic.vert", "res/shaders/basic/basic.frag"),
  m_vbMaxSize(64),
  m_vb(nullptr, m_vbMaxSize * sizeof(float), GL_DYNAMIC_DRAW),
  m_viewMat(viewMat),
  m_projMat(projMat)
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

DebugDraw::~DebugDraw() {
    unsigned int vaId = m_va.getID();
    GLCall(glDeleteVertexArrays(1, &vaId));
    m_vb.~VertexBuffer();
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    assert(vertexCount < m_vbMaxSize * 2);
    
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();

    // Update
    std::vector<float> array(vertexCount * 2);
    unsigned int dataIndex = 0;
    for (int i = 0; i < vertexCount; i++) {
        array[dataIndex] = vertices[i].x;
        array[dataIndex + 1] = vertices[i].y;
        dataIndex += 2;
	}
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * 2 * sizeof(float), array.data()));
    
    // Render
    glm::mat4 mvp = m_projMat * m_viewMat;
    m_shaderBasic.setUniformMat4f("u_mvp", mvp);
    m_shaderBasic.setUniform4f("u_color", color.r, color.g, color.b, color.a);
    GLCall(glDrawArrays(GL_LINE_LOOP, 0, vertexCount));
    
    // Unbinding
    m_vb.unbind();
    m_va.unbind();
    m_shaderBasic.unbind();
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    assert(vertexCount < m_vbMaxSize * 2);
    
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();

    // Update
    std::vector<float> array(vertexCount * 2);
    unsigned int dataIndex = 0;
    for (int i = 0; i < vertexCount; i++) {
        array[dataIndex] = vertices[i].x;
        array[dataIndex + 1] = vertices[i].y;
        dataIndex += 2;
	}
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * 2 * sizeof(float), array.data()));
    
    // Render outline
    glm::mat4 mvp = m_projMat * m_viewMat;
    m_shaderBasic.setUniformMat4f("u_mvp", mvp);
    m_shaderBasic.setUniform4f("u_color", color.r, color.g, color.b, color.a);
    GLCall(glDrawArrays(GL_LINE_LOOP, 0, vertexCount));
    
    // Render inside TODO use a tesselation shader to fill the inside completely no matter the shape
    // polygon triangulation
    m_shaderBasic.setUniform4f("u_color", color.r * 0.5f, color.g * 0.5f, color.b * 0.5f, color.a * 0.5f);
    GLCall(glDrawArrays(GL_TRIANGLES, 0, vertexCount));
    
    // Unbinding
    m_vb.unbind();
    m_va.unbind();
    m_shaderBasic.unbind();
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
    std::cout << "draw circle asked" << std::endl;
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
    std::cout << "draw solid circle asked" << std::endl;
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();

    // Update
    float data[] = {
        p1.x, p1.y,
        p2.x, p2.y
    };
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(float), &data));
    
    // Render
    glm::mat4 mvp = m_projMat * m_viewMat;
    m_shaderBasic.setUniformMat4f("u_mvp", mvp);
    m_shaderBasic.setUniform4f("u_color", color.r, color.g, color.b, color.a);
    GLCall(glDrawArrays(GL_LINES, 0, 4));
    
    // Unbinding
    m_vb.unbind();
    m_va.unbind();
    m_shaderBasic.unbind();
}

void DebugDraw::DrawTransform(const b2Transform& xf) {
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();

    // TODO add triangle at the end to make an arrow

    // Update Y axis
    float axisScale = 10.0f;
    b2Vec2 origin = xf.p;
    b2Vec2 endPoint = origin + axisScale * xf.q.GetYAxis();
    float yAxis[] = {
        origin.x,   origin.y,
        endPoint.x, endPoint.y
    };
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(float), &yAxis));
    
    // Render Y axis
    glm::mat4 mvp = m_projMat * m_viewMat;
    m_shaderBasic.setUniformMat4f("u_mvp", mvp);
    m_shaderBasic.setUniform4f("u_color", 0.0f, 1.0f, 0.0f, 1.0f); // Y axis in Green
    GLCall(glDrawArrays(GL_LINES, 0, 4));

    // Update X axis
    endPoint = origin + axisScale * xf.q.GetXAxis();
    float xAxis[] = {
        origin.x,   origin.y,
        endPoint.x, endPoint.y
    };
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(float), &xAxis));

    // Render X axis
    m_shaderBasic.setUniform4f("u_color", 1.0f, 0.0f, 0.0f, 1.0f); // X axis in Red
    GLCall(glDrawArrays(GL_LINES, 0, 4));
    
    // Unbinding
    m_vb.unbind();
    m_va.unbind();
    m_shaderBasic.unbind();
}

void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();
    GLCall(glPointSize(size));

    // Update buffer
    float data[] = {p.x,  p.y};
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(float), &data));
    
    // Update
    glm::mat4 mvp = m_projMat * m_viewMat;
    m_shaderBasic.setUniformMat4f("u_mvp", mvp);
    m_shaderBasic.setUniform4f("u_color", color.r, color.g, color.b, color.a);
    GLCall(glDrawArrays(GL_POINTS, 0, 1));

    // Unbinding
    GLCall(glPointSize(1));
    m_vb.unbind();
    m_va.unbind();
    m_shaderBasic.unbind();
}

// ------------------------------ SETTERS ---------------------------
void DebugDraw::setProjMat(glm::mat4 mat) { m_projMat = mat; }
void DebugDraw::setViewMat(glm::mat4 mat) { m_viewMat = mat; }
