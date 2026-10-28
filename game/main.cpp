#include <SFML/Graphics.hpp>
#include "src/player/player.hpp"
#include "src/level/level.hpp"
#include "src/camera/camera.hpp"
#include "src/config.hpp"
#include "src/state-manager/state-manager.hpp"
#include "src/hud/hud.hpp"

constexpr sf::Vector2f START_POS = {800.f, 700.f};

HUD hud;
Level level;
Player character;

void Reset() {
    character.Reset(START_POS);
    StateManager::SetGameState(GameState::Play);
}

int main()
{
    sf::Vector2u windowSize(
        static_cast<unsigned int>(SCREEN_WIDTH),
        static_cast<unsigned int>(SCREEN_HEIGHT)
    );

    sf::RenderWindow window(sf::VideoMode(windowSize), "SFML Test");
    sf::Clock clock;
    
    character.setPosition(START_POS);

    Camera camera;

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
                    if (StateManager::GetGameState() == GameState::Play) {
                        StateManager::SetGameState(GameState::Pause);
                    }
                    else if (StateManager::GetGameState() == GameState::Pause) {
                        StateManager::SetGameState(GameState::Play);
                    } 
                }
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    const GameState s = StateManager::GetGameState();
                    if (s == GameState::GameOver || s == GameState::Win) {
                        Reset();
                    }
                }
            }
        }
        if (StateManager::GetGameState() == GameState::Play) {
            character.Update(deltaTime, level);
            camera.Update(character.getPosition(), level);
        }
        hud.Update(Player::GetOxygen(), StateManager::GetGameState());

        window.clear(BACKGROUND_COLOR);

        window.setView(camera.GetView());

        window.draw(level);
        window.draw(character);  
        hud.Draw(window);

        window.display();
    }

    return 0;
}