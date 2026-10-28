#pragma once
#include <SFML/Graphics.hpp>
#include "../state-manager/state-manager.hpp"

class HUD
{
public:
    HUD();

    void Update(float oxygenPercent, GameState state);

    void Draw(sf::RenderWindow& window);

private:
    void DrawOverlay(sf::RenderWindow& window);

    sf::Font m_font;
    sf::Text m_oxygenText;
    sf::Text m_stateText;
    sf::RectangleShape m_dimOverlay;
    GameState m_currentState;
};