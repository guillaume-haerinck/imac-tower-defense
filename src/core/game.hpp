#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>

class Game {
public:
    Game();
    ~Game();

    int init();
    SDL_Window* getWindow();
    SDL_GLContext getContext();

private:
    bool isInit;
    SDL_Window* m_window;
    SDL_GLContext m_context;
};


