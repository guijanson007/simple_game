#pragma once

class Timer
{
  private:
    float m_length, m_time;
    bool m_timeout;

  public:
    Timer(float length) : m_length(length), m_time(0), m_timeout(false) {}

    bool step(float deltaTime)
    {
        m_time += deltaTime;
        if (m_time >= m_length)
        {
            m_time -= m_length;
            m_timeout = true;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool isTimeout() const { return m_timeout; }
    [[nodiscard]] float getTime() const { return m_time; }
    [[nodiscard]] float getLength() const { return m_length; }
    void reset() { m_time = 0, m_timeout = false; }
};
