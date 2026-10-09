#include "map.h"

namespace Game
{
Map::Map(const int width, const int height, const int tileSize)
    : mWidth(width),
      mHeight(height),
      mTileSize(tileSize)
{
    /*
     * 1 - Ground
     * 2 - Panel
     * 3 - Enemy
     * 4 - Player
     * 5 - Grass
     * 6 - Brick
     */

    // clang-format off
    short tempMap[MAP_ROWS][MAP_COLS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0,
        0, 0, 4, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    short tempForegroundMap[MAP_ROWS][MAP_COLS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        5, 0, 0, 5, 5, 5, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    short tempBackgroundMap[MAP_ROWS][MAP_COLS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 6, 0, 6, 6, 6, 6, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    // clang-format on
    memcpy(mMap, tempMap, sizeof(tempMap));
    memcpy(mForegroundMap, tempForegroundMap, sizeof(tempForegroundMap));
    memcpy(mBackgroundMap, tempBackgroundMap, sizeof(tempBackgroundMap));
}

GameObject Map::createObject(int row, int col, SDL_Texture *tex,
                             ObjectType type)
{
    GameObject o;
    o.type = type;
    o.position =
        glm::vec2(col * mTileSize, mHeight - (MAP_ROWS - row) * mTileSize);
    o.texture = tex;
    o.collider = RectF(0, 0, static_cast<float>(mTileSize),
                       static_cast<float>(mTileSize)); // defaults
    return o;
}

void Map::renderMap(std::shared_ptr<GameState> gs,
                    std::shared_ptr<Resources> res)
{
    const auto loadMap = [this, &gs, &res](short layer[MAP_ROWS][MAP_COLS])
    {
        for (int r = 0; r < MAP_ROWS; ++r)
        {
            for (int c = 0; c < MAP_COLS; ++c)
            {
                switch (layer[r][c])
                {
                    case 1: // Ground
                    {
                        GameObject ground = createObject(r, c, res->texGround,
                                                         ObjectType::level);

                        gs->layers[LAYER_IDX_LEVEL].push_back(ground);
                        break;
                    }
                    case 2: // Panel
                    {
                        GameObject panel = createObject(r, c, res->texPanel,
                                                        ObjectType::level);
                        gs->layers[LAYER_IDX_LEVEL].push_back(panel);
                        break;
                    }
                    case 3: // Enemy
                    {
                        GameObject enemy = createObject(r, c, res->texEnemy,
                                                        ObjectType::enemy);
                        enemy.data.enemy = EnemyData();
                        enemy.currentAnimation = res->ANIM_ENEMY;
                        enemy.animations = res->enemyAnims;
                        enemy.collider = RectF(10, 4, 12, 28);
                        enemy.maxSpeedX = 15;
                        enemy.dynamic = true;
                        gs->layers[LAYER_IDX_CHARACTER].push_back(enemy);
                        break;
                    }
                    case 4: // Player
                    {
                        GameObject player = createObject(r, c, res->texIdle,
                                                         ObjectType::player);
                        player.position =
                            glm::vec2(c * mTileSize,
                                      mHeight - (MAP_ROWS - r) * mTileSize);
                        player.data.player = PlayerData();
                        player.animations = res->playerAnims;
                        player.currentAnimation = res->ANIM_PLAYER_IDLE;
                        player.acceleration = glm::vec2(300, 0);
                        player.maxSpeedX = 100;
                        player.dynamic = true;
                        player.collider = RectF(11, 6, 10, 26);
                        gs->layers[LAYER_IDX_CHARACTER].push_back(player);
                        gs->playerIndex =
                            gs->layers[LAYER_IDX_CHARACTER].size() - 1;
                        break;
                    }
                    case 5: // grass
                    {
                        GameObject grass = createObject(r, c, res->texGrass,
                                                        ObjectType::level);
                        gs->foregroundTiles.push_back(grass);
                        break;
                    }
                    case 6: // brick
                    {
                        GameObject brick = createObject(r, c, res->texBrick,
                                                        ObjectType::level);
                        gs->backgroundTiles.push_back(brick);
                        break;
                    }
                }
            }
        }
    };
    loadMap(mMap);
    loadMap(mBackgroundMap);
    loadMap(mForegroundMap);

    assert(gs->playerIndex != -1);
}
}; // namespace Game
