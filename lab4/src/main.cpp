#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "SFML Test"
    );

    sf::Color bgColor = sf::Color::Green;

    while (window.isOpen())
    {
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
                if (keyPressed->code == sf::Keyboard::Key::Space) 
                {
                    if (bgColor == sf::Color::Green)
                    {
                        bgColor = sf::Color::Blue;
                    }
                    else
                    {
                        bgColor = sf::Color::Green;
                    }
                }
            }
        }

        window.clear(bgColor);
        window.display();
    }

    return 0;
}