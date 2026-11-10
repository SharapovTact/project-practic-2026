#include "player.hpp"
#include "../config.hpp"
#include "../level/level.hpp"
#include "../state-manager/state-manager.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>

namespace {
    bool lastSpaceState = false;
    bool isDivingFromSurface = false;

    bool IsUpPressed()    { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up); }
    bool IsDownPressed()  { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down); }
    bool IsLeftPressed()  { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left); }
    bool IsRightPressed() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right); }
}

constexpr float PLAYER_SPEED = 70.f;
constexpr float PLAYER_SHIFT_SPEED = 200.f;

constexpr float PRESSURE_MODIFIER = 0.02f;
constexpr float OXYGEN_LESS_SPEED = 4.f;
constexpr float OXYGEN_MORE_SPEED = 20.f;

constexpr float PLAYER_WIDTH = 128.f;
constexpr float PLAYER_HEIGHT = 128.f;
constexpr float minX = PLAYER_WIDTH / 2;
constexpr float maxX = SCREEN_WIDTH - PLAYER_WIDTH / 2;
constexpr float maxY = SCREEN_HEIGHT - PLAYER_HEIGHT / 2;
constexpr int   HORIZONTAL_MOVEMENT_DELAY = 50;

constexpr float FAST_SWIM_ANIM_SPEED = 1.7f;
constexpr float SURFACE_STICK_SPEED  = 8.f; 

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

    const float hbWidth  = 100.f;
    const float hbHeight = 30.f;
    const float hbOffsetY = 20.f; 

    m_hitbox = {
        {-hbWidth / 2.f, -hbHeight / 2.f + hbOffsetY}, 
        { hbWidth, hbHeight}                  
    };
}

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

void Player::DrawDebug(sf::RenderTarget& target) const
{
    sf::FloatRect hb = GetHitbox();

    sf::RectangleShape debugShape(hb.size);
    debugShape.setPosition(hb.position);
    debugShape.setFillColor(sf::Color::Transparent);
    debugShape.setOutlineColor(sf::Color::Red);
    debugShape.setOutlineThickness(1.f);
    target.draw(debugShape);
}

void Player::SetState(PlayerState newState) { m_currentState = newState; }
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

bool Player::CheckTileCollision(const sf::FloatRect& bounds, const Level& level, TileType type) const
{
    int startX = static_cast<int>(bounds.position.x / level.GetTileSize());
    int endX   = static_cast<int>((bounds.position.x + bounds.size.x) / level.GetTileSize());
    int startY = static_cast<int>(bounds.position.y / level.GetTileSize());
    int endY   = static_cast<int>((bounds.position.y + bounds.size.y) / level.GetTileSize());

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (level.GetTile(x, y) == type) {
                return true;
            }
        }
    }
    return false;
}

float Player::FindWaterSurfaceY(const Level& level) const
{
    const int tileX = static_cast<int>(getPosition().x / level.GetTileSize());
    int tileY = 0;
    while (tileY < level.GetHeight() && level.GetTile(tileX, tileY) == TileType::Surface) {
        ++tileY;
    }
    return tileY * level.GetTileSize();
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

    bool isShiftPressed =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

    bool currentSpaceState = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (!currentSpaceState) {
        isDivingFromSurface = false;
    }

    if (currentSpaceState) {
        speed = PLAYER_SHIFT_SPEED;
        m_horizontalMovementBlockTicks = HORIZONTAL_MOVEMENT_DELAY;

        if (isDivingFromSurface) {
            movement.y += speed * deltaTime;
        } else {
            movement.y -= speed * deltaTime;
        }

        if (IsLeftPressed()) {
            movement.x -= speed * deltaTime;
        }
        if (IsRightPressed()) {
            movement.x += speed * deltaTime;
        }
    }
    else {
        if (isShiftPressed) {
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
        m_animator.setSpeed(isShiftPressed ? FAST_SWIM_ANIM_SPEED : 1.f);
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
