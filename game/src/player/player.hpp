#pragma once
#include <SFML/Graphics.hpp>

class Level;
enum class TileType;

enum class PlayerState
{
    UnderWater,
    Surface,
    Relax
};

class Player : public sf::Drawable, public sf::Transformable
{
public:
    Player();
    void Update(float deltaTime, const Level& level);
    sf::FloatRect GetGlobalBounds() const;
    void Reset(sf::Vector2f startPosition);

    void SetState(PlayerState newState);
    PlayerState GetState() const;

    static void LessOxygen(float deltaTime);
    static void MoreOxygen(float deltaTime);
    static float GetOxygen();

private:
    static float m_oxygen_percent;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void UpdateUnderWater(float deltaTime, const Level& level);
    void UpdateSurface(float deltaTime, const Level& level);
    void UpdateRelax(float deltaTime, const Level& level);

    void SetRelaxSprite();
    void SetRightSprite();
    void SetLeftSprite();
    void SetUpSprite();
    void SetDownSprite();
    void SlowMovement(sf::Vector2f& movement, float speed, float deltaTime);
    void FastMovement(sf::Vector2f& movement, float speed, float deltaTime);

    bool CheckTileCollision(const sf::FloatRect& bounds, const Level& level, TileType type) const;

    sf::Texture m_textureRelax;
    sf::Texture m_textureRight;
    sf::Texture m_textureUp;
    sf::Sprite m_sprite;

    bool m_lastHorizontalPositionIsRight;
    int m_horizontalMovementBlockTicks;
    PlayerState m_currentState;
};