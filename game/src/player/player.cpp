#include "player.hpp"
#include "../config.hpp"
#include <iostream>
#include <string>
#include <cmath>

constexpr float PLAYER_SPEED = 100.f;
constexpr float PLAYER_SHIFT_SPEED = 200.f;

constexpr float PLAYER_WIDTH = 128.f;
constexpr float PLAYER_HEIGHT = 128.f;
constexpr float minX = PLAYER_WIDTH / 2;
constexpr float maxX = SCREEN_WIDTH - PLAYER_WIDTH / 2;
constexpr float maxY = SCREEN_HEIGHT - PLAYER_HEIGHT / 2;
constexpr int   HORIZONTAL_MOVEMENT_DELAY = 200;

constexpr const char* spritesSrc = "../../assets/player/";
constexpr const char* rightSpriteName = "right-start.png";
constexpr const char* upSpriteName = "up-start.png";

Player::Player() 
    : m_sprite(m_textureRight), 
      m_currentState(PlayerState::UnderWater)
{
    std::string rightPath = std::string(spritesSrc) + rightSpriteName;
    std::string upPath = std::string(spritesSrc) + upSpriteName;
    m_horizontalMovementBlockTicks = 0;

    if (!m_textureRight.loadFromFile(rightPath)) {
        std::cerr << "Error: failed to load texture '" << rightPath << std::endl;
    }
    if (!m_textureUp.loadFromFile(upPath)) {
        std::cerr << "Error: failed to load texture '" << upPath << std::endl;
    }

    m_sprite.setTexture(m_textureRight, true);
    m_lastHorizontalPositionIsRight = true;

    const sf::Vector2u textureSize = m_textureRight.getSize();
    if (textureSize.x > 0 && textureSize.y > 0)
    {
        m_sprite.setOrigin({textureSize.x / 2.f, textureSize.y / 2.f});

        const float scaleX = PLAYER_WIDTH / static_cast<float>(textureSize.x);
        const float scaleY = PLAYER_HEIGHT / static_cast<float>(textureSize.y);
        m_sprite.setScale({scaleX, scaleY});
    }
}

sf::FloatRect Player::GetGlobalBounds() const
{
    return getTransform().transformRect(m_sprite.getGlobalBounds());
}

void Player::SetState(PlayerState newState)
{
    m_currentState = newState;
}

PlayerState Player::GetState() const
{
    return m_currentState;
}

void Player::SetRightSprite() {
    m_lastHorizontalPositionIsRight = true;
    m_sprite.setTexture(m_textureRight, true);
    sf::Vector2f currentScale = m_sprite.getScale();
    float absScaleX = std::abs(currentScale.x); 
    m_sprite.setScale({absScaleX, std::abs(currentScale.y)});
}

void Player::SetLeftSprite() {
    m_lastHorizontalPositionIsRight = false;
    m_sprite.setTexture(m_textureRight, true);
    sf::Vector2f currentScale = m_sprite.getScale();
    float absScaleX = std::abs(currentScale.x); 
    m_sprite.setScale({-absScaleX, std::abs(currentScale.y)});
}

void Player::SetUpSprite() {
    m_sprite.setTexture(m_textureUp, true);
    sf::Vector2f currentScale = m_sprite.getScale();
    float absScaleY = std::abs(currentScale.y);
    m_sprite.setScale({currentScale.x, absScaleY});
}

void Player::SetDownSprite() {
    m_sprite.setTexture(m_textureUp, true);
    sf::Vector2f currentScale = m_sprite.getScale();
    float absScaleY = std::abs(currentScale.y);
    m_sprite.setScale({currentScale.x, -absScaleY});
}

void Player::SlowMovement(sf::Vector2f& movement, float speed, float deltaTime) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        movement.y -= speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        movement.y += speed * deltaTime;
    }
    if ((m_horizontalMovementBlockTicks == 0) &&
        (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)))
    {
        movement.x -= speed * deltaTime;
        SetLeftSprite();
    }
    if ((m_horizontalMovementBlockTicks == 0) &&
        (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)))
    {
        movement.x += speed * deltaTime;
        SetRightSprite();
    }
}

void Player::FastMovement(sf::Vector2f& movement, float speed, float deltaTime) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        m_horizontalMovementBlockTicks = HORIZONTAL_MOVEMENT_DELAY;
        movement.y -= speed * deltaTime;
        SetUpSprite();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        m_horizontalMovementBlockTicks = HORIZONTAL_MOVEMENT_DELAY;
        movement.y += speed * deltaTime;
        SetDownSprite();
    }
    if ((m_horizontalMovementBlockTicks == 0) &&
        (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)))
    {
        movement.x -= speed * deltaTime;
        SetLeftSprite();
    }
    if ((m_horizontalMovementBlockTicks == 0) &&
        (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)))
    {
        movement.x += speed * deltaTime;
        SetRightSprite();
    }
}

void NormalizationMovementVector(sf::Vector2f& movement, const float speed, const float deltaTime) {
    float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
    if (length > 0.f) {
        movement.x = (movement.x / length) * (speed * deltaTime);
        movement.y = (movement.y / length) * (speed * deltaTime);
    }
}

void Player::UpdateUnderWater(float deltaTime)
{
    float speed = PLAYER_SPEED; 
    sf::Vector2f movement(0.f, 0.f);

    bool isShiftPressed = 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

    if (isShiftPressed) 
    {
        speed = PLAYER_SHIFT_SPEED; 
        FastMovement(movement, speed, deltaTime);
    } 
    else 
    {
        speed = PLAYER_SPEED;
        SlowMovement(movement, speed, deltaTime);
    }

    if (!(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        && !(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        && !(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        && !(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    ) {
        if (m_lastHorizontalPositionIsRight) {
            SetRightSprite();
        } else {
            SetLeftSprite();
        }
    }

    if (m_horizontalMovementBlockTicks != 0) {
        m_horizontalMovementBlockTicks--;
    }
    
    NormalizationMovementVector(movement, speed, deltaTime);
    move(movement);
    
    sf::Vector2f currentPos = getPosition();
    const float surfaceY = SCREEN_HEIGHT * SURFACE_PROPORTION;

    if (currentPos.x < minX) currentPos.x = minX;
    if (currentPos.x > maxX) currentPos.x = maxX;
    
    if (currentPos.y < surfaceY) 
    {
        currentPos.y = surfaceY;
        SetState(PlayerState::Surface);
    }
    if (currentPos.y > maxY) currentPos.y = maxY;

    setPosition(currentPos);
}

void Player::UpdateSurface(float deltaTime)
{
    (void)deltaTime; 
}

void Player::UpdateRelax(float deltaTime)
{
    (void)deltaTime; 
}

void Player::DrawUnderWater(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_sprite, states);
}

void Player::DrawSurface(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_sprite, states);
}

void Player::DrawRelax(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_sprite, states);
}

void Player::Update(float deltaTime)
{
    switch (m_currentState)
    {
        case PlayerState::UnderWater:
            UpdateUnderWater(deltaTime);
            break;
        case PlayerState::Surface:
            UpdateSurface(deltaTime);
            break;
        case PlayerState::Relax:
            UpdateRelax(deltaTime);
            break;
    }
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    
    switch (m_currentState)
    {
        case PlayerState::UnderWater:
            DrawUnderWater(target, states);
            break;
        case PlayerState::Surface:
            DrawSurface(target, states);
            break;
        case PlayerState::Relax:
            DrawRelax(target, states);
            break;
    }
}