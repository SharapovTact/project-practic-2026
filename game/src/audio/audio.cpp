#include "audio.hpp"
#include <algorithm>
#include <iostream>
#include <string>

namespace {
    constexpr const char* SOUNDS_DIR = "../../assets/sounds/";

    constexpr float AMBIENT_VOLUME        = 55.f;
    constexpr float AMBIENT_FADE_SPEED    = 3.0f;
    constexpr float LOOP_VOLUME           = 70.f;
    constexpr float BREATH_VOLUME         = 65.f;
    constexpr float ONESHOT_VOLUME        = 90.f;
    constexpr float RELAX_SWIM_ANIM_SPEED = 3.0f;
    constexpr float MOVEMENT_AUDIO_SPEED  = 0.7f;
    constexpr int   LOOP_VOLUME_DELAY      = 100;

    std::string PathTo(const char* name) { return std::string(SOUNDS_DIR) + name; }

    bool IsPaused(const sf::SoundSource& s) { return s.getStatus() == sf::SoundSource::Status::Paused; }
    bool IsStopped(const sf::SoundSource& s) { return s.getStatus() == sf::SoundSource::Status::Stopped; }
}

void Audio::Load(sf::SoundBuffer& buffer, const char* name)
{
    if (!buffer.loadFromFile(PathTo(name))) {
        std::cerr << "Ошибка: не удалось загрузить звук '" << PathTo(name) << "'\n";
    }
}

Audio::Audio(): m_loop(m_loopBuffers[0]), m_breath(m_breathBuffer), m_oneShot(m_diveBuffer)
{
    if (!m_ambUnderwater.openFromFile(PathTo("amb_underwater.ogg"))) {
        std::cerr << "Ошибка: не удалось открыть 'amb_underwater.ogg'\n";
    }
    if (!m_ambSurface.openFromFile(PathTo("amb_surface.ogg"))) {
        std::cerr << "Ошибка: не удалось открыть 'amb_surface.ogg'\n";
    }
    m_ambUnderwater.setLooping(true);
    m_ambSurface.setLooping(true);
    m_ambUnderwater.setVolume(0.f);
    m_ambSurface.setVolume(AMBIENT_VOLUME);

    Load(m_loopBuffers[static_cast<std::size_t>(Loop::Swim)],        "swim_loop.ogg");
    Load(m_loopBuffers[static_cast<std::size_t>(Loop::Hover)],       "hover_loop.ogg");
    Load(m_loopBuffers[static_cast<std::size_t>(Loop::Vertical)],    "vertical_loop.ogg");
    Load(m_loopBuffers[static_cast<std::size_t>(Loop::Idle)],        "idle_loop.ogg");
    Load(m_loopBuffers[static_cast<std::size_t>(Loop::SurfaceSwim)], "surface_swim_loop.ogg");
    Load(m_breathBuffer,  "relax_loop.ogg");
    Load(m_diveBuffer,    "dive.ogg");
    Load(m_surfaceBuffer, "surface.ogg");

    m_loop.setLooping(true);
    m_loop.setVolume(LOOP_VOLUME);
    m_breath.setLooping(true);
    m_breath.setVolume(BREATH_VOLUME);
}

void Audio::SetLoop(Loop loop, float pitch)
{
    if (loop != m_currentLoop && m_loopVolumeDelayTicks == 0) {
        m_loop.stop();
        m_loopVolumeDelayTicks = LOOP_VOLUME_DELAY;
        m_currentLoop = loop;
        if (loop != Loop::None) {
            m_loop.setBuffer(m_loopBuffers[static_cast<std::size_t>(loop)]);
            m_loop.play();
        }
    }
    m_loop.setPitch(pitch);
}

void Audio::SetBreathing(bool enabled)
{
    if (enabled && IsStopped(m_breath)) {
        m_breath.play();
    } else if (!enabled && !IsStopped(m_breath)) {
        m_breath.stop();
    }
}

void Audio::PlayOneShot(const sf::SoundBuffer& buffer, float volume)
{
    m_oneShot.stop();
    m_oneShot.setBuffer(buffer);
    m_oneShot.setVolume(volume);
    m_oneShot.play();
}

void Audio::PauseAll()
{
    m_ambUnderwater.pause();
    m_ambSurface.pause();
    m_loop.pause();
    m_breath.pause();
    m_oneShot.pause();
}

void Audio::ResumeAll()
{
    m_ambUnderwater.play();
    m_ambSurface.play();
    if (IsPaused(m_loop))    m_loop.play();
    if (IsPaused(m_breath))  m_breath.play();
    if (IsPaused(m_oneShot)) m_oneShot.play();
}

void Audio::UpdateAmbient(float deltaTime, bool underwater)
{
    const float target = underwater ? 1.f : 0.f;
    const float k = std::min(1.f, AMBIENT_FADE_SPEED * deltaTime);
    m_underwaterMix += (target - m_underwaterMix) * k;
    m_ambUnderwater.setVolume(AMBIENT_VOLUME * m_underwaterMix);
    m_ambSurface.setVolume(AMBIENT_VOLUME * (1.f - m_underwaterMix));
}

void Audio::Update(float deltaTime, const Player& player, GameState gameState)
{
    if (!m_started) {
        m_ambUnderwater.play();
        m_ambSurface.play();
        m_started = true;
    }

    if (m_loopVolumeDelayTicks > 0) {
        m_loopVolumeDelayTicks -= (int)deltaTime;
    }

    if (gameState == GameState::Pause) {
        if (!m_paused) {
            PauseAll();
            m_paused = true;
        }
        return;
    }
    if (m_paused) {
        ResumeAll();
        m_paused = false;
    }

    if (gameState != GameState::Play) {
        SetLoop(Loop::None, 1.f);
        SetBreathing(false);
        UpdateAmbient(deltaTime, gameState != GameState::Menu && m_underwaterMix > 0.5f);
        m_prevState = player.GetState();
        return;
    }

    const PlayerState state = player.GetState();
    const diver::Anim anim  = player.GetAnim();
    const float animSpeed   = player.GetAnimSpeed();

    if (state == PlayerState::Dive && m_prevState != PlayerState::Dive) {
        PlayOneShot(m_diveBuffer, ONESHOT_VOLUME);
    }
    if (state == PlayerState::Surface && m_prevState == PlayerState::UnderWater) {
        PlayOneShot(m_surfaceBuffer, ONESHOT_VOLUME);
    }
    m_prevState = state;

    switch (anim) {
        case diver::Anim::Swim: SetLoop(Loop::Swim, MOVEMENT_AUDIO_SPEED); break;
        case diver::Anim::Hover: SetLoop(Loop::Hover, 1.f); break;
        case diver::Anim::VerticalUp: SetLoop(Loop::Vertical, MOVEMENT_AUDIO_SPEED); break;
        case diver::Anim::VerticalDown: SetLoop(Loop::Vertical, MOVEMENT_AUDIO_SPEED); break;
        case diver::Anim::Idle: SetLoop(Loop::Idle, 1.f); break;
        case diver::Anim::Relax:
            if (animSpeed > 1.f) {
                SetLoop(Loop::SurfaceSwim, MOVEMENT_AUDIO_SPEED);
            } else {
                SetLoop(Loop::None, 1.f);
            }
            break;
        case diver::Anim::Dive: SetLoop(Loop::None, 1.f); break;
    }

    SetBreathing(state == PlayerState::Relax);
    UpdateAmbient(deltaTime, state == PlayerState::UnderWater);
}
