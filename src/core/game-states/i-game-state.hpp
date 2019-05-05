#pragma once

#include "systems/render-system.hpp"
#include "systems/movement-system.hpp"
#include "systems/animation-system.hpp"
#include "systems/construction-system.hpp"
#include "systems/wave-system.hpp"
#include "systems/attack-system.hpp"
#include "systems/health-system.hpp"
#include "events/handlers/event-emitter.hpp"

class IGameState {
public:
	IGameState(EventEmitter& emitter, AnimationSystem& animationSystem, MovementSystem& movementSystem, AttackSystem& attackSystem, RenderSystem& renderSystem)
	: m_emitter(emitter), m_animationSystem(animationSystem), m_movementSystem(movementSystem), m_attackSystem(attackSystem), m_renderSystem(renderSystem)
	{}
	
	virtual void onEnter() = 0;
	virtual void update(float deltatime) = 0;
	virtual void onExit() = 0;

protected:
	EventEmitter& m_emitter;
	AnimationSystem& m_animationSystem;
	MovementSystem& m_movementSystem;
	AttackSystem& m_attackSystem;
	RenderSystem& m_renderSystem;
};
