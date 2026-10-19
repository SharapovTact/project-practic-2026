#include "background.hpp"
#include "../config.hpp"
#include <iostream>
#include <algorithm>
#include <string>

// ~ (вода)
// # (поверхность)
// G (земля)
const std::vector<std::string> LEVEL_DESIGN = {
    "#########################",
    "#########################",
    "~~~~~~~~~~~~~~~~~~~~~~~~~",
    "~~~~~~~~~~~~~~~~~~~~~~~~~",
    "~~~~~~~~~~~~~~~~~~~~~~~~~",
    "~~~~~~~~~~~~~~~~~~~~~~~~~",
    "~~~~~~~~~~~~~~~~~~~~~~~~~",
    "~~~~~~~~~~~~~~~~~~~~~~~~~",
    "~~~~~~~~~~~~~~~~~~~~~~~~~",
    "~~~~~~~~~~~~~~~~~~~~~~~~~",
    "G~~~~~~~~~~~~~~~~~~~~~~~G",
    "G~~~~~~~~~~GGG~~~~~~~~~~G",
    "GGGGGGGGGGGGGGGGGGGGGGGGG",
    "GGGGGGGGGGGGGGGGGGGGGGGGG"
};

Background::Background()
    : m_width(0), m_height(0), m_tileSize(64.f)
{
    if (!m_surfaceTexture.loadFromFile("../../assets/background/surface.png")) {
        std::cerr << "Error: failed to load surface texture\n";
    }
    if (!m_waterTexture.loadFromFile("../../assets/background/water.png")) {
        std::cerr << "Error: failed to load water texture\n";
    }
    if (!m_groundTexture.loadFromFile("../../assets/background/ground.png")) {
        std::cerr << "Error: failed to load ground texture\n";
    }

    int mapHeight = LEVEL_DESIGN.size();
    int mapWidth = (mapHeight > 0) ? LEVEL_DESIGN[0].size() : 0;

    Resize(mapWidth, mapHeight);

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            char tileChar = LEVEL_DESIGN[y][x];

            if (tileChar == '#') {
                SetTile(x, y, TileType::Surface);
            } else if (tileChar == '~') {
                SetTile(x, y, TileType::Water);
            } else if (tileChar == 'G') {
                SetTile(x, y, TileType::Ground);
            } else {
                SetTile(x, y, TileType::Water); 
            }
        }
    }
}

void Background::Resize(int newWidth, int newHeight) {
    if (newWidth == m_width && newHeight == m_height) return;
    std::vector<TileType> newTiles(newWidth * newHeight, TileType::Water);
    int copyWidth = std::min(m_width, newWidth);
    int copyHeight = std::min(m_height, newHeight);
    for (int y = 0; y < copyHeight; ++y) {
        for (int x = 0; x < copyWidth; ++x) {
            newTiles[y * newWidth + x] = m_tiles[y * m_width + x];
        }
    }
    m_tiles = std::move(newTiles);
    m_width = newWidth;
    m_height = newHeight;
}

void Background::SetTile(int x, int y, TileType type) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) m_tiles[y * m_width + x] = type;
}

TileType Background::GetTile(int x, int y) const {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) return m_tiles[y * m_width + x];
    return TileType::Ground;
}

void Background::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::Sprite tileSprite(m_surfaceTexture);
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            TileType type = GetTile(x, y);
            
            if (type == TileType::Surface) {
                tileSprite.setTexture(m_surfaceTexture);
            } else if (type == TileType::Ground) {
                tileSprite.setTexture(m_groundTexture);
            } else {
                tileSprite.setTexture(m_waterTexture);
            }
            
            tileSprite.setPosition({x * m_tileSize, y * m_tileSize});
            sf::Vector2u texSize = tileSprite.getTexture().getSize();
            tileSprite.setScale({
                m_tileSize / static_cast<float>(texSize.x), 
                m_tileSize / static_cast<float>(texSize.y)
            });

            target.draw(tileSprite, states);
        }
    }
}