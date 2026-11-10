#include "level-textures.hpp"
#include <iostream>
#include <string>

namespace {
    const std::string TEXTURES_DIR = "../../assets/level/";

    bool LoadRequired(sf::Texture& texture, const std::string& fileName)
    {
        const std::string path = TEXTURES_DIR + fileName;
        if (!texture.loadFromFile(path)) {
            std::cerr << "Ошибка: не удалось загрузить текстуру '" << path << "'\n";
            return false;
        }
        return true;
    }
}

bool LevelTextures::Load()
{
    if (m_loaded) {
        return true;
    }

    bool ok = true;
    ok = LoadRequired(m_surface, "surface.png") && ok;
    ok = LoadRequired(m_water,   "water.png") && ok;
    ok = LoadRequired(m_ground,  "ground.png") && ok;

    m_hasBackground = m_background.loadFromFile(TEXTURES_DIR + "background.jpeg");
    if (!m_hasBackground) {
        std::cout << "Примечание: необязательный фон '" << TEXTURES_DIR
                  << "background.jpeg' не найден, уровень будет нарисован без него\n";
    }

    m_loaded = ok;
    return m_loaded;
}
