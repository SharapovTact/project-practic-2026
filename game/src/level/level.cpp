#include "level.hpp"
#include "../config.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>

Level::Level(const LevelTextures& textures, const std::string& mapFilePath)
    : m_textures(textures), m_width(0), m_height(0), m_tileSize(64.f)
{
    std::vector<std::string> levelDesign;
    std::ifstream mapFile(mapFilePath);

    if (mapFile.is_open()) {
        std::string line;
        while (std::getline(mapFile, line)) {
            if (!line.empty()) {
                levelDesign.push_back(line);
            }
        }
        mapFile.close();
    } else {
        std::cerr << "Ошибка: не удалось загрузить карту уровня: " << mapFilePath << "\n";
        levelDesign = {"GGGG", "~~~~", "####"};
    }

    int mapHeight = static_cast<int>(levelDesign.size());
    int mapWidth = (mapHeight > 0) ? static_cast<int>(levelDesign[0].size()) : 0;

    Resize(mapWidth, mapHeight);

    for (int y = 0; y < m_height; y++) {
        int currentLineWidth = static_cast<int>(levelDesign[y].size());

        for (int x = 0; x < m_width; x++) {
            char tileChar = (x < currentLineWidth) ? levelDesign[y][x] : '~';

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

void Level::Resize(int newWidth, int newHeight) {
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

void Level::SetTile(int x, int y, TileType type) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) m_tiles[y * m_width + x] = type;
}

TileType Level::GetTile(int x, int y) const {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) return m_tiles[y * m_width + x];
    return TileType::Ground;
}

void Level::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!m_textures.IsLoaded()) {
        return;
    }

    if (m_textures.HasBackground()) {
        const sf::Vector2u bgSize = m_textures.Background().getSize();
        if (bgSize.x > 0 && bgSize.y > 0) {
            sf::Sprite bgSprite(m_textures.Background());
            bgSprite.setScale({
                (m_width * m_tileSize) / static_cast<float>(bgSize.x),
                (m_height * m_tileSize) / static_cast<float>(bgSize.y)
            });
            target.draw(bgSprite, states);
        }
    }

    sf::Sprite tileSprite(m_textures.Surface());

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            TileType type = GetTile(x, y);

            if (type == TileType::Surface) {
                tileSprite.setTexture(m_textures.Surface());
                tileSprite.setColor(sf::Color(255, 255, 255, 255));
            } else if (type == TileType::Ground) {
                tileSprite.setTexture(m_textures.Ground());
                tileSprite.setColor(sf::Color(255, 255, 255, 255));
            } else {
                tileSprite.setTexture(m_textures.Water());
                tileSprite.setColor(sf::Color(255, 255, 255, 150));
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
