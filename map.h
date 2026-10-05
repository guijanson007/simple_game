#pragma once

#include "SDL_API.h"
#include "gameState.h"
#include "resources.h"

#include <memory>

namespace Game
{
constexpr int MAP_ROWS = 5;
constexpr int MAP_COLS = 50;
class Map
{
  public:
    Map(const int width, const int height, const int tileSize = 32);
    ~Map() = default;

    void renderMap(std::shared_ptr<GameState> gs,
                   std::shared_ptr<Resources> res);

  private:
    int mTileSize;
    int mWidth;
    int mHeight;
    short mMap[MAP_ROWS][MAP_COLS]; // 2D array of tile indices
    short mForegroundMap[MAP_ROWS]
                        [MAP_COLS]; // 2D array of foreground tile indices
    short mBackgroundMap[MAP_ROWS]
                        [MAP_COLS]; // 2D array of background tile indices
    GameObject createObject(int row, int col, SDL_Texture *tex,
                            ObjectType type);
};
} // namespace Game
