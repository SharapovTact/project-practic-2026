#pragma once
#include <SFML/Graphics.hpp>
#include <array>

namespace diver {

constexpr int kCell = 128;

enum class Anim {
    Swim,
    Hover,
    VerticalUp,
    VerticalDown,
    Idle,
    Dive,
    Relax
};

struct AnimDef {
    int row;
    int frames;
    bool loop;
    sf::Vector2f origin; 
    int waterline; 
    std::array<int, 12> ms;  
};

inline constexpr std::array<AnimDef, 7> kAnims = {{
    /* Swim         */ { 0,  8, true , {57.f, 63.f},  -1, {80, 80, 80, 80, 80, 80, 80, 80, 0, 0, 0, 0} },
    /* Hover        */ { 1,  8, true , {57.f, 63.f},  -1, {140, 140, 140, 140, 140, 140, 140, 140, 0, 0, 0, 0} },
    /* VerticalUp   */ { 2,  8, true , {67.f, 73.f},  -1, {90, 90, 90, 90, 90, 90, 90, 90, 0, 0, 0, 0} },
    /* VerticalDown */ { 3,  8, true , {61.f, 55.f},  -1, {90, 90, 90, 90, 90, 90, 90, 90, 0, 0, 0, 0} },
    /* Idle         */ { 4,  8, true , {65.f, 69.f},  44, {150, 150, 150, 150, 150, 150, 150, 150, 0, 0, 0, 0} },
    /* Dive         */ { 5, 12, false, {57.f, 63.f},  58, {140, 90, 90, 160, 90, 90, 110, 90, 90, 100, 90, 90} },
    /* Relax        */ { 6,  8, true , {57.f, 63.f},  58, {180, 180, 180, 180, 180, 180, 180, 180, 0, 0, 0, 0} },
}};

inline constexpr std::array<float, 12> kDiveRootDy = { 0.f, -2.f, -4.f, -6.f, -6.f, -4.f, 2.f, 10.f, 20.f, 32.f, 48.f, 64.f };

inline const AnimDef& def(Anim a) { return kAnims[static_cast<std::size_t>(a)]; }

inline float hipYOnSurface(Anim a, float surfaceY) {
    const AnimDef& d = def(a);
    return surfaceY + (d.origin.y - static_cast<float>(d.waterline));
}

class Animator {
public:
    explicit Animator(sf::Sprite& sprite) : m_sprite(sprite) { play(Anim::Hover, true); }

    void play(Anim a, bool restart = false) {
        if (a == m_anim && !restart) return;
        m_anim = a; m_frame = 0; m_timeMs = 0.f; m_finished = false; m_speed = 1.f;
        apply();
    }

    void update(sf::Time dt) {
        const AnimDef& d = def(m_anim);
        if (m_finished) return;
        m_timeMs += dt.asSeconds() * 1000.f * m_speed;
        while (m_timeMs >= static_cast<float>(d.ms[m_frame])) {
            m_timeMs -= static_cast<float>(d.ms[m_frame]);
            if (m_frame + 1 < d.frames) ++m_frame;
            else if (d.loop) m_frame = 0;
            else { m_finished = true; m_timeMs = 0.f; break; }
        }
        apply();
    }

    sf::Vector2f rootOffset() const {
        return m_anim == Anim::Dive ? sf::Vector2f{0.f, kDiveRootDy[m_frame]} : sf::Vector2f{};
    }

    void setSpeed(float speed) { m_speed = speed; }

    bool finished() const { return m_finished; }
    Anim current() const { return m_anim; }
    int frame() const { return m_frame; }

private:
    void apply() {
        const AnimDef& d = def(m_anim);
        m_sprite.setTextureRect(sf::IntRect({m_frame * kCell, d.row * kCell}, {kCell, kCell}));
        m_sprite.setOrigin(d.origin);
    }

    sf::Sprite& m_sprite;
    Anim m_anim = Anim::Idle;
    int m_frame = 0;
    float m_timeMs = 0.f;
    bool m_finished = false;
    float m_speed = 1.f;
};

}

