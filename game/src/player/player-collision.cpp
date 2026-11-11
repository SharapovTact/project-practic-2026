#include "player.hpp"
#include "../level/level.hpp"

static sf::FloatRect MakeHitbox(sf::Vector2f size)
{
    return {{-size.x / 2.f, -size.y / 2.f}, size};
}

bool Player::CanChangeHitbox(const sf::FloatRect& newHitbox, const Level& level) const
{
    sf::FloatRect big = MakeHitbox(BIG_HITBOX);
    big.position += getPosition();
    if (!CheckTileCollision(big, level, TileType::Ground)) {
        return true;
    }

    sf::FloatRect target = newHitbox;
    target.position += getPosition();
    return !CheckTileCollision(target, level, TileType::Ground);
}

void Player::SetVerticalCollision(const Level& level) {
    const sf::FloatRect newHitbox = MakeHitbox(VERTICAL_HITBOX);
    if (m_hitboxOrientation != HitboxOrientation::Vertical && CanChangeHitbox(newHitbox, level)) {
        m_hitbox = newHitbox;
        m_hitboxOrientation = HitboxOrientation::Vertical;
    }
}

void Player::SetHorizontalCollision(const Level& level) {
    const sf::FloatRect newHitbox = MakeHitbox(HORIZONTAL_HITBOX);
    if (m_hitboxOrientation != HitboxOrientation::Horizontal && CanChangeHitbox(newHitbox, level)) {
        m_hitbox = newHitbox;
        m_hitboxOrientation = HitboxOrientation::Horizontal;
    }
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
        tileY++;
    }
    return tileY * level.GetTileSize();
}

void Player::DrawDebug(sf::RenderTarget& target) const
{
    sf::FloatRect big = MakeHitbox(BIG_HITBOX);
    big.position += getPosition();

    sf::RectangleShape bigShape(big.size);
    bigShape.setPosition(big.position);
    bigShape.setFillColor(sf::Color::Transparent);
    bigShape.setOutlineColor(sf::Color::Yellow);
    bigShape.setOutlineThickness(1.f);
    target.draw(bigShape);

    sf::FloatRect hb = GetHitbox();

    sf::RectangleShape debugShape(hb.size);
    debugShape.setPosition(hb.position);
    debugShape.setFillColor(sf::Color::Transparent);
    debugShape.setOutlineColor(sf::Color::Red);
    debugShape.setOutlineThickness(1.f);
    target.draw(debugShape);
}
