#pragma once
#include <SFML/Graphics.hpp>

class Character : public sf::Drawable, public sf::Transformable
{
public: 
    Character();
    void update(float deltaTime);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::RectangleShape _body;
    sf::CircleShape    _head;
    sf::ConvexShape    _hat;
};