#pragma once
#include <SFML/Graphics.hpp>
#include <string>

constexpr float SCREEN_HEIGHT = 900.f;
constexpr float SCREEN_WIDTH = 1600.f;
constexpr float SURFACE_PROPORTION = 0.2f;

constexpr sf::Color BACKGROUND_COLOR = sf::Color(0, 76, 138);

inline sf::String Utf8(const std::string& s)
{
    return sf::String::fromUtf8(s.begin(), s.end());
}