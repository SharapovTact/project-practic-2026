#include <SFML/Graphics.hpp>
#include "src/player/player.hpp"
#include "src/background/background.hpp"
#include "src/config.hpp"

int main()
{
    sf::Vector2u windowSize(
        static_cast<unsigned int>(SCREEN_WIDTH),
        static_cast<unsigned int>(SCREEN_HEIGHT)
    );

    sf::RenderWindow window(sf::VideoMode(windowSize), "SFML Test");

    sf::Clock clock;
    Background background;
    Player character;
    character.setPosition({800.f, 350.f});

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        
        while (const std::optional<sf::Event> event = window.pollEvent())
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

        character.Update(deltaTime, background);

        window.clear(BACKGROUND_COLOR);
        
        window.draw(background); 
        window.draw(character);  

        window.display();
    }

    return 0;
}