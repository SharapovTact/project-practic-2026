#include <SFML/Graphics.hpp>
#include "src/player/player.hpp"
#include "src/level/level.hpp"
#include "src/camera/camera.hpp"
#include "src/config.hpp"
#include "src/state-manager/state-manager.hpp"
#include "src/hud/hud.hpp"
#include "src/level/level-textures.hpp"
#include "src/menu/main-menu.hpp"
#include "src/audio/audio.hpp"

constexpr sf::Vector2f START_POS = {800.f, 700.f};

LevelTextures levelTextures;
MainMenu mainMenu(levelTextures);
HUD hud;
Level level(levelTextures);
Player character; //Переделать хитбокс персонажа, чтобы мог касаться ногой дна
bool debugDraw = false;

void Reset() {
    character.Reset(START_POS);
    StateManager::SetGameState(GameState::Menu);
}

int main()
{
    Reset();
    sf::Vector2u windowSize(
        static_cast<unsigned int>(SCREEN_WIDTH),
        static_cast<unsigned int>(SCREEN_HEIGHT)
    );

    sf::RenderWindow window(sf::VideoMode(windowSize), "SFML Test");
    sf::Clock clock;
    
    character.setPosition(START_POS);

    Camera camera;
    Audio audio;

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            } 
            if (StateManager::GetGameState() == GameState::Menu) {
                if (mainMenu.HandleEvent(*event, window)) {
                    StateManager::SetGameState(GameState::Play);
                }
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) 
            {
                if (keyPressed->code == sf::Keyboard::Key::F1) {
                    debugDraw = !debugDraw;
                }
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
        const GameState state = StateManager::GetGameState();
        if (state == GameState::Menu) {
            mainMenu.Update(window);
        }
        else if (state == GameState::Play) {
            character.Update(deltaTime, level);
            const PlayerState playerState = character.GetState();
            hud.UpdateView(deltaTime, playerState == PlayerState::UnderWater || playerState == PlayerState::Dive || playerState == PlayerState::Relax, character.GetDepth(level));
            camera.Update(character.getPosition(), level);
        }
        audio.Update(deltaTime, character, StateManager::GetGameState());
        hud.Update(Player::GetOxygen(), StateManager::GetGameState());

        window.clear(BACKGROUND_COLOR);

        if (StateManager::GetGameState() == GameState::Menu) {
            mainMenu.Draw(window);
        }
        else {
            window.setView(camera.GetView());

            window.draw(level);
            window.draw(character);
            if (debugDraw) {
                character.DrawDebug(window);  
            }
            hud.Draw(window);
        }

        window.display();
    }

    return 0;
}