#pragma once

#include "timer.h"
class Animation
{
  private:
    Timer m_timer;
    int m_frameCount;

  public:
    Animation() : m_timer(0), m_frameCount(0) {}
    Animation(int frameCount, float length)
        : m_timer(length), m_frameCount(frameCount)
    {
    }

    [[nodiscard]] float getLength() const { return m_timer.getLength(); }
    [[nodiscard]] int currentFrame() const
    {
        return static_cast<int>(m_timer.getTime() / m_timer.getLength() *
                                m_frameCount);
    }

    void step(float deltaTime) { m_timer.step(deltaTime); }
    bool isDone() const { return m_timer.isTimeout(); }
};
