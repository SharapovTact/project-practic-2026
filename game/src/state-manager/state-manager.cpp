    #include "state-manager.hpp"
    #include <iostream>
    #include <string>
    #include <cmath>

    GameState StateManager::m_gameState = GameState::Play; 

    StateManager::StateManager() {
        StateManager::m_gameState = GameState::Play;
    }

    void StateManager::SetGameState(GameState state) {
        StateManager::m_gameState = state;
    }

    GameState StateManager::GetGameState() {
        return StateManager::m_gameState;
    }