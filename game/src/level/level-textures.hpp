#pragma once
#include <SFML/Graphics.hpp>

class LevelTextures
{
public:
    bool Load();

    bool IsLoaded() const { return m_loaded; }
    bool HasBackground() const { return m_hasBackground; }

    const sf::Texture& Surface() const { return m_surface; }
    const sf::Texture& Water() const { return m_water; }
    const sf::Texture& Ground() const { return m_ground; }
    const sf::Texture& Background() const { return m_background; }

private:
    sf::Texture m_surface;
    sf::Texture m_water;
    sf::Texture m_ground;
    sf::Texture m_background;

    bool m_loaded = false;
    bool m_hasBackground = false;
};
