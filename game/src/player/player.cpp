#include "player.hpp"
#include "../config.hpp"
#include "../level/level.hpp"
#include "../state-manager/state-manager.hpp"
#include <iostream>
#include <string>
#include <cmath>

namespace {
    bool lastSpaceState = false;
    bool isDivingFromSurface = false;
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

constexpr const char* spritesSrc = "../../assets/player/";
constexpr const char* rightSpriteName = "right-start.png";
constexpr const char* upSpriteName = "up-start.png";
constexpr const char* relaxName = "relax.png";

float Player::m_oxygen_percent = 100;

Player::Player()
    : m_sprite(m_textureRight),
      m_currentState(PlayerState::UnderWater)
{
    std::string rightPath = std::string(spritesSrc) + rightSpriteName;
    std::string upPath    = std::string(spritesSrc) + upSpriteName;
    std::string relaxPath = std::string(spritesSrc) + relaxName;
    m_horizontalMovementBlockTicks = 0;

    if (!m_textureRight.loadFromFile(rightPath)) {
        std::cerr << "Error: failed to load texture '" << rightPath << "'" << std::endl;
    }
    if (!m_textureUp.loadFromFile(upPath)) {
        std::cerr << "Error: failed to load texture '" << upPath << "'" << std::endl;
    }
    if (!m_textureRelax.loadFromFile(relaxPath)) {
        std::cerr << "Error: failed to load texture '" << relaxPath << "'" << std::endl;
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
    m_lastHorizontalPositionIsRight = true;

    lastSpaceState = false;
    isDivingFromSurface = false;
    setPosition(startPosition);
    SetRightSprite();
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
            SetDownSprite();
        } else {
            movement.y -= speed * deltaTime;
            SetUpSprite();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            movement.x -= speed * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
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

    lastSpaceState = currentSpaceState;

    if (!currentSpaceState
        && !(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        && !(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        && !(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
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
    }
}

void Player::UpdateSurface(float deltaTime, const Level& level)
{
    SetRelaxSprite();

    Player::MoreOxygen(deltaTime);

    bool currentSpaceState = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (currentSpaceState && !lastSpaceState)
    {
        isDivingFromSurface = true;
        move({0.f, level.GetTileSize() * 0.5f});
        SetState(PlayerState::UnderWater);
    }

    lastSpaceState = currentSpaceState;
}

void Player::UpdateRelax(float , const Level&)
{
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
    }
    UpdateGameState();
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}