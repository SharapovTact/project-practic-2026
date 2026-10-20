#pragma once
#include <SFML/Graphics.hpp>

class Background; 

class Camera 
{
public:
    Camera();
    void Update(const sf::Vector2f& targetPos, const Background& background);
    const sf::View& GetView() const;
private:
    sf::View m_view;
};