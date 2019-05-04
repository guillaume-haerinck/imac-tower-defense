#include "level-state.hpp"

#include <spdlog/spdlog.h>

LevelState::LevelState() {
}

LevelState::~LevelState() {
}

void LevelState::update(float deltatime, AnimationSystem& animationSystem, MovementSystem& movementSystem, AttackSystem& attackSystem, RenderSystem& renderSystem) {
	// TODO create deltatima with this 6/1000 factor ? must check where it is used and why there is such a unit
	animationSystem.update(deltatime / 1000 * 6); 
	movementSystem.update(deltatime);
	attackSystem.update();
	renderSystem.update();
}
