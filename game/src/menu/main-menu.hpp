#pragma once
#include <SFML/Graphics.hpp>
#include "../level/level-textures.hpp"

class MainMenu
{
public:
    explicit MainMenu(LevelTextures& textures);

    void Update(const sf::RenderWindow& window);
    bool HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void Draw(sf::RenderWindow& window);

private:
    void Layout(const sf::Vector2u& windowSize);
    void CenterText(sf::Text& text, float centerX, float centerY);
    bool CanStart() const { return m_textures.IsLoaded(); }

    LevelTextures& m_textures;
    bool m_loadAttempted;
    bool m_buttonHovered;

    sf::View m_view;
    sf::Font m_font;
    sf::Text m_titleText;
    sf::Text m_buttonText;
    sf::RectangleShape m_button;
};