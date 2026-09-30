#include "player.hpp"
#include "../config.hpp"

constexpr float PLAYER_WIDTH = 100.f;
constexpr float PLAYER_HEIGHT = 40.f;

constexpr float minX = PLAYER_WIDTH / 2;
constexpr float maxX = SCREEN_WIDTH - PLAYER_WIDTH / 2;
constexpr float minY = PLAYER_HEIGHT / 2;
constexpr float maxY = SCREEN_HEIGHT - PLAYER_HEIGHT / 2;

Player::Player() 
{
    m_body.setSize({PLAYER_WIDTH, PLAYER_HEIGHT});
    m_body.setFillColor(sf::Color(128, 128, 128)); 
    m_body.setOrigin({PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2});
    m_body.setPosition({0.f, 0.f});
}

void Player::Update(float deltaTime)
{
    const float speed = 300.f;
    sf::Vector2f movement(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        movement.y -= speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        movement.x -= speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        movement.y += speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        movement.x += speed * deltaTime;
    }

    move(movement);
    sf::Vector2f currentPos = getPosition();

    if (currentPos.x < minX) 
    {
        currentPos.x = minX;
    }
    if (currentPos.x > maxX) 
    {
        currentPos.x = maxX;
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

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_body, states);
}