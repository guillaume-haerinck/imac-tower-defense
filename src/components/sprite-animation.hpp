#pragma once

namespace cmpt {
struct SpriteAnimation {
    SpriteAnimation(unsigned int startTile = 0, unsigned int endTile = 0, unsigned int activeTile = 0) 
    : startTile(startTile), endTile(endTile), activeTile(activeTile) {}

    unsigned int startTile;
    unsigned int endTile;
    unsigned int activeTile;
};
}
