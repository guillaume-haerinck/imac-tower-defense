#pragma once

#include "systems/render-system.hpp"
#include "systems/movement-system.hpp"
#include "systems/animation-system.hpp"
#include "systems/construction-system.hpp"
#include "systems/wave-system.hpp"
#include "systems/attack-system.hpp"
#include "systems/health-system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "i-game-state.hpp"

class LevelState : public IGameState {
public:
	LevelState(EventEmitter& emitter, AnimationSystem& animationSystem, MovementSystem& movementSystem, AttackSystem& attackSystem, RenderSystem& renderSystem);

	void onEnter() override;
	void update(float deltatime) override;
	void onExit() override;
};
