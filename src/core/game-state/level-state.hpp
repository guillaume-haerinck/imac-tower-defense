#pragma once

#include "systems/render-system.hpp"
#include "systems/movement-system.hpp"
#include "systems/animation-system.hpp"
#include "systems/construction-system.hpp"
#include "systems/wave-system.hpp"
#include "systems/attack-system.hpp"
#include "systems/health-system.hpp"

class LevelState {
public:
	LevelState();
	~LevelState();

	void update(float deltatime, AnimationSystem& animationSystem, MovementSystem& movementSystem, AttackSystem& attackSystem, RenderSystem& renderSystem);
};
