#pragma once

#include <Box2D/Box2D.h>

#include "graphics/shader.hpp"
#include "graphics/vertex-array.hpp"

/**
 * @brief Immediate-mode debug drawing
 * @note Makes heavy use of glBufferSubData to update vertex array at each function call
 */

class DebugDraw : public b2Draw {
public:
    DebugDraw();
    ~DebugDraw();

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

	void DrawTransform(const b2Transform& xf) override;

	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override;

private:
    Shader m_shaderBasic;
	VertexArray m_va;
	VertexBuffer m_vb;
	unsigned int m_vertexBufferMaxSize;
};

