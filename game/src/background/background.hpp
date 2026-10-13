#pragma once
#include <SFML/Graphics.hpp>

class Background : public sf::Drawable
{
public:
    Background();

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Texture m_surfaceTexture;
    sf::Texture m_waterTexture;
    sf::Sprite  m_surfaceSprite;
    sf::Sprite  m_waterSprite;
};