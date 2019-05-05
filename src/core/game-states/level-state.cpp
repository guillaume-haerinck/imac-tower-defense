#include "level-state.hpp"

#include <spdlog/spdlog.h>

LevelState::LevelState(EventEmitter& emitter,
	AnimationSystem& animationSystem,
	AttackSystem& attackSystem,
	ConstructionSystem& constructionSystem,
	HealthSystem& healthSystem,
	MovementSystem& movementSystem,
	RenderSystem& renderSystem,
	WaveSystem& waveSystem)
: IGameState(emitter,
		animationSystem,
		attackSystem,
		constructionSystem,
		healthSystem,
		movementSystem,
		renderSystem,
		waveSystem)
{}

void LevelState::update(float deltatime) {
	// TODO create deltatima with this 6/1000 factor ? must check where it is used and why there is such a unit
	// TODO ne pas utiliser le deltatime comme ça, il est quasiment fixe à chaque frame
	m_animationSystem.update(deltatime / 1000 * 6); 
	m_movementSystem.update(deltatime);
	m_attackSystem.update(deltatime);
	m_renderSystem.update(deltatime);
}

void LevelState::onEnter() {
	// Set event subscription for used systems 
	m_animationSystem.connectEvents();
	m_attackSystem.connectEvents();
	m_constructionSystem.connectEvents();
	m_healthSystem.connectEvents();
	m_movementSystem.connectEvents();
	m_renderSystem.connectEvents();
	m_waveSystem.connectEvents();
}

void LevelState::onExit() {

}
