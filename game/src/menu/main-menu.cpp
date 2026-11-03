#include "main-menu.hpp"
#include <iostream>

constexpr const char*  FONT_PATH = "../../assets/fonts/arial.ttf";
constexpr const char*  GAME_TITLE = "DEEP DIVE";

constexpr unsigned int TITLE_FONT_SIZE = 96;
constexpr unsigned int BUTTON_FONT_SIZE = 40;
constexpr float BUTTON_WIDTH = 320.f;
constexpr float BUTTON_HEIGHT = 80.f;
constexpr float BUTTON_OUTLINE = 3.f;

const sf::Color TITLE_COLOR(255, 255, 255);
const sf::Color BUTTON_COLOR(30, 110, 170);
const sf::Color BUTTON_HOVER_COLOR(50, 150, 210);
const sf::Color BUTTON_DISABLED_COLOR(90, 90, 90);
const sf::Color BUTTON_OUTLINE_COLOR(255, 255, 255);

MainMenu::MainMenu(LevelTextures& textures)
    : m_textures(textures),
      m_loadAttempted(false),
      m_buttonHovered(false),
      m_titleText(m_font, GAME_TITLE, TITLE_FONT_SIZE),
      m_buttonText(m_font, "Play", BUTTON_FONT_SIZE)
{
    if (!m_font.openFromFile(FONT_PATH)) {
        std::cerr << "Error: failed to load font '" << FONT_PATH << "'\n";
    }

    m_titleText.setFillColor(TITLE_COLOR);
    m_buttonText.setFillColor(sf::Color::White);

    m_button.setSize({BUTTON_WIDTH, BUTTON_HEIGHT});
    m_button.setOutlineThickness(BUTTON_OUTLINE);
    m_button.setOutlineColor(BUTTON_OUTLINE_COLOR);
    m_button.setOrigin({BUTTON_WIDTH / 2.f, BUTTON_HEIGHT / 2.f});
}

void MainMenu::Update(const sf::RenderWindow& window)
{
    if (!m_loadAttempted) {
        m_loadAttempted = true;
        m_textures.Load();
    }

    Layout(window.getSize());

    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window), m_view);
    m_buttonHovered = CanStart() && m_button.getGlobalBounds().contains(mouse);
}

bool MainMenu::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (!CanStart()) {
        return false;
    }

    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            const sf::Vector2f mouse = window.mapPixelToCoords(mousePressed->position, m_view);
            return m_button.getGlobalBounds().contains(mouse);
        }
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        return keyPressed->code == sf::Keyboard::Key::Enter;
    }

    return false;
}

void MainMenu::Draw(sf::RenderWindow& window)
{
    Layout(window.getSize());
    window.setView(m_view);

    window.draw(m_titleText);
    if (!CanStart()) {
        m_button.setFillColor(BUTTON_DISABLED_COLOR);
    }
    else if(m_buttonHovered) {
        m_button.setFillColor(BUTTON_HOVER_COLOR);
    }
    else {
        m_button.setFillColor(BUTTON_COLOR);
    }
    window.draw(m_button);
    window.draw(m_buttonText);
}

void MainMenu::Layout(const sf::Vector2u& windowSize)
{
    const float w = static_cast<float>(windowSize.x);
    const float h = static_cast<float>(windowSize.y);

    m_view = sf::View(sf::FloatRect({0.f, 0.f}, {w, h}));

    CenterText(m_titleText, w / 2.f, h * 0.28f);

    m_button.setPosition({w / 2.f, h * 0.55f});
    CenterText(m_buttonText, w / 2.f, h * 0.55f);
}

void MainMenu::CenterText(sf::Text& text, float centerX, float centerY)
{
    const sf::FloatRect b = text.getLocalBounds();
    text.setOrigin({b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f});
    text.setPosition({centerX, centerY});
}