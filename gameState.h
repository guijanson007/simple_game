#pragma once

#include "gameObject.h"

#include <array>
#include <vector>

namespace Game
{
constexpr size_t LAYER_IDX_LEVEL = 0;
constexpr size_t LAYER_IDX_CHARACTER = 1;
struct GameState
{

    std::array<std::vector<GameObject>, 2> layers;
    std::vector<GameObject> backgroundTiles;
    std::vector<GameObject> foregroundTiles;
    std::vector<GameObject> bullets;
    float bg2Scroll, bg3Scroll, bg4Scroll;
    int playerIndex;
    SDL_FRect mapViewPort;

    bool debugMode;
    GameState()
    {
        bg2Scroll = bg3Scroll = bg4Scroll = 0.0f;
        playerIndex = -1;
        debugMode = false;
    }

    void initializeViewPort(float width, float height)
    {
        mapViewPort = {.x = 0, .y = 0, .w = width, .h = height};
    }
    GameObject &player() { return layers[LAYER_IDX_CHARACTER][playerIndex]; }
};
} // namespace Game
