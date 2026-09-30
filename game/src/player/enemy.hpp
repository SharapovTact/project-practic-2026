#pragma once
#include <SFML/Graphics.hpp>

class Enemy : public sf::Drawable, public sf::Transformable
{
public: 
    Enemy();
    void Update(float deltaTime);
    sf::FloatRect getGlobalBounds() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::RectangleShape m_body;
};