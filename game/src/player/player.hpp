#pragma once
#include <SFML/Graphics.hpp>

class Background; 
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
    void Update(float deltaTime, const Background& level); 
    sf::FloatRect GetGlobalBounds() const;

    void SetState(PlayerState newState);
    PlayerState GetState() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void UpdateUnderWater(float deltaTime, const Background& level);
    void UpdateSurface(float deltaTime, const Background& level);
    void UpdateRelax(float deltaTime, const Background& level);

    void SetRelaxSprite();
    void SetRightSprite();
    void SetLeftSprite();
    void SetUpSprite();
    void SetDownSprite();
    void SlowMovement(sf::Vector2f& movement, float speed, float deltaTime);
    void FastMovement(sf::Vector2f& movement, float speed, float deltaTime);

    bool CheckTileCollision(const sf::FloatRect& bounds, const Background& level, TileType type) const;

    sf::Texture m_textureRelax;
    sf::Texture m_textureRight;
    sf::Texture m_textureUp;
    sf::Sprite m_sprite;

    bool m_lastHorizontalPositionIsRight;
    int m_horizontalMovementBlockTicks;
    PlayerState m_currentState;
};