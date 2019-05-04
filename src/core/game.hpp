#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

#include "game-state/i-game-state.hpp"
#include "level/level.hpp"
#include "systems/render-system.hpp"
#include "systems/movement-system.hpp"
#include "systems/animation-system.hpp"
#include "systems/construction-system.hpp"
#include "systems/wave-system.hpp"
#include "systems/attack-system.hpp"
#include "systems/health-system.hpp"
#include "events/handlers/event-emitter.hpp"

class Game {
public:
	// Lifetime
    Game(EventEmitter& emitter);
    ~Game();
    int init();

	void update();

	// Getters
    SDL_Window* getWindow();
    SDL_GLContext getContext();

	// Setters
	//void setState(GameState state);

public:
	// Game structure
	entt::DefaultRegistry registry;
	EventEmitter& emitter;
	Level* level;

	// System
	RenderSystem* renderSystem;
	AnimationSystem* animationSystem;
	MovementSystem* movementSystem;
	ConstructionSystem* constructionSystem;
	WaveSystem* waveSystem;
	AttackSystem* attackSystem;
	HealthSystem* healthSystem;

private:
	// Lifetime
    static bool m_bInit;
	static bool m_bInstanciated;

	// SDL
    SDL_Window* m_window;
    SDL_GLContext m_context;

	// State machine
	//GameState m_gameState;

	// Camera
	glm::mat4 m_projMat;
	glm::mat4 m_viewMat;
	glm::vec2 m_viewTranslation;
	float m_viewScale;
};
