#pragma once
#include <SFML/Graphics.hpp>

class Player : public sf::Drawable, public sf::Transformable
{
public:
    Player();
    void Update(float deltaTime);
    sf::FloatRect GetGlobalBounds() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void SetRightSprite();
    void SetLeftSprite();
    void SetUpSprite();
    void SetDownSprite();
    void FastMovement(sf::Vector2f& movement, float speed, float deltaTime);
    void SlowMovement(sf::Vector2f& movement, float speed, float deltaTime);

    sf::Texture m_textureRight;
    sf::Texture m_textureUp;
    sf::Sprite  m_sprite;
    int         m_horizontalMovementBlockTicks;
    bool        m_lastHorizontalPositionIsRight;
};