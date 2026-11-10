#pragma once
#include <SFML/Audio.hpp>
#include <array>
#include <cstddef>
#include "../player/player.hpp"
#include "../state-manager/state-manager.hpp"

class Audio
{
public:
    Audio();
    void Update(float deltaTime, const Player& player, GameState gameState);

private:
    enum class Loop { None, Swim, Hover, Vertical, Idle, SurfaceSwim, Count };

    void SetLoop(Loop loop, float pitch);
    void SetBreathing(bool enabled);
    void PlayOneShot(const sf::SoundBuffer& buffer, float volume);
    void PauseAll();
    void ResumeAll();
    void UpdateAmbient(float deltaTime, bool underwater);

    static void Load(sf::SoundBuffer& buffer, const char* name);

    sf::Music m_ambUnderwater;
    sf::Music m_ambSurface;

    std::array<sf::SoundBuffer, static_cast<std::size_t>(Loop::Count)> m_loopBuffers;
    sf::SoundBuffer m_breathBuffer;
    sf::SoundBuffer m_diveBuffer;
    sf::SoundBuffer m_surfaceBuffer;

    sf::Sound m_loop;
    sf::Sound m_breath;
    sf::Sound m_oneShot;

    Loop  m_currentLoop = Loop::None;
    float m_underwaterMix = 0.f;
    bool  m_paused = false;
    bool  m_started = false;
    PlayerState m_prevState = PlayerState::UnderWater;
};
