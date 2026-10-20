#pragma once

#include "animation.h"
#include "platform.h"
#include <glm/glm.hpp>
#include <variant>
#include <vector>

//  boilerplate for overloaded lambdas, see
//  https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};

// explicit deduction guide (Required for C++17, optional but harmless in C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace Game
{
enum class PlayerState
{
    idle,
    running,
    jumping
};

enum class BulletState
{
    moving,
    colliding,
    inactive
};

enum class EnemyState
{
    shambling,
    damaged,
    dead
};

struct PlayerData
{
    PlayerState state;
    Timer weaponTimer;
    PlayerData()
        : weaponTimer(0.1f)
    {
        state = PlayerState::idle;
    }
};

struct LevelData
{
};

struct EnemyData
{
    EnemyState state;
    Timer damagedTimer;
    int healthPoints;
    EnemyData()
        : state{EnemyState::shambling},
          damagedTimer{0.5f},
          healthPoints{100}
    {
    }
};

struct BulletData
{
    BulletState state;

    BulletData()
        : state(BulletState::moving)
    {
    }
};

using ObjectData = std::variant<PlayerData, LevelData, EnemyData, BulletData>;

struct GameObject
{
    ObjectData data;
    glm::vec2 position, velocity, acceleration;
    float direction;
    float maxSpeedX;
    std::vector<Animation> animations;
    int currentAnimation;
    TextureId texture;
    bool dynamic = false;
    bool grounded = false;
    RectF collider;
    Timer flashTimer;
    bool shouldFlash = false;
    int spriteFrame;

    GameObject()
        : data{LevelData()},
          collider{},
          flashTimer{0.05f}
    {
        direction = 1;
        maxSpeedX = 0;
        position = velocity = acceleration = glm::vec2(0);
        currentAnimation = -1;
        dynamic = false;
        grounded = false;
        shouldFlash = false;
        spriteFrame = 1;
    }
};
} // namespace Game
