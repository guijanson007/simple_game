#pragma once

#include "SDL.h"
#include "gameObject.h"
#include "gameState.h"
#include "map.h"
#include "platform.h"
#include "resources.h"
#include <memory>

namespace Game
{
constexpr int TILE_SIZE = 32;
constexpr int SPRITE_SIZE = 32;
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 320;

class Game
{
  public:
    Game()
        : mGameState(std::make_shared<GameState>()),
          mResources(std::make_shared<Resources>()),
          mMap(SCREEN_WIDTH, SCREEN_HEIGHT, TILE_SIZE),
          mSDL(mResources.get(), mGameState.get())
    {
        mGameState->initializeViewPort(SCREEN_WIDTH, SCREEN_HEIGHT);
        mMap.renderMap(mGameState, mResources);
    }

    ~Game() = default;

    void start();
    void pushLayer(const std::vector<GameObject> &layer, size_t layerIndex);

  private:
    std::shared_ptr<GameState> mGameState;
    std::shared_ptr<Resources> mResources;
    std::vector<GameObject> mGameObjects;
    Map mMap;
    SDL::SDL mSDL;

    void iterate();
    void update(GameObject &obj, float deltaTime);
    void handleEvent(const Event &event);
    void handleKeyInput(SDL_Scancode key, bool keyDown, GameObject &obj);
    void collisionResponse(const RectF &rectA, const RectF &rectB, RectF &rectC,
                           GameObject &objA, GameObject &objB, float deltaTime);
    void checkCollision(GameObject &a, GameObject &b, float deltaTime);
};
} // namespace Game
