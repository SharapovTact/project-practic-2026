#include "hud.hpp"
#include "../config.hpp"
#include <iostream>
#include <string>
#include <algorithm>    
#include <cstdint>

namespace {
    constexpr const char*  FONT_PATH = "../../assets/fonts/arial.ttf";
    constexpr unsigned int OXYGEN_FONT_SIZE = 24;
    constexpr unsigned int STATE_FONT_SIZE = 64;
    constexpr float MARGIN = 20.f;
    constexpr float DEPTH_LIGHT_FALLOFF = 15.f;
    const sf::Color DEEP_TINT = sf::Color(0, 6, 16, 200);

    constexpr float VIGNETTE_START      = 50.f;
    constexpr float VIGNETTE_MAX_RADIUS = 1.1f;
    constexpr float VIGNETTE_MIN_RADIUS = 0.12f;
    constexpr float VIGNETTE_SOFTNESS   = 0.25f;
    constexpr float PULSE_START         = 20.f;
    constexpr const char* VIGNETTE_SHADER = R"(
        uniform vec2  resolution;
        uniform float radius;
        uniform float softness;

        void main()
        {
            vec2 p = (gl_FragCoord.xy - 0.5 * resolution) / resolution.y;
            float a = smoothstep(radius, radius + softness, length(p));
            gl_FragColor = vec4(0.0, 0.0, 0.0, a);
        }
    )";

    constexpr const char* MASK_PATH = "../../assets/hud/mask.png";
    constexpr float VIEW_FADE_SPEED = 2.f;
    const sf::Color SURFACE_TINT    = sf::Color(0, 0, 0, 25);
    const sf::Color UNDERWATER_TINT = sf::Color(0, 28, 60, 70);
    sf::Color LerpColor(sf::Color a, sf::Color b, float t)
    {
        auto mix = [t](std::uint8_t x, std::uint8_t y) {
            return static_cast<std::uint8_t>(x + (y - x) * t);
        };
        return sf::Color(mix(a.r, b.r), mix(a.g, b.g), mix(a.b, b.b), mix(a.a, b.a));
    }
}

HUD::HUD(): 
    m_oxygenText(m_font, "", OXYGEN_FONT_SIZE), 
    m_depthText (m_font, "", OXYGEN_FONT_SIZE),
    m_stateText (m_font, "", STATE_FONT_SIZE), 
    m_currentState(GameState::Play),
    m_maskSprite(m_maskTexture)
{
    if (!m_font.openFromFile(FONT_PATH)) { 
        std::cerr << "Ошибка: не удалось загрузить шрифт '" << FONT_PATH << "'\n";
    }
    m_maskLoaded = m_maskTexture.loadFromFile(MASK_PATH);
    if (!m_maskLoaded) {
        std::cerr << "Ошибка: не удалось загрузить маску '" << MASK_PATH << "'\n";
    }
    m_maskSprite.setTexture(m_maskTexture, true);   

     m_depthText.setFillColor(sf::Color::White);
    m_depthText.setPosition({MARGIN, MARGIN + OXYGEN_FONT_SIZE + 10.f});

    m_oxygenText.setFillColor(sf::Color::White);
    m_oxygenText.setPosition({MARGIN, MARGIN});

    m_stateText.setFillColor(sf::Color::White);

    m_dimOverlay.setFillColor(sf::Color(0, 0, 0, 170));

    if (sf::Shader::isAvailable()) {
        m_vignetteReady = m_vignette.loadFromMemory(VIGNETTE_SHADER, sf::Shader::Type::Fragment);
    }
}

void HUD::Update(float oxygenPercent, GameState state)
{
    m_currentState = state;
    m_oxygen = oxygenPercent;
    m_oxygenText.setString(
        Utf8("Кислород: " + std::to_string(static_cast<int>(oxygenPercent)) + "%")
    );
}

void HUD::UpdateView(float deltaTime, bool underwater, float depthMeters)
{
    const float target = underwater ? 1.f : 0.f;
    m_underwaterMix += (target - m_underwaterMix) * std::min(1.f, VIEW_FADE_SPEED * deltaTime);

    m_depth = depthMeters;
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%.1f", m_depth);
    m_depthText.setString(Utf8("Глубина: " + std::string(buf) + " м"));
}

void HUD::Draw(sf::RenderWindow& window)
{
    if (m_currentState == GameState::Menu) {
        return;
    }

    const sf::Vector2u size = window.getSize();
    window.setView(sf::View(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(size.x), static_cast<float>(size.y)}
    )));

    const sf::Vector2f screenSize(static_cast<float>(size.x), static_cast<float>(size.y));
    m_tint.setSize(screenSize);
    const float darkness = 1.f - std::exp(-m_depth / DEPTH_LIGHT_FALLOFF);
    const sf::Color waterTint = LerpColor(SURFACE_TINT, UNDERWATER_TINT, m_underwaterMix);
    m_tint.setFillColor(LerpColor(waterTint, DEEP_TINT, darkness * m_underwaterMix));
    window.draw(m_tint);
    if (m_maskLoaded) {
        const sf::Vector2u ts = m_maskTexture.getSize();
        m_maskSprite.setScale({screenSize.x / ts.x, screenSize.y / ts.y});
        window.draw(m_maskSprite);
    }

    if (m_vignetteReady && m_oxygen < VIGNETTE_START) {
        const sf::Vector2f screen(static_cast<float>(size.x), static_cast<float>(size.y));

        float t = 1.f - m_oxygen / VIGNETTE_START;
        float radius = VIGNETTE_MAX_RADIUS + (VIGNETTE_MIN_RADIUS - VIGNETTE_MAX_RADIUS) * t;

        if (m_oxygen < PULSE_START) {
            radius += 0.03f * std::sin(m_clock.getElapsedTime().asSeconds() * 7.f);
        }

        m_vignette.setUniform("resolution", screen);
        m_vignette.setUniform("radius", radius);
        m_vignette.setUniform("softness", VIGNETTE_SOFTNESS);

        sf::RectangleShape overlay(screen);
        window.draw(overlay, &m_vignette);
    }

    window.draw(m_oxygenText);
    window.draw(m_depthText);

    if (m_currentState != GameState::Play) {
        DrawOverlay(window);
    }
}

void HUD::DrawOverlay(sf::RenderWindow& window)
{
    const sf::Vector2u size = window.getSize();

    m_dimOverlay.setSize({static_cast<float>(size.x), static_cast<float>(size.y)});
    m_dimOverlay.setPosition({0.f, 0.f});
    window.draw(m_dimOverlay);

    std::string text;
    switch (m_currentState) {
        case GameState::Menu:
            text = "Главное меню";
            break;
        case GameState::Pause:    
            text = "Пауза";    
            break;
        case GameState::GameOver: 
            text = "Утонул\nНажми пробел"; 
            break;
        case GameState::Win:      
            text = "Ты победил";   
            break;
    }
    m_stateText.setString(Utf8(text));

    const sf::FloatRect b = m_stateText.getLocalBounds();
    m_stateText.setOrigin({
        b.position.x + b.size.x / 2.f,
        b.position.y + b.size.y / 2.f
    });
    m_stateText.setPosition({
        static_cast<float>(size.x) / 2.f,
        static_cast<float>(size.y) / 2.f
    });
    window.draw(m_stateText);
}