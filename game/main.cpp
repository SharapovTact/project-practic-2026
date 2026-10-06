#include <SFML/Graphics.hpp>
#include "src/player/player.hpp"
#include "src/player/enemy.hpp"
#include "src/config.hpp"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1600, 900}),
        "SFML Test"
    );

    sf::Clock clock;
    sf::Color bgColor = BACKGROUND_COLOR;
    float deltaTime = 0;
    Player character;
    character.setPosition({400.f, 350.f});

    Enemy enemy;
    enemy.setPosition({500.f, 500.f});


    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            } 
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) 
            {
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
            }
        }

        character.Update(deltaTime);
        enemy.Update(deltaTime);
        window.clear(bgColor);
        
        window.draw(character);
        window.draw(enemy);
        window.display();
        
    }

    return 0;
}