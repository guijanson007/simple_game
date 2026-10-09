#pragma once

#include "SDL_API.h"

#include <utility>

struct RectF
{
    float x;
    float y;
    float w;
    float h;

    RectF(const SDL_FRect &rect)
        : x(static_cast<float>(rect.x)),
          y(static_cast<float>(rect.y)),
          w(static_cast<float>(rect.w)),
          h(static_cast<float>(rect.h))
    {
    }

    RectF(const RectF &) = default;
    RectF(RectF &&) = default;

    RectF &operator=(const SDL_FRect &rect)
    {
        x = static_cast<float>(rect.x);
        y = static_cast<float>(rect.y);
        w = static_cast<float>(rect.w);
        h = static_cast<float>(rect.h);
        return *this;
    }

    RectF &operator=(const RectF &) = default;
    RectF &operator=(RectF &&) = default;

    RectF(float x, float y, float w, float h)
        : x(x),
          y(y),
          w(w),
          h(h)
    {
    }

    RectF()
        : x(0.0f),
          y(0.0f),
          w(0.0f),
          h(0.0f)
    {
    }

    SDL_FRect toSDLFRect() const { return SDL_FRect{x, y, w, h}; }
};

enum class EventType
{
    KeyDown,
    KeyUp,
    Quit,
    WindowResized,
    NoEvent
};

struct Event
{
    EventType type;
    union
    {
        struct
        {
            SDL_Scancode scancode;
        } key;
        std::pair<int, int> window;
    };

    Event()
        : type(EventType::NoEvent),
          window{0, 0}
    {
    }

    Event &operator=(const Event &other)
    {
        type = other.type;
        if (type == EventType::KeyDown || type == EventType::KeyUp)
        {
            key.scancode = other.key.scancode;
        }
        else if (type == EventType::WindowResized)
        {
            window = other.window;
        }
        return *this;
    }
};

enum class TextureId
{
    PlayerIdle,
    PlayerRun,
    Brick,
    Grass,
    Ground,
    Panel,
    PlayerSlide,
    Bg1,
    Bg2,
    Bg3,
    Bg4,
    Bullet,
    BulletHit,
    PlayerShoot,
    PlayerRunShoot,
    PlayerSlideShoot,
    Enemy,
    EnemyHit,
    EnemyDie
};

enum class Key
{
    Left,
    Right,
    Jump,
    Shoot,
    ToggleFullscreen,
    Escape
};

struct InputState
{
    bool left = false;
    bool right = false;
    bool jump = false;
    bool shoot = false;
};

class Platform
{
  public:
    Platform() = default;
    virtual ~Platform() = default;
    virtual void pollEvents(InputState &input) = 0;
    virtual float deltaTime() = 0;
    virtual bool shouldQuit() const = 0;
    virtual RectF getRectIntersection(const RectF &a, const RectF &b) = 0;
};
