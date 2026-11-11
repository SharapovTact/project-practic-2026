#include "player.hpp"
#include "../level/level.hpp"
#include <algorithm>
#include <cmath>

namespace {
    bool lastSpaceState = false;
    bool isDivingFromSurface = false;

    bool IsUpPressed()    { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up); }
    bool IsDownPressed()  { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down); }
    bool IsLeftPressed()  { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left); }
    bool IsRightPressed() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right); }
    bool IsShiftPressed() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift); }
}

constexpr float PLAYER_SPEED = 100.f;
constexpr float PLAYER_SHIFT_SPEED = 150.f;

constexpr float PRESSURE_MODIFIER = 0.02f;
constexpr int   HORIZONTAL_MOVEMENT_DELAY = 50;

constexpr float FAST_SWIM_ANIM_SPEED = 1.7f;
constexpr float SURFACE_STICK_SPEED  = 8.f; 

void Player::Reset(sf::Vector2f startPosition) {
    m_oxygen_percent = 100.f;

    m_currentState = PlayerState::UnderWater;
    m_horizontalMovementBlockTicks = 0;

    lastSpaceState = false;
    isDivingFromSurface = false;
    setPosition(startPosition);
    SetRightSprite();
    m_animator.play(diver::Anim::Hover, true);
}

void Player::StickToSurface(const Level& level, diver::Anim anim, float deltaTime)
{
    const float targetY = diver::hipYOnSurface(anim, FindWaterSurfaceY(level));
    const float k = std::min(1.f, SURFACE_STICK_SPEED * deltaTime);
    sf::Vector2f pos = getPosition();
    pos.y += (targetY - pos.y) * k;
    setPosition(pos);
}

void Player::SlowMovement(sf::Vector2f& movement, float speed, float deltaTime) {
    if (IsUpPressed()) {
        movement.y -= speed * deltaTime;
    }
    if (IsDownPressed()) {
        movement.y += speed * deltaTime;
    }
    if ((m_horizontalMovementBlockTicks == 0) && IsLeftPressed()) {
        movement.x -= speed * deltaTime;
        SetLeftSprite();
    }
    if ((m_horizontalMovementBlockTicks == 0) && IsRightPressed()) {
        movement.x += speed * deltaTime;
        SetRightSprite();
    }
}

void Player::FastMovement(sf::Vector2f& movement, float speed, float deltaTime) {
    if ((m_horizontalMovementBlockTicks == 0) && IsLeftPressed()) {
        movement.x -= speed * deltaTime;
        SetLeftSprite();
    }
    if ((m_horizontalMovementBlockTicks == 0) && IsRightPressed()) {
        movement.x += speed * deltaTime;
        SetRightSprite();
    }
}

static void NormalizationMovementVector(sf::Vector2f& movement, const float speed, const float deltaTime) {
    float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
    if (length > 0.f) {
        movement.x = (movement.x / length) * (speed * deltaTime);
        movement.y = (movement.y / length) * (speed * deltaTime);
    }
}

