#include "character.hpp"

Character::Character() 
{
    _body.setSize({100.f, 60.f});
    _body.setFillColor(sf::Color(70, 130, 180)); 
    _body.setOrigin({50.f, 30.f});
    _body.setPosition({0.f, 0.f});

    _head.setRadius(25.f);
    _head.setFillColor(sf::Color(240, 200, 160));
    _head.setOrigin({25.f, 25.f});
    _head.setPosition({0.f, -55.f});

    _hat.setPointCount(3);
    _hat.setPoint(0, {0.f, -40.f});
    _hat.setPoint(1, {-25.f, 30.f});
    _hat.setPoint(2, {25.f, 30.f});
    _hat.setFillColor(sf::Color(200, 50, 50));
    _hat.setPosition({0.f, -100.f});
}

void Character::update(float deltaTime)
{
}

void Character::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(_body, states);
    target.draw(_head, states);
    target.draw(_hat, states);
}