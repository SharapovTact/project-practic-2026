#pragma once
#include <SFML/Graphics.hpp>

class Player : public sf::Drawable, public sf::Transformable
{
public: 
    Player();
    void Update(float deltaTime);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::RectangleShape m_body;
};