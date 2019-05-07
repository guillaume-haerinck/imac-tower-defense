#include "i-game-state.hpp"

#include "core/game.hpp"

IGameState::IGameState(Game& game) : InputHandler(game.emitter), m_game(game) {}
