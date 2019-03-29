#include "debug-draw.hpp"

#include <iostream>

DebugDraw::DebugDraw() {}

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
    //std::cout << "draw transform asked" << std::endl;
}

void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
    //std::cout << "draw point asked" << std::endl;
}
