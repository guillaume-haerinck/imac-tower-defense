#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

class Game {
public:
    Game(entt::DefaultRegistry& registry);
    ~Game();

    int init();
    SDL_Window* getWindow();
    SDL_GLContext getContext();

private:
    bool m_bInit;
	static bool m_bInstanciated;
    SDL_Window* m_window;
    SDL_GLContext m_context;
	entt::DefaultRegistry& m_registry;
};


