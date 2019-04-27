#include "debug-draw-service.hpp"

#include <SDL2/SDL.h>
#include <iostream>
#include <glad/glad.h>
#include <vector>

#include "logger/gl-log-handler.hpp"
#include "core/constants.hpp"

DebugDrawService::DebugDrawService(glm::mat4 viewMat, glm::mat4 projMat)
:	m_shaderBasic("res/shaders/basic/basic.vert", "res/shaders/basic/basic.frag"),
	m_vbMaxSize(32),
	m_vb(nullptr, 0, GL_DYNAMIC_DRAW),
	m_viewMat(viewMat),
	m_projMat(projMat),
	m_color(255., 255., 255., 1.)
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

DebugDrawService::~DebugDrawService() {
    unsigned int vaId = m_va.getID();
    GLCall(glDeleteVertexArrays(1, &vaId));
    m_vb.~VertexBuffer();
}

/* ------------------------------- BOX2I API ------------------------------- */

void DebugDrawService::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
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
    GLCall(glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW));
    
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

void DebugDrawService::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
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
    GLCall(glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW));
    
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

void DebugDrawService::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();

    // Update buffer
    unsigned int segmentNumber = 17; // TODO define segment number based on radius
    const float TWO_PI = (float) M_PI * 2;
    std::vector<float> array;
    for (unsigned int i = 0; i <= segmentNumber; i++) {
		array.push_back(radius * cos(i * TWO_PI / segmentNumber) + center.x);
		array.push_back(radius * sin(i * TWO_PI / segmentNumber) + center.y);
	}
    GLCall(glBufferData(GL_ARRAY_BUFFER, array.size() * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW));
    
    // Update
    glm::mat4 mvp = m_projMat * m_viewMat;
    m_shaderBasic.setUniformMat4f("u_mvp", mvp);
    m_shaderBasic.setUniform4f("u_color", color.r, color.g, color.b, color.a);
    GLCall(glDrawArrays(GL_LINE_STRIP, 0, segmentNumber + 1));

    // Unbinding
    m_vb.unbind();
    m_va.unbind();
    m_shaderBasic.unbind();
}

void DebugDrawService::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();

    // Update buffer
    unsigned int segmentNumber = 17; // TODO define segment number based on radius
    const float TWO_PI = (float) M_PI * 2;
    std::vector<float> array;
	array.push_back(center.x);
	array.push_back(center.y);
    for (unsigned int i = 0; i <= segmentNumber; i++) {
		array.push_back(radius * cos(i * TWO_PI / segmentNumber) + center.x);
		array.push_back(radius * sin(i * TWO_PI / segmentNumber) + center.y);
	}
    GLCall(glBufferData(GL_ARRAY_BUFFER, array.size() * 2 * sizeof(float), array.data(), GL_DYNAMIC_DRAW));
    
    // Update
    glm::mat4 mvp = m_projMat * m_viewMat;
    m_shaderBasic.setUniformMat4f("u_mvp", mvp);
    m_shaderBasic.setUniform4f("u_color", color.r * 0.5f, color.g * 0.5f, color.b * 0.5f, color.a * 0.5f);
    GLCall(glDrawArrays(GL_TRIANGLE_FAN, 0, segmentNumber + 2));

    // Unbinding
    m_vb.unbind();
    m_va.unbind();
    m_shaderBasic.unbind();

    // Draw a line fixed in the circle to animate rotation.
	b2Vec2 endpoint = center + radius * axis;
    DrawSegment(center, endpoint, color);
}

void DebugDrawService::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();

    // Update
    float data[] = {
        p1.x, p1.y,
        p2.x, p2.y
    };
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), &data, GL_DYNAMIC_DRAW));
    
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

void DebugDrawService::DrawTransform(const b2Transform& xf) {
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();

    // Update Y axis
    float axisScale = 5.0f;
    b2Vec2 origin = xf.p;
    b2Vec2 endPoint = origin + axisScale * xf.q.GetYAxis();
    float yAxis[] = {
        origin.x,   origin.y,
        endPoint.x, endPoint.y
    };
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), &yAxis, GL_DYNAMIC_DRAW));
    
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

void DebugDrawService::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
    // Binding
    m_shaderBasic.bind();
    m_va.bind();
    m_vb.bind();
    GLCall(glPointSize(size));

    // Update buffer
    float data[] = {p.x,  p.y};
    GLCall(glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), &data, GL_DYNAMIC_DRAW));
    
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

/* ---------------------------- PROCESSING-LIKE API --------------------------- */

void DebugDrawService::triangle(float x1, float y1, float x2, float y2, float x3, float y3) {

}

void DebugDrawService::rect(float x1, float y1, float x2, float y2) {

}

void DebugDrawService::square(float x, float y, float extent) {

}

void DebugDrawService::ellipse(float a, float b, float c, float d) {

}

void DebugDrawService::quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

}

void DebugDrawService::line(float x1, float y1, float x2, float y2) {
	// Binding
	m_shaderBasic.bind();
	m_va.bind();
	m_vb.bind();

	// Update
	float data[] = {
		x1, y1,
		x2, y2
	};
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), &data, GL_DYNAMIC_DRAW));

	// Render
	glm::mat4 mvp = m_projMat * m_viewMat;
	m_shaderBasic.setUniformMat4f("u_mvp", mvp);
	m_shaderBasic.setUniform4f("u_color", m_color.r, m_color.g, m_color.b, m_color.a);
	GLCall(glDrawArrays(GL_LINES, 0, 4));

	// Unbinding
	m_vb.unbind();
	m_va.unbind();
	m_shaderBasic.unbind();
}

void DebugDrawService::point(float x, float y) {
	// Binding
	m_shaderBasic.bind();
	m_va.bind();
	m_vb.bind();

	// Update buffer
	float data[] = { x * WIN_RATIO,  y };
	GLCall(glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), &data, GL_DYNAMIC_DRAW));

	// Update
	glm::mat4 mvp = m_projMat * m_viewMat;
	m_shaderBasic.setUniformMat4f("u_mvp", mvp);
	m_shaderBasic.setUniform4f("u_color", m_color.r, m_color.g, m_color.b, m_color.a);
	GLCall(glDrawArrays(GL_POINTS, 0, 1));

	// Unbinding
	m_vb.unbind();
	m_va.unbind();
	m_shaderBasic.unbind();
}

void DebugDrawService::shape(glm::vec2* vertices) {
	// TODO
}

// ------------------------------ SETTERS ---------------------------
void DebugDrawService::setProjMat(glm::mat4 mat) { m_projMat = mat; }
void DebugDrawService::setViewMat(glm::mat4 mat) { m_viewMat = mat; }
void DebugDrawService::setColor(glm::vec4 color) { m_color = color; }
void DebugDrawService::setColor(float r, float g, float b, float a) { m_color = glm::vec4(r, g, b, a); }
