#include "player.hpp"
#include "../config.hpp"
#include "../background/background.hpp"
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
constexpr const char* relaxName = "relax.png";

Player::Player() 
    : m_sprite(m_textureRight), 
      m_currentState(PlayerState::UnderWater)
{
    std::string rightPath = std::string(spritesSrc) + rightSpriteName;
    std::string upPath = std::string(spritesSrc) + upSpriteName;
    std::string relaxPath = std::string(spritesSrc) + relaxName;
    m_horizontalMovementBlockTicks = 0;

    if (!m_textureRight.loadFromFile(rightPath)) {
        std::cerr << "Error: failed to load texture '" << rightPath << std::endl;
    }
    if (!m_textureUp.loadFromFile(upPath)) {
        std::cerr << "Error: failed to load texture '" << upPath << std::endl;
    }
    if (!m_textureRelax.loadFromFile(relaxPath)) {
        std::cerr << "Error: failed to load texture '" << relaxPath << std::endl;
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

void Player::SetRelaxSprite() {
    m_sprite.setTexture(m_textureRelax, true);
    sf::Vector2f currentScale = m_sprite.getScale();
    float absScaleX = std::abs(currentScale.x); 
    m_sprite.setScale({absScaleX, std::abs(currentScale.y)});
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

bool Player::CheckTileCollision(const sf::FloatRect& bounds, const Background& level, TileType type) const 
{
    int startX = static_cast<int>(bounds.position.x / level.GetTileSize());
    int endX = static_cast<int>((bounds.position.x + bounds.size.x) / level.GetTileSize());
    int startY = static_cast<int>(bounds.position.y / level.GetTileSize());
    int endY = static_cast<int>((bounds.position.y + bounds.size.y) / level.GetTileSize());

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (level.GetTile(x, y) == type) {
                return true; 
            }
        }
    }
    return false;
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

void Player::UpdateUnderWater(float deltaTime, const Background& level)
{
    float speed = PLAYER_SPEED; 
    sf::Vector2f movement(0.f, 0.f);

    bool isShiftPressed = 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

    if (isShiftPressed) {
        speed = PLAYER_SHIFT_SPEED; 
        FastMovement(movement, speed, deltaTime);
    } else {
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
    
    sf::FloatRect playerBounds = GetGlobalBounds();
    
    playerBounds.position.x += movement.x;
    if (CheckTileCollision(playerBounds, level, TileType::Ground)) {
        movement.x = 0.f;
    }
    
    playerBounds = GetGlobalBounds(); 
    playerBounds.position.y += movement.y;
    if (CheckTileCollision(playerBounds, level, TileType::Ground)) {
        movement.y = 0.f;
    }

    move(movement);
    
    sf::FloatRect currentBounds = GetGlobalBounds();
    sf::FloatRect centerHitbox;
    
    centerHitbox.position.x = currentBounds.position.x + currentBounds.size.x / 3.f;
    centerHitbox.position.y = currentBounds.position.y + currentBounds.size.y / 3.f;
    centerHitbox.size.x = 1.f;
    centerHitbox.size.y = 1.f;

    if (CheckTileCollision(centerHitbox, level, TileType::Surface)) {
        SetState(PlayerState::Surface);
    }
}

void Player::UpdateSurface(float deltaTime, const Background& level)
{
    SetRelaxSprite();
    bool isShiftPressed = 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
        
    if (isShiftPressed && 
       (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))) 
    {
        move({0.f, level.GetTileSize() * 0.5f}); 
        SetState(PlayerState::UnderWater);
    }
}

void Player::UpdateRelax(float deltaTime, const Background& level)
{
}

void Player::Update(float deltaTime, const Background& level)
{
    switch (m_currentState)
    {
        case PlayerState::UnderWater:
            UpdateUnderWater(deltaTime, level);
            break;
        case PlayerState::Surface:
            UpdateSurface(deltaTime, level);
            break;
        case PlayerState::Relax:
            UpdateRelax(deltaTime, level);
            break;
    }
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}