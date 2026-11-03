#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "level-textures.hpp"

enum class TileType {
    Water = 0,
    Surface = 1,
    Ground = 2
};

class Level : public sf::Drawable
{
public:
    explicit Level(const LevelTextures& textures, const std::string& mapFilePath = "../../assets/level/level.txt");

    void Resize(int newWidth, int newHeight);
    void SetTile(int x, int y, TileType type);
    TileType GetTile(int x, int y) const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    float GetTileSize() const { return m_tileSize; }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    const LevelTextures& m_textures;

    std::vector<TileType> m_tiles;
    int m_width;
    int m_height;
    float m_tileSize;
};