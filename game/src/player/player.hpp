#pragma once
#include <SFML/Graphics.hpp>
#include "diver-animations.hpp"

class Level;
enum class TileType;

constexpr sf::Vector2f HORIZONTAL_HITBOX = {100.f, 30.f};
constexpr sf::Vector2f VERTICAL_HITBOX   = {30.f, 100.f};
constexpr sf::Vector2f BIG_HITBOX        = {108.f, 108.f};

enum class HitboxOrientation {
    Horizontal,
    Vertical
};

enum class PlayerState
{
    UnderWater,
    Surface,
    Relax,
    Dive
};

class Player : public sf::Transformable, public sf::Drawable
{
public:
    Player();
    void Update(float deltaTime, const Level& level);
    void Reset(sf::Vector2f startPosition);

    sf::FloatRect GetHitbox() const;    
    sf::FloatRect GetLocalHitbox() const { return m_hitbox; }
    sf::FloatRect GetGlobalBounds() const;  

    void DrawDebug(sf::RenderTarget& target) const;

    void SetState(PlayerState newState);
    PlayerState GetState() const;
    diver::Anim GetAnim() const { return m_animator.current(); }
    float GetAnimSpeed() const { return m_animator.speed(); }
    float GetDepth(const Level& level) const;

    static void LessOxygen(float deltaTime);
    static void MoreOxygen(float deltaTime);
    static float GetOxygen();

    HitboxOrientation GetHitboxOrientation() const { return m_hitboxOrientation; }

private:
    HitboxOrientation m_hitboxOrientation = HitboxOrientation::Horizontal;
    static float m_oxygen_percent;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void UpdateUnderWater(float deltaTime, const Level& level);
    void UpdateSurface(float deltaTime, const Level& level);
    void UpdateRelax(float deltaTime, const Level& level);
    void UpdateDive(float deltaTime, const Level& level);
    void SetVerticalCollision(const Level& level);
    void SetHorizontalCollision(const Level& level);
    bool CanChangeHitbox(const sf::FloatRect& newHitbox, const Level& level) const;

    void SetRightSprite(); 
    void SetLeftSprite(); 
    void ApplyFacing();
    void SlowMovement(sf::Vector2f& movement, float speed, float deltaTime);
    void FastMovement(sf::Vector2f& movement, float speed, float deltaTime);

    float FindWaterSurfaceY(const Level& level) const;
    void  StickToSurface(const Level& level, diver::Anim anim, float deltaTime);

    bool CheckTileCollision(const sf::FloatRect& bounds, const Level& level, TileType type) const;

    sf::Texture     m_textureSheet;
    sf::Sprite      m_sprite;
    diver::Animator m_animator;
    sf::FloatRect   m_hitbox; 

    bool m_lastHorizontalPositionIsRight;
    int  m_horizontalMovementBlockTicks;
    PlayerState m_currentState;
};