void Player::UpdateUnderWater(float deltaTime, const Level& level)
{
    float speed = PLAYER_SPEED;
    sf::Vector2f movement(0.f, 0.f);
    Player::LessOxygen(deltaTime);

    bool currentSpaceState = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (!currentSpaceState) {
        isDivingFromSurface = false;
    }

    if (currentSpaceState) {
        speed = PLAYER_SHIFT_SPEED;
        m_horizontalMovementBlockTicks = HORIZONTAL_MOVEMENT_DELAY;
        SetVerticalCollision(level);
        if (isDivingFromSurface) {
            movement.y += speed * deltaTime;
        } else {
            movement.y -= speed * deltaTime;
        }

        if (IsLeftPressed()) {
            movement.x -= speed * deltaTime;
            SetLeftSprite();     
        }
        if (IsRightPressed()) {
            movement.x += speed * deltaTime;
            SetRightSprite();
        }
    }
    else {
        SetHorizontalCollision(level);
        if (IsShiftPressed()) {
            speed = PLAYER_SHIFT_SPEED;
            FastMovement(movement, speed, deltaTime);
        } else {
            speed = PLAYER_SPEED;
            SlowMovement(movement, speed, deltaTime);
        }
    }

    if (currentSpaceState) {
        m_animator.play(isDivingFromSurface ? diver::Anim::VerticalDown : diver::Anim::VerticalUp);
    } else if (movement.x != 0.f) {
        m_animator.play(diver::Anim::Swim);
        m_animator.setSpeed(IsShiftPressed() ? FAST_SWIM_ANIM_SPEED : 1.f);
    } else {
        m_animator.play(diver::Anim::Hover);
    }

    lastSpaceState = currentSpaceState;

    if (m_horizontalMovementBlockTicks != 0) {
        m_horizontalMovementBlockTicks--;
    }
    NormalizationMovementVector(movement, speed, deltaTime);

    float currentDepth = GetHitbox().position.y;
    float pressureForce = currentDepth * PRESSURE_MODIFIER * deltaTime;
    movement.y += pressureForce;

    sf::FloatRect playerBounds = GetHitbox();
    playerBounds.position.x += movement.x;
    if (CheckTileCollision(playerBounds, level, TileType::Ground)) {
        movement.x = 0.f;
    }

    playerBounds = GetHitbox();
    playerBounds.position.y += movement.y;
    if (CheckTileCollision(playerBounds, level, TileType::Ground)) {
        movement.y = 0.f;
    }

    move(movement);

    sf::FloatRect currentBounds = GetHitbox();
    sf::FloatRect centerHitbox;
    centerHitbox.position.x = currentBounds.position.x + currentBounds.size.x / 3.f;
    centerHitbox.position.y = currentBounds.position.y + currentBounds.size.y / 3.f;
    centerHitbox.size = {1.f, 1.f};

    if (CheckTileCollision(centerHitbox, level, TileType::Surface)) {
        SetHorizontalCollision(level);
        SetState(PlayerState::Surface);
        m_animator.play(diver::Anim::Idle);
    }
}

void Player::UpdateSurface(float deltaTime, const Level& level)
{
    m_animator.play(diver::Anim::Idle);
    StickToSurface(level, diver::Anim::Idle, deltaTime);
    Player::MoreOxygen(deltaTime);

    bool currentSpaceState = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (currentSpaceState && !lastSpaceState) {
        SetState(PlayerState::Relax);
        m_animator.play(diver::Anim::Relax, true);
    }

    lastSpaceState = currentSpaceState;
}

void Player::UpdateRelax(float deltaTime, const Level& level)
{
    m_animator.play(diver::Anim::Relax);

    float speed = IsShiftPressed() ? PLAYER_SHIFT_SPEED : PLAYER_SPEED;

    sf::Vector2f movement(0.f, 0.f);
    if (IsLeftPressed()) {
        movement.x -= speed * deltaTime;
        SetLeftSprite();
    }
    if (IsRightPressed()) {
        movement.x += speed * deltaTime;
        SetRightSprite();
    }

    sf::FloatRect playerBounds = GetHitbox();
    playerBounds.position.x += movement.x;
    if (CheckTileCollision(playerBounds, level, TileType::Ground)) {
        movement.x = 0.f;
    }
    move(movement);

    m_animator.setSpeed(movement.x != 0.f ? (IsShiftPressed() ? 2.5f : 1.8f) : 1.f);

    StickToSurface(level, diver::Anim::Relax, deltaTime);
    Player::MoreOxygen(deltaTime);

    bool currentSpaceState = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (currentSpaceState && !lastSpaceState) {
        SetState(PlayerState::Dive);
        m_animator.play(diver::Anim::Dive, true);
    } else if (IsUpPressed()) {
        SetState(PlayerState::Surface);
        m_animator.play(diver::Anim::Idle, true);
    }

    lastSpaceState = currentSpaceState;
}

void Player::UpdateDive(float deltaTime, const Level&)
{
    Player::LessOxygen(deltaTime);

    if (m_animator.finished()) {
        move({0.f, diver::kDiveRootDy.back()});
        isDivingFromSurface = true; 
        SetState(PlayerState::UnderWater);
        m_animator.play(diver::Anim::VerticalDown, true);
    }

    lastSpaceState = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
}
