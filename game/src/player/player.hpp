#pragma once
#include <SFML/Graphics.hpp>

class Player : public sf::Drawable, public sf::Transformable
{
public:
    Player();
    void Update(float deltaTime);
    sf::FloatRect getGlobalBounds() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Texture m_texture;
    sf::Sprite  m_sprite;
};