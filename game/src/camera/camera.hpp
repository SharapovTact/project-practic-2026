#pragma once
#include <SFML/Graphics.hpp>

class Level;

class Camera
{
public:
    Camera();
    void Update(const sf::Vector2f& targetPos, const Level& level);
    const sf::View& GetView() const;
private:
    sf::View m_view;
};