#pragma once

namespace cmpt {
struct SpriteAnimation {
    SpriteAnimation(unsigned int startTile , unsigned int endTile , float duration) 
    : startTile(startTile), endTile(endTile), activeTile(startTile), age(0), duration(duration) {}

    unsigned int startTile;
    unsigned int endTile;
    unsigned int activeTile;

	float age;
	float duration;
};
}
