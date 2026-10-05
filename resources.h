#pragma once

#include "animation.h"

// SDL
#include "SDL_API.h"

// STL includes
#include <SDL3/SDL_render.h>
#include <string>
#include <vector>

struct Resources
{
    const int ANIM_PLAYER_IDLE = 0;
    const int ANIM_PLAYER_RUN = 1;
    const int ANIM_PLAYER_SLIDE = 2;
    const int ANIM_PLAYER_SHOOT = 3;
    const int ANIM_PLAYER_SLIDE_SHOOT = 4;
    std::vector<Animation> playerAnims;
    const int ANIM_BULLET_MOVING = 0;
    const int ANIM_BULLET_HIT = 1;
    std::vector<Animation> bulletAnims;
    const int ANIM_ENEMY = 0;
    const int ANIM_ENEMY_HIT = 1;
    const int ANIM_ENEMY_DIE = 2;
    std::vector<Animation> enemyAnims;
    std::vector<SDL_Texture *> textures;
    SDL_Texture *texIdle, *texRun, *texBrick, *texGrass, *texGround, *texPanel,
        *texSlide, *texBg1, *texBg2, *texBg3, *texBg4, *texBullet,
        *texBulletHit, *texShoot, *texRunShoot, *texSlideShoot, *texEnemy,
        *texEnemyHit, *texEnemyDie;
    MIX_Mixer *mixer = nullptr;
    std::vector<MIX_Audio *> audio;
    MIX_Audio *chunkShoot, *chunkShootHit, *chunkEnemyHit, *musicMain;
    MIX_Track *musicTrack;

    Resources() {}

    void loadResources(SDL_Renderer &renderer);

    ~Resources();

  private:
    MIX_Audio *loadAudio(const std::string &filepath);

    SDL_Texture *loadTexture(const std::string &filepath,
                             SDL_Renderer &renderer);
};
