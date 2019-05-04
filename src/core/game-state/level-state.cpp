#include "level-state.hpp"

LevelState::LevelState() {
}

LevelState::~LevelState() {
}

void LevelState::update(Game& game, double deltatime) {
	// Game updates
	// TODO create deltatima with this 6/1000 factor ? must check where it is used and why there is such a unit
	game.animationSystem->update(deltatime / 1000 * 6); 
	game.movementSystem->update(deltatime);
	game.attackSystem->update();
	game.renderSystem->update();
}
