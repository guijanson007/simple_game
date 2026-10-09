#pragma once

#include "SDL_API.h"

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

    RectF operator=(const SDL_FRect &rect)
    {
        x = static_cast<float>(rect.x);
        y = static_cast<float>(rect.y);
        w = static_cast<float>(rect.w);
        h = static_cast<float>(rect.h);
        return *this;
    }

    RectF operator=(const RectF &rect)
    {
        x = rect.x;
        y = rect.y;
        w = rect.w;
        h = rect.h;
        return *this;
    }

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
