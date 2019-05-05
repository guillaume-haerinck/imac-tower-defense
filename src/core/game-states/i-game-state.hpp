#pragma once

#include "systems/render-system.hpp"
#include "systems/movement-system.hpp"
#include "systems/animation-system.hpp"
#include "systems/construction-system.hpp"
#include "systems/wave-system.hpp"
#include "systems/attack-system.hpp"
#include "systems/health-system.hpp"
#include "core/progression.hpp"
#include "events/handlers/event-emitter.hpp"

enum GameState {
	TITLE_SCREEN,
	LEVEL,
	CINEMATIC,
	GAME_OVER
};

class IGameState {
public:
	IGameState(Progression& progression,
		EventEmitter& emitter,
		AnimationSystem& animationSystem,
		AttackSystem& attackSystem,
		ConstructionSystem& constructionSystem,
		HealthSystem& healthSystem,
		MovementSystem& movementSystem,
		RenderSystem& renderSystem,
		WaveSystem& waveSystem)
	:   m_progression(progression),
		m_emitter(emitter),
		m_animationSystem(animationSystem),
		m_attackSystem(attackSystem),
		m_constructionSystem(constructionSystem),
		m_healthSystem(healthSystem),
		m_movementSystem(movementSystem),
		m_renderSystem(renderSystem),
		m_waveSystem(waveSystem)
	{}
	
	virtual void onEnter() = 0;
	virtual void update(float deltatime) = 0;
	virtual void onExit() = 0;

protected:
	Progression& m_progression;
	EventEmitter& m_emitter;
	AnimationSystem& m_animationSystem;
	AttackSystem& m_attackSystem;
	ConstructionSystem& m_constructionSystem;
	HealthSystem& m_healthSystem;
	MovementSystem& m_movementSystem;
	RenderSystem& m_renderSystem;
	WaveSystem& m_waveSystem;
};
