#pragma once

class Game; // Forward declaration

enum GameState {
	TITLE_SCREEN,
	LEVEL,
	CINEMATIC,
	GAME_OVER
};

class IGameState {
public:
	IGameState(Game& game) : m_game(game) {}

	virtual void onEnter() = 0;
	virtual void update(float deltatime) = 0;
	virtual void onExit() = 0;

protected:
	Game& m_game;
};
