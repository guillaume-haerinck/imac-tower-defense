#include "level-state.hpp"

#include <spdlog/spdlog.h>

LevelState::LevelState(EventEmitter& emitter, AnimationSystem& animationSystem, MovementSystem& movementSystem, AttackSystem& attackSystem, RenderSystem& renderSystem)
: IGameState(emitter, animationSystem, movementSystem, attackSystem, renderSystem)
{}

void LevelState::update(float deltatime) {
	// TODO create deltatima with this 6/1000 factor ? must check where it is used and why there is such a unit
	// TODO ne pas utiliser le deltatime comme ça, il est quasiment fixe à chaque frame
	m_animationSystem.update(deltatime / 1000 * 6); 
	m_movementSystem.update(deltatime);
	m_attackSystem.update();
	m_renderSystem.update();
}

void LevelState::onEnter() {

}

void LevelState::onExit() {

}
