#pragma once

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>

#include "graphics/shader.hpp"
#include "graphics/vertex-array.hpp"

/**
 * @brief Immediate-mode debug drawing
 * @note Makes heavy use of glBufferData to update vertex array at each function call
 */

/* 
    TODO check if better performance with immutable buffer (size is set and cannot change, update with subdata) http://docs.gl/gl4/glBufferStorage 
    http://voidptr.io/blog/2016/04/28/ldEngine-Part-1.html
    https://stackoverflow.com/questions/27810542/what-is-the-difference-between-glbufferstorage-and-glbufferdata
*/

class DebugDraw : public b2Draw {
public:
    DebugDraw(glm::mat4 viewMat = glm::mat4(1.0f), glm::mat4 projMat = glm::mat4(1.0f));
    ~DebugDraw();

	/* Box2d Draw API */
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
	void DrawTransform(const b2Transform& xf) override;
	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override;

	/* Processing-Like API */
	void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
	void rect(float x1, float y1, float x2, float y2);
	void square(float x, float y, float extent);
	void ellipse(float a, float b, float c, float d);
	void quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	void line(float x1, float y1, float x2, float y2);
	void point(float x, float y);
	void shape(glm::vec2* vertices);

	/* Setters */
	void setProjMat(glm::mat4 mat);
	void setViewMat(glm::mat4 mat);
	void setColor(glm::vec4 color);

private:
    Shader m_shaderBasic;
	VertexArray m_va;
	VertexBuffer m_vb;
	unsigned int m_vbMaxSize;
	glm::mat4 m_viewMat;
	glm::mat4 m_projMat;
	glm::vec4 m_color;
};

