#pragma once
#include <SFML/Graphics.hpp>

class StateManager;

enum class GameState
{
    Menu,
    Play,
    Pause,
    GameOver,
    Win,
};

class StateManager
{
public:
    StateManager();
    static GameState GetGameState();
    static void SetGameState(GameState state);

private:
    static GameState m_gameState;
};