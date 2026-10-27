#include "level.hpp"
#include "../config.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>

Level::Level(const std::string& mapFilePath)
    : m_bgSprite(m_bgTexture), m_width(0), m_height(0), m_tileSize(64.f)
{
    if (!m_surfaceTexture.loadFromFile("../../assets/level/surface.png")) {
        std::cerr << "Error: failed to load surface texture\n";
    }
    if (!m_waterTexture.loadFromFile("../../assets/level/water.png")) {
        std::cerr << "Error: failed to load water texture\n";
    }
    if (!m_groundTexture.loadFromFile("../../assets/level/ground.png")) {
        std::cerr << "Error: failed to load ground texture\n";
    }
    
    if (!m_bgTexture.loadFromFile("../../assets/level/background.jpeg")) {
    std::cout << "Error: failed to load background\n";
    } else {
        m_bgSprite = sf::Sprite(m_bgTexture);
    }

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
        std::cerr << "Error: failed to load map file: " << mapFilePath << "\n";
        levelDesign = {"GGGG", "~~~~", "####"};
    }

    int mapHeight = static_cast<int>(levelDesign.size());
    int mapWidth = (mapHeight > 0) ? static_cast<int>(levelDesign[0].size()) : 0;

    Resize(mapWidth, mapHeight);

    if (m_bgTexture.getSize().x > 0 && m_bgTexture.getSize().y > 0) {
        float totalWidth = m_width * m_tileSize;
        float totalHeight = m_height * m_tileSize;
        
        float scaleX = totalWidth / static_cast<float>(m_bgTexture.getSize().x);
        float scaleY = totalHeight / static_cast<float>(m_bgTexture.getSize().y);
        
        m_bgSprite.setScale({scaleX, scaleY});
        m_bgSprite.setPosition({0.f, 0.f});
    }

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
    target.draw(m_bgSprite, states);

    sf::Sprite tileSprite(m_surfaceTexture);

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            TileType type = GetTile(x, y);

            if (type == TileType::Surface) {
                tileSprite.setTexture(m_surfaceTexture);
                tileSprite.setColor(sf::Color(255, 255, 255, 255));
            } else if (type == TileType::Ground) {
                tileSprite.setTexture(m_groundTexture);
                tileSprite.setColor(sf::Color(255, 255, 255, 255));
            } else {
                tileSprite.setTexture(m_waterTexture);
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