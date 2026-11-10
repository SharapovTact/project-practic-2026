#pragma once
#include <SFML/Graphics.hpp>
#include "../state-manager/state-manager.hpp"

class HUD
{
public:
    HUD();

    void Update(float oxygenPercent, GameState state);

    void Draw(sf::RenderWindow& window);
     void UpdateView(float deltaTime, bool underwater, float depthMeters);

private:
    void DrawOverlay(sf::RenderWindow& window);

    sf::Font m_font;
    sf::Text m_oxygenText;
    sf::Text m_stateText;
    sf::RectangleShape m_dimOverlay;
    GameState m_currentState;
    sf::Text m_depthText;
    float    m_depth = 0.f;

    sf::Shader m_vignette;
    bool  m_vignetteReady = false;
    float m_oxygen = 100.f;
    sf::Clock m_clock;

    sf::Texture        m_maskTexture;
    sf::Sprite         m_maskSprite;
    bool               m_maskLoaded = false;
    sf::RectangleShape m_tint;
    float              m_underwaterMix = 0.f;
};