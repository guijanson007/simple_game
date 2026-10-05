#include "resources.h"

MIX_Audio *Resources::loadAudio(const std::string &filepath)
{
    MIX_Audio *sound = MIX_LoadAudio(mixer, filepath.c_str(), true);
    audio.push_back(sound);
    return sound;
}

SDL_Texture *Resources::loadTexture(const std::string &filepath,
                                    SDL_Renderer &renderer)
{
    SDL_Texture *tex = IMG_LoadTexture(&renderer, filepath.c_str());
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    textures.push_back(tex);
    return tex;
}

void Resources::loadResources(SDL_Renderer &renderer)
{
    playerAnims.resize(5);
    playerAnims[ANIM_PLAYER_IDLE] = Animation(8, 1.6f);
    playerAnims[ANIM_PLAYER_RUN] = Animation(4, 0.5f);
    playerAnims[ANIM_PLAYER_SLIDE] = Animation(1, 1.0f);
    playerAnims[ANIM_PLAYER_SHOOT] = Animation(4, 0.5f);
    playerAnims[ANIM_PLAYER_SLIDE_SHOOT] = Animation(4, 0.5f);
    bulletAnims.resize(2);
    bulletAnims[ANIM_BULLET_MOVING] = Animation(4, 0.5f);
    bulletAnims[ANIM_BULLET_HIT] = Animation(4, 0.15f);
    enemyAnims.resize(3);
    enemyAnims[ANIM_ENEMY] = Animation(8, 1.0f);
    enemyAnims[ANIM_ENEMY_HIT] = Animation(8, 1.0f);
    enemyAnims[ANIM_ENEMY_DIE] = Animation(18, 2.0f);

    texIdle = loadTexture("data/idle.png", renderer);
    texRun = loadTexture("data/run.png", renderer);
    texSlide = loadTexture("data/slide.png", renderer);
    texBrick = loadTexture("data/tiles/brick.png", renderer);
    texGrass = loadTexture("data/tiles/grass.png", renderer);
    texGround = loadTexture("data/tiles/ground.png", renderer);
    texPanel = loadTexture("data/tiles/panel.png", renderer);
    texBg1 = loadTexture("data/bg/bg_layer1.png", renderer);
    texBg2 = loadTexture("data/bg/bg_layer2.png", renderer);
    texBg3 = loadTexture("data/bg/bg_layer3.png", renderer);
    texBg4 = loadTexture("data/bg/bg_layer4.png", renderer);
    texBullet = loadTexture("data/bullet.png", renderer);
    texBulletHit = loadTexture("data/bullet_hit.png", renderer);
    texShoot = loadTexture("data/shoot.png", renderer);
    texRunShoot = loadTexture("data/shoot_run.png", renderer);
    texSlideShoot = loadTexture("data/slide_shoot.png", renderer);
    texEnemy = loadTexture("data/enemy.png", renderer);
    texEnemyHit = loadTexture("data/enemy_hit.png", renderer);
    texEnemyDie = loadTexture("data/enemy_die.png", renderer);

    chunkShoot = loadAudio("data/audio/shoot.wav");
    chunkShootHit = loadAudio("data/audio/wall_hit.wav");
    chunkEnemyHit = loadAudio("data/audio/enemy_hit.wav");

    musicMain = loadAudio(
        "data/audio/Juhani Junkala [Retro Game Music Pack] Level 1.mp3");
    musicTrack = MIX_CreateTrack(mixer);
    MIX_SetTrackAudio(musicTrack, musicMain);
    MIX_SetTrackLoops(musicTrack, -1);
    MIX_SetTrackGain(musicTrack, 1.0f / 3.0f);
}

// Resources &Resources::operator=(SDL_Renderer &renderer)
// {
//     // Construct a temporary object with the new renderer.
//     // exception safe
//     Resources temp(renderer);
//
//     std::swap(mRenderer, temp.mRenderer);
//     std::swap(textures, temp.textures);
//     std::swap(audio, temp.audio);
//     std::swap(playerAnims, temp.playerAnims);
//     std::swap(bulletAnims, temp.bulletAnims);
//     std::swap(enemyAnims, temp.enemyAnims);
//     std::swap(musicTrack, temp.musicTrack);
//
//     // Swap all raw texture pointers...
//     std::swap(texIdle, temp.texIdle);
//     std::swap(texRun, temp.texRun);
//     std::swap(texSlide, temp.texSlide);
//     std::swap(texBrick, temp.texBrick);
//     std::swap(texGrass, temp.texGrass);
//     std::swap(texGround, temp.texGround);
//     std::swap(texPanel, temp.texPanel);
//     std::swap(texBg1, temp.texBg1);
//     std::swap(texBg2, temp.texBg2);
//     std::swap(texBg3, temp.texBg3);
//     std::swap(texBg4, temp.texBg4);
//     std::swap(texBullet, temp.texBullet);
//     std::swap(texBulletHit, temp.texBulletHit);
//     std::swap(texShoot, temp.texShoot);
//     std::swap(texRunShoot, temp.texRunShoot);
//     std::swap(texSlideShoot, temp.texSlideShoot);
//     std::swap(texEnemy, temp.texEnemy);
//     std::swap(texEnemyHit, temp.texEnemyHit);
//     std::swap(texEnemyDie, temp.texEnemyDie);
//     std::swap(chunkShoot, temp.chunkShoot);
//     std::swap(chunkShootHit, temp.chunkShootHit);
//     std::swap(chunkEnemyHit, temp.chunkEnemyHit);
//     std::swap(musicMain, temp.musicMain);
//     std::swap(musicTrack, temp.musicTrack);
//
//     return *this;
// }

Resources::~Resources()
{
    for (SDL_Texture *tex : textures)
    {
        SDL_DestroyTexture(tex);
    }
    MIX_DestroyTrack(musicTrack);
    for (MIX_Audio *sound : audio)
    {
        MIX_DestroyAudio(sound);
    }
}
