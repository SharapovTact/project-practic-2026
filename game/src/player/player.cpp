#include "player.hpp"
#include "../config.hpp"
#include <iostream>

constexpr float PLAYER_WIDTH = 100.f;
constexpr float PLAYER_HEIGHT = 100.f;
constexpr float minX = PLAYER_WIDTH / 2;
constexpr float maxX = SCREEN_WIDTH - PLAYER_WIDTH / 2;
constexpr float minY = PLAYER_HEIGHT / 2;
constexpr float maxY = SCREEN_HEIGHT - PLAYER_HEIGHT / 2;

Player::Player() 
    : m_sprite(m_texture)
{
    if (!m_texture.loadFromFile("../../assets/player.png"))
    {
        std::cerr << "Error: failed to load texture '../../assets/player.png'\n";
        return;
    }
    m_sprite.setTexture(m_texture, true);

    const sf::Vector2u texSize = m_texture.getSize();
    if (texSize.x > 0 && texSize.y > 0)
    {
        m_sprite.setOrigin({texSize.x / 2.f, texSize.y / 2.f});

        const float scaleX = PLAYER_WIDTH / static_cast<float>(texSize.x);
        const float scaleY = PLAYER_HEIGHT / static_cast<float>(texSize.y);
        m_sprite.setScale({scaleX, scaleY});
    }
}
sf::FloatRect Player::getGlobalBounds() const
{
    return getTransform().transformRect(m_sprite.getGlobalBounds());
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

    if (currentPos.x < minX) currentPos.x = minX;
    if (currentPos.x > maxX) currentPos.x = maxX;
    if (currentPos.y < minY) currentPos.y = minY;
    if (currentPos.y > maxY) currentPos.y = maxY;

    setPosition(currentPos);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}