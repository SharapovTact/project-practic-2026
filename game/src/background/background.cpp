#include "background.hpp"
#include "../config.hpp"
#include <iostream>


constexpr float WATER_LEVEL = SCREEN_HEIGHT * SURFACE_PROPORTION;

Background::Background()
    : m_surfaceSprite(m_surfaceTexture),
      m_waterSprite(m_waterTexture)
{
    if (!m_surfaceTexture.loadFromFile("../../assets/background/surface.png"))
    {
        std::cerr << "Error: failed to load surface texture\n";
    }
    else
    {
        m_surfaceTexture.setRepeated(true);

        int surfaceWidth = static_cast<int>(SCREEN_WIDTH);
        int surfaceHeight = static_cast<int>(WATER_LEVEL);

        m_surfaceSprite.setTextureRect(sf::IntRect({0, 0}, {surfaceWidth, surfaceHeight}));
        m_surfaceSprite.setPosition({0.f, 0.f});
    }

    if (!m_waterTexture.loadFromFile("../../assets/background/water.png"))
    {
        std::cerr << "Error: failed to load water texture\n";
    }
    else
    {
        m_waterTexture.setRepeated(true);

        int waterWidth = static_cast<int>(SCREEN_WIDTH);
        int waterHeight = static_cast<int>(SCREEN_HEIGHT - WATER_LEVEL);

        m_waterSprite.setTextureRect(sf::IntRect({0, 0}, {waterWidth, waterHeight}));
        m_waterSprite.setPosition({0.f, WATER_LEVEL});
    }
}

void Background::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_surfaceSprite, states);
    target.draw(m_waterSprite, states);
}