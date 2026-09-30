#include "enemy.hpp"
#include "../config.hpp"

constexpr float PLAYER_WIDTH = 100.f;
constexpr float PLAYER_HEIGHT = 40.f;

constexpr float minX = PLAYER_WIDTH / 2;
constexpr float maxX = SCREEN_WIDTH - PLAYER_WIDTH / 2;
constexpr float minY = PLAYER_HEIGHT / 2;
constexpr float maxY = SCREEN_HEIGHT - PLAYER_HEIGHT / 2;

int direction = 1;

Enemy::Enemy() 
{
    m_body.setSize({PLAYER_WIDTH, PLAYER_HEIGHT});
    m_body.setFillColor(sf::Color(255, 255, 0)); 
    m_body.setOrigin({PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2});
    m_body.setPosition({0.f, 0.f});
}

sf::FloatRect Enemy::getGlobalBounds() const
{
    return getTransform().transformRect(m_body.getGlobalBounds());
}

void Enemy::Update(float deltaTime)
{
    const float speed = 150.f;
    sf::Vector2f movement(0.f, 0.f);
    if (direction == 1) {
        movement.x += speed * deltaTime;
    }
    else {
        movement.x -= speed * deltaTime;
    }
    
    move(movement);
    sf::Vector2f currentPos = getPosition();

    if (currentPos.x < minX) 
    {
        currentPos.x = minX;
        direction = 1;
    }
    if (currentPos.x > maxX) 
    {
        currentPos.x = maxX;
        direction = 0;
    }
    if (currentPos.y < minY)
    {
        currentPos.y = minY;
    }
    if (currentPos.y > maxY) 
    {
        currentPos.y = maxY;
    }

    setPosition(currentPos);
}

void Enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_body, states);
}