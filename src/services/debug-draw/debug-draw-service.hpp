#pragma once

#include <glm/glm.hpp>

#include "i-debug-draw.hpp"
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

class DebugDrawService : public IDebugDraw {
public:
    DebugDrawService(glm::mat4 viewMat = glm::mat4(1.0f), glm::mat4 projMat = glm::mat4(1.0f));
    virtual ~DebugDrawService();

	/* Box2d Draw API */
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
	void DrawTransform(const b2Transform& xf) override;
	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override;

	/* Processing-Like API */
	void triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
	void rect(float x1, float y1, float x2, float y2) override;
	void square(float x, float y, float extent) override;
	void ellipse(float a, float b, float c, float d) override;
	void circleWithGlow(float x, float y, float r) override;
	void quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
	void line(float x1, float y1, float x2, float y2, BasicShaderType shaderType = BASIC) override;
	void point(float x, float y) override;
	void shape(glm::vec2* vertices) override;

	/* Setters */
	void setProjMat(glm::mat4 mat) override;
	void setViewMat(glm::mat4 mat) override;
	void setColor(glm::vec4 color) override;
	void setColor(float r, float g, float b, float a) override;

	//Shaders
	Shader& getShader(BasicShaderType shaderType);

private:
    Shader m_shaderBasic;
	Shader m_shaderLaser;
	Shader m_shaderCircleWithGlow;
	VertexArray m_va;
	VertexBuffer m_vb;
	unsigned int m_vbMaxSize;
	glm::mat4 m_viewMat;
	glm::mat4 m_projMat;
	glm::vec4 m_color;
};

