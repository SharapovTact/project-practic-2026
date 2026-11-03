#include "hud.hpp"
#include <iostream>
#include <string>

namespace {
    constexpr const char*  FONT_PATH = "../../assets/fonts/arial.ttf";
    constexpr unsigned int OXYGEN_FONT_SIZE = 24;
    constexpr unsigned int STATE_FONT_SIZE = 64;
    constexpr float MARGIN = 20.f;
}

HUD::HUD(): 
    m_oxygenText(m_font, "", OXYGEN_FONT_SIZE), 
    m_stateText (m_font, "", STATE_FONT_SIZE), 
    m_currentState(GameState::Play)
{
    if (!m_font.openFromFile(FONT_PATH)) { 
        std::cerr << "Error: failed to load font '" << FONT_PATH << "'\n";
    }

    m_oxygenText.setFillColor(sf::Color::White);
    m_oxygenText.setPosition({MARGIN, MARGIN});

    m_stateText.setFillColor(sf::Color::White);

    m_dimOverlay.setFillColor(sf::Color(0, 0, 0, 170));
}

void HUD::Update(float oxygenPercent, GameState state)
{
    m_currentState = state;

    m_oxygenText.setString(
        "Oxygen: " + std::to_string(static_cast<int>(oxygenPercent)) + "%"
    );
}

void HUD::Draw(sf::RenderWindow& window)
{
    // В главном меню HUD не нужен: меню рисует себя само.
    if (m_currentState == GameState::Menu) {
        return;
    }

    const sf::Vector2u size = window.getSize();
    window.setView(sf::View(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(size.x), static_cast<float>(size.y)}
    )));

    window.draw(m_oxygenText);

    if (m_currentState != GameState::Play) {
        DrawOverlay(window);
    }
}

void HUD::DrawOverlay(sf::RenderWindow& window)
{
    const sf::Vector2u size = window.getSize();

    m_dimOverlay.setSize({static_cast<float>(size.x), static_cast<float>(size.y)});
    m_dimOverlay.setPosition({0.f, 0.f});
    window.draw(m_dimOverlay);

    std::string text;
    switch (m_currentState) {
        case GameState::Menu:
            text = "MAIN MENU";
            break;
        case GameState::Pause:    
            text = "PAUSED";    
            break;
        case GameState::GameOver: 
            text = "GAME OVER\nPress Space"; 
            break;
        case GameState::Win:      
            text = "YOU WIN";   
            break;
    }
    m_stateText.setString(text);

    const sf::FloatRect b = m_stateText.getLocalBounds();
    m_stateText.setOrigin({
        b.position.x + b.size.x / 2.f,
        b.position.y + b.size.y / 2.f
    });
    m_stateText.setPosition({
        static_cast<float>(size.x) / 2.f,
        static_cast<float>(size.y) / 2.f
    });
    window.draw(m_stateText);
}