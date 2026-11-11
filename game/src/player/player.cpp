#include "player.hpp"
#include "../level/level.hpp"
#include "../state-manager/state-manager.hpp"
#include <algorithm>
#include <iostream>
#include <string>

constexpr float OXYGEN_LESS_SPEED = 4.f;
constexpr float OXYGEN_MORE_SPEED = 20.f;
constexpr float PIXELS_PER_METER = 64.f;

constexpr const char* spritesSrc = "../../assets/player/";
constexpr const char* sheetName  = "diver_sheet.png";

float Player::m_oxygen_percent = 100;

Player::Player()
    : m_sprite(m_textureSheet),
      m_animator(m_sprite),
      m_currentState(PlayerState::UnderWater)
{
    const std::string sheetPath = std::string(spritesSrc) + sheetName;
    m_horizontalMovementBlockTicks = 0;

    if (!m_textureSheet.loadFromFile(sheetPath)) {
        std::cerr << "Ошибка: не удалось загрузить текстуру '" << sheetPath << "'" << std::endl;
    }
    m_sprite.setTexture(m_textureSheet);

    m_lastHorizontalPositionIsRight = true;
    ApplyFacing();
    m_animator.play(diver::Anim::Hover, true);

    m_hitbox = {-HORIZONTAL_HITBOX / 2.f, HORIZONTAL_HITBOX};
}

float Player::GetOxygen() {
    return Player::m_oxygen_percent;
}

void Player::LessOxygen(float deltaTime) {
    if (Player::m_oxygen_percent > 0) {
        Player::m_oxygen_percent -= OXYGEN_LESS_SPEED * deltaTime;
    }
    if (Player::m_oxygen_percent < 0) {
        Player::m_oxygen_percent = 0;
    }
}

void Player::MoreOxygen(float deltaTime) {
    if (Player::m_oxygen_percent < 100) {
        Player::m_oxygen_percent += OXYGEN_MORE_SPEED * deltaTime;
    }
    if (Player::m_oxygen_percent > 100) {
        Player::m_oxygen_percent = 100;
    }
}

sf::FloatRect Player::GetHitbox() const
{
    return getTransform().transformRect(m_hitbox);
}

sf::FloatRect Player::GetGlobalBounds() const
{
    return getTransform().transformRect(m_sprite.getGlobalBounds());
}

void Player::SetState(PlayerState newState) { 
    m_currentState = newState; 
}
PlayerState Player::GetState() const { return m_currentState; }

void Player::ApplyFacing() {
    m_sprite.setScale({m_lastHorizontalPositionIsRight ? 1.f : -1.f, 1.f});
}

void Player::SetRightSprite() {
    m_lastHorizontalPositionIsRight = true;
    ApplyFacing();
}

void Player::SetLeftSprite() {
    m_lastHorizontalPositionIsRight = false;
    ApplyFacing();
}

float Player::GetDepth(const Level& level) const
{
    if (m_currentState == PlayerState::Surface || m_currentState == PlayerState::Relax) {
        return 0.f;
    }
    const float y = getPosition().y + m_animator.rootOffset().y;
    return std::max(0.f, (y - FindWaterSurfaceY(level)) / PIXELS_PER_METER);
}

void UpdateGameState() {
    if (Player::GetOxygen() == 0) {
        StateManager::SetGameState(GameState::GameOver);
    }
}

void Player::Update(float deltaTime, const Level& level)
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
        case PlayerState::Dive:
            UpdateDive(deltaTime, level);
            break;
    }

    m_animator.update(sf::seconds(deltaTime));
    m_sprite.setPosition(m_animator.rootOffset());

    UpdateGameState();
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}
