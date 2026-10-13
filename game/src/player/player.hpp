#pragma once
#include <SFML/Graphics.hpp>

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
    void Update(float deltaTime);
    sf::FloatRect GetGlobalBounds() const;

    // Управление состояниями
    void SetState(PlayerState newState);
    PlayerState GetState() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void UpdateUnderWater(float deltaTime);
    void UpdateSurface(float deltaTime);
    void UpdateRelax(float deltaTime);

    void DrawUnderWater(sf::RenderTarget& target, sf::RenderStates states) const;
    void DrawSurface(sf::RenderTarget& target, sf::RenderStates states) const;
    void DrawRelax(sf::RenderTarget& target, sf::RenderStates states) const;

    void SetRightSprite();
    void SetLeftSprite();
    void SetUpSprite();
    void SetDownSprite();
    void SlowMovement(sf::Vector2f& movement, float speed, float deltaTime);
    void FastMovement(sf::Vector2f& movement, float speed, float deltaTime);

    sf::Texture m_textureRight;
    sf::Texture m_textureUp;
    sf::Sprite m_sprite;

    bool m_lastHorizontalPositionIsRight;
    int m_horizontalMovementBlockTicks;
    PlayerState m_currentState;
};