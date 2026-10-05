
#include <SDL3/SDL_audio.h>
#define SDL_MAIN_USE_CALLBACKS 1

// STL includes
#include <array>
#include <cstdint>
#include <format>
#include <string>
#include <vector>

// SDL INCLUDES
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

// glm
#include <glm/glm.hpp>

// Own
#include "gameObject.h"

struct SDLState
{
    int width, height, logW, logH;
    SDL_Window *window;
    SDL_Renderer *renderer;
    const bool *keys;
    bool fullscreen;
    SDLState()
        : keys(SDL_GetKeyboardState(nullptr)),
          fullscreen{false}
    {
    }
};

constexpr size_t LAYER_IDX_LEVEL = 0;
constexpr size_t LAYER_IDX_CHARACTER = 1;
constexpr float SPRITE_SIZE = 32.0f;
constexpr int MAP_ROWS = 5;
constexpr int MAP_COLS = 50;
constexpr int TILE_SIZE = 32;

struct GameState
{
    std::array<std::vector<GameObject>, 2> layers;
    std::vector<GameObject> backgroundTiles;
    std::vector<GameObject> foregroundTiles;
    std::vector<GameObject> bullets;
    int playerIndex;
    SDL_FRect mapViewPort;
    float bg2Scroll, bg3Scroll, bg4Scroll;
    bool debugMode;
    GameState()
    {
        bg2Scroll = bg3Scroll = bg4Scroll = 0.0f;
        playerIndex = -1;
        debugMode = false;
    }

    void initializeViewProt(float width, float height)
    {
        mapViewPort = {.x = 0, .y = 0, .w = width, .h = height};
    }
    GameObject &player() { return layers[LAYER_IDX_CHARACTER][playerIndex]; }
};

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

    MIX_Audio *loadAudio(const std::string &filepath)
    {
        MIX_Audio *sound = MIX_LoadAudio(mixer, filepath.c_str(), true);
        audio.push_back(sound);
        return sound;
    }

    SDL_Texture *loadTexture(SDL_Renderer *renderer,
                             const std::string &filepath)
    {
        SDL_Texture *tex = IMG_LoadTexture(renderer, filepath.c_str());
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        textures.push_back(tex);
        return tex;
    }
    void load(SDLState &state)
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

        texIdle = loadTexture(state.renderer, "data/idle.png");
        texRun = loadTexture(state.renderer, "data/run.png");
        texSlide = loadTexture(state.renderer, "data/slide.png");
        texBrick = loadTexture(state.renderer, "data/tiles/brick.png");
        texGrass = loadTexture(state.renderer, "data/tiles/grass.png");
        texGround = loadTexture(state.renderer, "data/tiles/ground.png");
        texPanel = loadTexture(state.renderer, "data/tiles/panel.png");
        texBg1 = loadTexture(state.renderer, "data/bg/bg_layer1.png");
        texBg2 = loadTexture(state.renderer, "data/bg/bg_layer2.png");
        texBg3 = loadTexture(state.renderer, "data/bg/bg_layer3.png");
        texBg4 = loadTexture(state.renderer, "data/bg/bg_layer4.png");
        texBullet = loadTexture(state.renderer, "data/bullet.png");
        texBulletHit = loadTexture(state.renderer, "data/bullet_hit.png");
        texShoot = loadTexture(state.renderer, "data/shoot.png");
        texRunShoot = loadTexture(state.renderer, "data/shoot_run.png");
        texSlideShoot = loadTexture(state.renderer, "data/slide_shoot.png");
        texEnemy = loadTexture(state.renderer, "data/enemy.png");
        texEnemyHit = loadTexture(state.renderer, "data/enemy_hit.png");
        texEnemyDie = loadTexture(state.renderer, "data/enemy_die.png");

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

    void unload()
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
};

// FUNCTION DECLARATIONS
void drawObject(const SDLState &state, GameState &gs, GameObject &obj,
                float width, float height, float deltaTime);
void update(const SDLState &state, GameState &gs, Resources &res,
            GameObject &obj, float deltaTime);
void createTiles(const SDLState &state, GameState &gs, const Resources &res);
void checkCollision(const SDLState &state, GameState &gs, Resources &res,
                    GameObject &a, GameObject &b, float deltaTime);
void handleKeyInput(const SDLState &state, GameState &gs, GameObject &obj,
                    SDL_Scancode key, bool keyDown);
void drawParalaxBackground(SDL_Renderer *renderer, SDL_Texture *texture,
                           float xVelocity, float &scrollPos,
                           float scrollFactor, float deltaTime);

// GLOBALS
SDLState state;
GameState gs;
Resources res;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return SDL_APP_FAILURE;

    // global state initialization
    state.width = 640;
    state.height = 320;
    state.logW = 640;
    state.logH = 320;

    gs.initializeViewProt(state.logW, state.logH);

    SDL_CreateWindowAndRenderer("SDL Window", state.width, state.height,
                                SDL_WINDOW_RESIZABLE, &state.window,
                                &state.renderer);

    if (!state.window || !state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR",
                                 "Error creating Window and Renderer.",
                                 state.window);
        return SDL_APP_FAILURE;
    }

    if (!SDL_SetRenderVSync(state.renderer, true))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR",
                                 "Error enabling VSync.", state.window);
        return SDL_APP_FAILURE;
    }

    // initialize SDL_mixer and create its SDL3 audio device
    if (!MIX_Init())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error initializing SDL_mixer", state.window);
        return SDL_APP_FAILURE;
    }
    res.mixer =
        MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!res.mixer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error creating audio device", state.window);
        MIX_Quit();
        return SDL_APP_FAILURE;
    }
    MIX_SetMixerGain(res.mixer, 0.05f);

    // load game assets after the renderer and audio mixer have been created
    res.load(state);

    // setup game data
    createTiles(state, gs, res);

    // configure presentation
    SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    MIX_PlayTrack(res.musicTrack, 0);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_WINDOW_RESIZED:
            state.width = event->window.data1;
            state.height = event->window.data2;
            break;
        case SDL_EVENT_KEY_DOWN:
        {
            handleKeyInput(state, gs, gs.player(), event->key.scancode, true);
            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            handleKeyInput(state, gs, gs.player(), event->key.scancode, false);
            if (event->key.scancode == SDL_SCANCODE_ESCAPE)
            {
                gs.debugMode = !gs.debugMode;
            }
            else if (event->key.scancode == SDL_SCANCODE_F)
            {
                state.fullscreen = !state.fullscreen;
                SDL_SetWindowFullscreen(state.window, state.fullscreen);
            }
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    static const Uint64 frequency = SDL_GetPerformanceFrequency();
    static Uint64 previous = SDL_GetPerformanceCounter();

    const Uint64 current = SDL_GetPerformanceCounter();
    const float deltaTime =
        static_cast<float>(current - previous) / static_cast<float>(frequency);
    previous = current;

    // update all objects
    for (auto &layer : gs.layers)
    {
        for (GameObject &obj : layer)
        {
            update(state, gs, res, obj, deltaTime);
        }
    }

    // update bullets
    for (GameObject &bullet : gs.bullets)
    {
        update(state, gs, res, bullet, deltaTime);
        if (bullet.currentAnimation != -1)
        {
            bullet.animations[bullet.currentAnimation].step(deltaTime);
        }
    }

    // calculate viewport position
    const GameObject &player = gs.player();
    gs.mapViewPort.x =
        player.position.x + SPRITE_SIZE / 2.0f - gs.mapViewPort.w / 2.0f;

    // gs.mapViewPort.y =
    //     player.position.y + SPRITE_SIZE / 2.0f - gs.mapViewPort.h / 2.0f;

    // perform drawing commands
    SDL_SetRenderDrawColor(state.renderer, 30, 30, 30, 255);
    SDL_RenderClear(state.renderer);

    // draw background images
    SDL_RenderTexture(state.renderer, res.texBg1, nullptr, nullptr);
    drawParalaxBackground(state.renderer, res.texBg4, player.velocity.x,
                          gs.bg4Scroll, 0.075f, deltaTime);
    drawParalaxBackground(state.renderer, res.texBg3, player.velocity.x,
                          gs.bg3Scroll, 0.150f, deltaTime);
    drawParalaxBackground(state.renderer, res.texBg2, player.velocity.x,
                          gs.bg2Scroll, 0.3f, deltaTime);

    // draw background tiles
    for (GameObject &obj : gs.backgroundTiles)
    {
        SDL_FRect ds{.x = obj.position.x - gs.mapViewPort.x,
                     .y = obj.position.y - gs.mapViewPort.y,
                     .w = static_cast<float>(obj.texture->w),
                     .h = static_cast<float>(obj.texture->h)};
        SDL_RenderTexture(state.renderer, obj.texture, nullptr, &ds);
    }

    // draw all objects
    for (auto &layer : gs.layers)
    {
        for (GameObject &obj : layer)
        {
            drawObject(state, gs, obj, TILE_SIZE, TILE_SIZE, deltaTime);
        }
    }

    // draw bullets
    for (GameObject &bullet : gs.bullets)
    {
        if (bullet.data.bullet.state != BulletState::inactive)
        {
            drawObject(state, gs, bullet, bullet.collider.w, bullet.collider.h,
                       deltaTime);
        }
    }

    // draw foreground tiles
    for (GameObject &obj : gs.foregroundTiles)
    {
        SDL_FRect ds{.x = obj.position.x - gs.mapViewPort.x,
                     .y = obj.position.y - gs.mapViewPort.y,
                     .w = static_cast<float>(obj.texture->w),
                     .h = static_cast<float>(obj.texture->h)};
        SDL_RenderTexture(state.renderer, obj.texture, nullptr, &ds);
    }

    if (gs.debugMode)
    {
        // display some debug info
        SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(
            state.renderer, 5, 5,
            std::format("State: {}, Bullets: {}, Grounded: {}",
                        static_cast<int>(gs.player().data.player.state),
                        gs.bullets.size(), gs.player().grounded)
                .c_str());
    }

    SDL_RenderPresent(state.renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    res.unload();
    MIX_DestroyMixer(res.mixer);
    MIX_Quit();

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

void drawObject(const SDLState &state, GameState &gs, GameObject &obj,
                float width, float height, float deltaTime)
{
    float srcX =
        obj.currentAnimation != -1
            ? obj.animations[obj.currentAnimation].currentFrame() * width
            : (obj.spriteFrame - 1) * width;

    SDL_FRect src = {.x = srcX, .y = 0, .w = width, .h = height};
    SDL_FRect dest = {.x = obj.position.x - gs.mapViewPort.x,
                      .y = obj.position.y - gs.mapViewPort.y,
                      .w = width,
                      .h = height};

    SDL_FlipMode flipMode =
        obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    if (!obj.shouldFlash)
    {
        SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dest, 0,
                                 nullptr, flipMode);
    }
    else
    {
        // flash object with a redish tint
        SDL_SetTextureColorModFloat(obj.texture, 2.5f, 1.0f, 1.0f);
        SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dest, 0,
                                 nullptr, flipMode);
        SDL_SetTextureColorModFloat(obj.texture, 1.0f, 1.0f, 1.0f);

        if (obj.flashTimer.step(deltaTime))
        {
            obj.shouldFlash = false;
        }
    }

    if (gs.debugMode)
    {
        SDL_FRect rectA = {.x = obj.position.x + obj.collider.x -
                                gs.mapViewPort.x,
                           .y = obj.position.y + obj.collider.y,
                           .w = obj.collider.w,
                           .h = obj.collider.h};
        SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(state.renderer, 255, 0, 0, 150);
        SDL_RenderFillRect(state.renderer, &rectA);
        SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_NONE);
    }
}

void update(const SDLState &state, GameState &gs, Resources &res,
            GameObject &obj, float deltaTime)
{
    // update the animation
    if (obj.currentAnimation != -1)
    {
        obj.animations[obj.currentAnimation].step(deltaTime);
    }

    // Apply some gravity
    if (obj.dynamic && !obj.grounded)
    {
        obj.velocity += glm::vec2(0, 500) * deltaTime;
    }
    float currentDirection = 0;
    if (obj.type == ObjectType::player)
    {
        if (state.keys[SDL_SCANCODE_A])
        {
            currentDirection += -1;
        }
        if (state.keys[SDL_SCANCODE_D])
        {
            currentDirection += 1;
        }

        Timer &weaponTimer = obj.data.player.weaponTimer;
        weaponTimer.step(deltaTime);

        const auto handleShooting = [&state, &gs, &res, &obj, &weaponTimer](
                                        SDL_Texture *tex, SDL_Texture *shootTex,
                                        int animIndex, int shootAnimIndex)
        {
            if (state.keys[SDL_SCANCODE_J])
            {
                // set shooting tex/animation
                obj.texture = shootTex;
                obj.currentAnimation = shootAnimIndex;

                if (weaponTimer.isTimeout())
                {
                    weaponTimer.reset();

                    // spawn some bullets
                    GameObject bullet;
                    bullet.data.bullet = BulletData();
                    bullet.type = ObjectType::bullet;
                    bullet.direction = gs.player().direction;
                    bullet.texture = res.texBullet;
                    bullet.currentAnimation = res.ANIM_BULLET_MOVING;
                    bullet.collider =
                        SDL_FRect{.x = 0,
                                  .y = 0,
                                  .w = static_cast<float>(res.texBullet->h),
                                  .h = static_cast<float>(res.texBullet->h)};
                    constexpr int yVariation = 60;
                    const float yVelocity =
                        SDL_rand(yVariation) - yVariation / 2.0f;
                    bullet.velocity = glm::vec2(
                        obj.velocity.x + 600.0f * obj.direction, yVelocity);
                    bullet.maxSpeedX = 1000.0f;
                    bullet.animations = res.bulletAnims;

                    // adjust bullet start position
                    const float left = 4.0f;
                    const float right = 24.0f;
                    const float t = (obj.direction + 1) /
                                    2.0f; // results in a value of 0..1
                    const float xOffset =
                        left + right * t; // LERP between left and right
                                          // based on direction
                    bullet.position =
                        glm::vec2(obj.position.x + xOffset,
                                  obj.position.y + TILE_SIZE / 2.0f + 1.0f);

                    // look for an inactive bullet to reuse, otherwise add a new
                    // one
                    bool foundInactive{false};
                    for (int i = 0; i < gs.bullets.size() && !foundInactive;
                         ++i)
                    {
                        if (gs.bullets[i].data.bullet.state ==
                            BulletState::inactive)
                        {
                            gs.bullets[i] = bullet;
                            foundInactive = true;
                        }
                    }
                    // if no inactive slot was found, push a new bullet
                    if (!foundInactive)
                    {
                        gs.bullets.push_back(bullet);
                    }
                    MIX_PlayAudio(res.mixer, res.chunkShoot);
                }
            }
            else
            {
                obj.texture = tex;
                obj.currentAnimation = animIndex;
            }
        };

        switch (obj.data.player.state)
        {
            case PlayerState::idle:
            {
                // switching to running state
                if (currentDirection)
                {
                    obj.data.player.state = PlayerState::running;
                }
                else
                {
                    // decelerate
                    if (obj.velocity.x)
                    {
                        const float factor = obj.velocity.x > 0 ? -1.5f : 1.5f;
                        float amount = factor * obj.acceleration.x * deltaTime;
                        if (std::abs(obj.velocity.x) < std::abs(amount))
                        {
                            obj.velocity.x = 0;
                        }
                        else
                        {
                            obj.velocity.x += amount;
                        }
                    }
                }
                handleShooting(res.texIdle, res.texShoot, res.ANIM_PLAYER_IDLE,
                               res.ANIM_PLAYER_SHOOT);
                break;
            }
            case PlayerState::running:
            {
                // switching to idle state
                if (!currentDirection)
                {
                    obj.data.player.state = PlayerState::idle;
                }

                // moving in opposite direction of velocity, sliding!
                if (obj.velocity.x * obj.direction < 0 && obj.grounded)
                {
                    handleShooting(res.texSlide, res.texSlideShoot,
                                   res.ANIM_PLAYER_SLIDE,
                                   res.ANIM_PLAYER_SLIDE_SHOOT);
                }
                else
                {
                    handleShooting(res.texRun, res.texRunShoot,
                                   res.ANIM_PLAYER_RUN, res.ANIM_PLAYER_RUN);
                }
                break;
            }
            case PlayerState::jumping:
            {
                handleShooting(res.texRun, res.texRunShoot, res.ANIM_PLAYER_RUN,
                               res.ANIM_PLAYER_RUN);
                break;
            }
        }
    }
    else if (obj.type == ObjectType::bullet)
    {
        switch (obj.data.bullet.state)
        {
            case BulletState::moving:
            {
                // clang-format off
                if (obj.position.x - gs.mapViewPort.x < 0           ||      // left edge
                    obj.position.x - gs.mapViewPort.x > state.logW  ||     // right edge
                    obj.position.y - gs.mapViewPort.y < 0           ||    // top edge
                    obj.position.y - gs.mapViewPort.y > state.logH)      // bottom edge
                // clang-format on
                {
                    obj.data.bullet.state = BulletState::inactive;
                }
                break;
            }
            case BulletState::colliding:
            {
                if (obj.animations[obj.currentAnimation].isDone())
                {
                    obj.data.bullet.state = BulletState::inactive;
                }
                break;
            }
        }
    }

    else if (obj.type == ObjectType::enemy)
    {
        EnemyData &enemyData = obj.data.enemy;
        switch (enemyData.state)
        {
            case EnemyState::shambling:
            {
                glm::vec2 playerDir = gs.player().position - obj.position;
                if (glm::length(playerDir) < 100.0f)
                {
                    currentDirection = playerDir.x > 0 ? 1 : -1;
                    obj.acceleration = glm::vec2(30, 0);
                }
                else
                {
                    obj.acceleration = glm::vec2(0, 0);
                    obj.velocity.x = 0;
                }
                break;
            }
            case EnemyState::damaged:
            {
                if (enemyData.damagedTimer.step(deltaTime))
                {
                    enemyData.state = EnemyState::shambling;
                    obj.texture = res.texEnemy;
                    obj.currentAnimation = res.ANIM_ENEMY;
                }
                break;
            }
            case EnemyState::dead:
            {
                obj.velocity.x = 0;
                if (obj.currentAnimation != -1 &&
                    obj.animations[obj.currentAnimation].isDone())
                {
                    // remove animation and set to last frame
                    obj.currentAnimation = -1;
                    obj.spriteFrame = 18;
                }
                break;
            }
        }
    }

    if (currentDirection)
    {
        obj.direction = currentDirection;
    }

    // add acceleration to velocity
    obj.velocity += currentDirection * obj.acceleration * deltaTime;
    if (std::abs(obj.velocity.x) > obj.maxSpeedX)
    {
        obj.velocity.x = currentDirection * obj.maxSpeedX;
    }

    // add velocity to position
    obj.position += obj.velocity * deltaTime;

    bool foundGround = false;
    for (auto &layer : gs.layers)
    {
        for (GameObject &objB : layer)
        {
            if (&obj != &objB)
            {
                checkCollision(state, gs, res, obj, objB, deltaTime);

                if (objB.type == ObjectType::level)
                {
                    // grounded sensor
                    const float feetY =
                        obj.position.y + obj.collider.y + obj.collider.h;
                    SDL_FRect sensor{.x = obj.position.x + obj.collider.x,
                                     .y = feetY - 1.0f,
                                     .w = obj.collider.w,
                                     .h = 2};
                    SDL_FRect rectB{.x = objB.position.x + objB.collider.x,
                                    .y = objB.position.y + objB.collider.y,
                                    .w = objB.collider.w,
                                    .h = objB.collider.h};
                    SDL_FRect rectC{0};
                    // A wall can overlap the sensor from the side. Only
                    // consider a tile ground when its top surface is at the
                    // player's feet.
                    constexpr float GROUND_TOLERANCE = 1.0f;
                    if (SDL_GetRectIntersectionFloat(&sensor, &rectB, &rectC) &&
                        rectB.y >= feetY - GROUND_TOLERANCE &&
                        rectB.y <= feetY + GROUND_TOLERANCE)
                    {
                        foundGround = true;
                    }
                }
            }
        }
    }
    if (obj.grounded != foundGround)
    {
        // switching grounded state
        obj.grounded = foundGround;
        if (foundGround && obj.type == ObjectType::player)
        {
            obj.data.player.state = PlayerState::running;
        }
    }
}

void collisionResponse(const SDLState &state, GameState &gs, Resources &res,
                       const SDL_FRect &rectA, const SDL_FRect &rectB,
                       SDL_FRect &rectC, GameObject &objA, GameObject &objB,
                       float deltaTime)
{
    const auto genericResponse = [&]()
    {
        if (rectC.w < rectC.h)
        {
            // horizontal collision
            if (objA.velocity.x > 0) // going right
            {
                objA.position.x -= rectC.w;
            }
            else if (objA.velocity.x < 0) // going left
            {
                objA.position.x += rectC.w;
            }
            objA.velocity.x = 0;
        }
        else
        {
            // vertical collision
            if (objA.velocity.y > 0) // going down
            {
                objA.position.y -= rectC.h;
            }
            else if (objA.velocity.y < 0) // going up
            {
                objA.position.y += rectC.h;
            }
            objA.velocity.y = 0;
        }
    };

    if (objA.type == ObjectType::player)
    {
        switch (objB.type)
        {
            case ObjectType::level:
            {
                genericResponse();
                break;
            }
            case ObjectType::enemy:
            {
                if (objB.data.enemy.state != EnemyState::dead)
                {
                    objA.velocity = glm::vec2(100, 0) * -objA.direction;
                }
                break;
            }
        }
    }
    else if (objA.type == ObjectType::bullet)
    {
        bool passThrough{false};
        switch (objA.data.bullet.state)
        {
            case BulletState::moving:
            {
                switch (objB.type)
                {
                    case ObjectType::level:
                    {
                        MIX_PlayAudio(res.mixer, res.chunkShootHit);
                        break;
                    }
                    case ObjectType::enemy:
                    {
                        EnemyData &enemyData = objB.data.enemy;
                        if (enemyData.state != EnemyState::dead)
                        {
                            objB.direction = -objA.direction;
                            objB.shouldFlash = true;
                            objB.flashTimer.reset();
                            objB.texture = res.texEnemyHit;
                            objB.currentAnimation = res.ANIM_ENEMY_HIT;
                            enemyData.state = EnemyState::damaged;
                            enemyData.healthPoints -= 10;

                            if (enemyData.healthPoints <= 0)
                            {
                                enemyData.state = EnemyState::dead;
                                objB.texture = res.texEnemyDie;
                                objB.currentAnimation = res.ANIM_ENEMY_DIE;
                            }
                            MIX_PlayAudio(res.mixer, res.chunkEnemyHit);
                        }
                        else
                        {
                            passThrough = true;
                        }
                        break;
                    }
                }
                if (!passThrough)
                {
                    genericResponse();
                    objA.velocity *= 0;
                    objA.data.bullet.state = BulletState::colliding;
                    objA.texture = res.texBulletHit;
                    objA.currentAnimation = res.ANIM_BULLET_HIT;
                    break;
                }
            }
        }
    }
    else if (objA.type == ObjectType::enemy)
    {
        genericResponse();
    }
}

void checkCollision(const SDLState &state, GameState &gs, Resources &res,
                    GameObject &a, GameObject &b, float deltaTime)
{
    SDL_FRect rectA = {.x = a.position.x + a.collider.x,
                       .y = a.position.y + a.collider.y,
                       .w = a.collider.w,
                       .h = a.collider.h};
    SDL_FRect rectB = {.x = b.position.x + b.collider.x,
                       .y = b.position.y + b.collider.y,
                       .w = b.collider.w,
                       .h = b.collider.h};

    SDL_FRect rectC;

    if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &rectC))
    {
        collisionResponse(state, gs, res, rectA, rectB, rectC, a, b, deltaTime);
    }
}

void createTiles(const SDLState &state, GameState &gs, const Resources &res)
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
    short map[MAP_ROWS][MAP_COLS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0,
        0, 0, 4, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    short foreground[MAP_ROWS][MAP_COLS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        5, 0, 0, 5, 5, 5, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    short background[MAP_ROWS][MAP_COLS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 6, 0, 6, 6, 6, 6, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    // clang-format on

    const auto loadMap = [&state, &gs, &res](short layer[MAP_ROWS][MAP_COLS])
    {
        const auto createObject =
            [&state](int r, int c, SDL_Texture *tex, ObjectType type)
        {
            GameObject o;
            o.type = type;
            o.position = glm::vec2(c * TILE_SIZE,
                                   state.logH - (MAP_ROWS - r) * TILE_SIZE);
            o.texture = tex;
            o.collider = {
                .x = 0, .y = 0., .w = TILE_SIZE, .h = TILE_SIZE}; // defaults
            return o;
        };

        for (int r = 0; r < MAP_ROWS; ++r)
        {
            for (int c = 0; c < MAP_COLS; ++c)
            {
                switch (layer[r][c])
                {
                    case 1: // Ground
                    {
                        GameObject ground = createObject(r, c, res.texGround,
                                                         ObjectType::level);
                        gs.layers[LAYER_IDX_LEVEL].push_back(ground);
                        break;
                    }
                    case 2: // Panel
                    {
                        GameObject panel =
                            createObject(r, c, res.texPanel, ObjectType::level);
                        gs.layers[LAYER_IDX_LEVEL].push_back(panel);
                        break;
                    }
                    case 3: // Enemy
                    {
                        GameObject enemy =
                            createObject(r, c, res.texEnemy, ObjectType::enemy);
                        enemy.data.enemy = EnemyData();
                        enemy.currentAnimation = res.ANIM_ENEMY;
                        enemy.animations = res.enemyAnims;
                        enemy.collider =
                            SDL_FRect{.x = 10, .y = 4, .w = 12, .h = 28};
                        enemy.maxSpeedX = 15;
                        enemy.dynamic = true;
                        gs.layers[LAYER_IDX_CHARACTER].push_back(enemy);
                        break;
                    }
                    case 4: // Player
                    {
                        GameObject player =
                            createObject(r, c, res.texIdle, ObjectType::player);
                        player.position =
                            glm::vec2(c * TILE_SIZE,
                                      state.logH - (MAP_ROWS - r) * TILE_SIZE);
                        player.data.player = PlayerData();
                        player.animations = res.playerAnims;
                        player.currentAnimation = res.ANIM_PLAYER_IDLE;
                        player.acceleration = glm::vec2(300, 0);
                        player.maxSpeedX = 100;
                        player.dynamic = true;
                        player.collider = {.x = 11, .y = 6, .w = 10, .h = 26};
                        gs.layers[LAYER_IDX_CHARACTER].push_back(player);
                        gs.playerIndex =
                            gs.layers[LAYER_IDX_CHARACTER].size() - 1;
                        break;
                    }
                    case 5: // grass
                    {
                        GameObject grass =
                            createObject(r, c, res.texGrass, ObjectType::level);
                        gs.foregroundTiles.push_back(grass);
                        break;
                    }
                    case 6: // brick
                    {
                        GameObject brick =
                            createObject(r, c, res.texBrick, ObjectType::level);
                        gs.backgroundTiles.push_back(brick);
                        break;
                    }
                }
            }
        }
    };

    loadMap(map);
    loadMap(background);
    loadMap(foreground);

    assert(gs.playerIndex != -1);
}

void handleKeyInput(const SDLState &state, GameState &gs, GameObject &obj,
                    SDL_Scancode key, bool keyDown)
{
    constexpr float JUMP_FORCE = -200.0f;
    if (obj.type == ObjectType::player)
    {
        switch (obj.data.player.state)
        {
            case PlayerState::idle:
            {
                if (key == SDL_SCANCODE_K && keyDown && obj.grounded)
                {
                    // debug
                    // SDL_Log("Idle state: key %d, keyDown %d, grounded
                    // %d", key, keyDown, obj.grounded);
                    obj.data.player.state = PlayerState::jumping;
                    obj.grounded = false;
                    obj.velocity.y = JUMP_FORCE;
                }
                break;
            }
            case PlayerState::running:
            {
                // debug
                // SDL_Log("Running state: key %d, keyDown %d, grounded %d",
                // key,
                //         keyDown, obj.grounded);
                if (key == SDL_SCANCODE_K && keyDown && obj.grounded)
                {
                    obj.data.player.state = PlayerState::jumping;
                    obj.grounded = false;
                    obj.velocity.y = JUMP_FORCE;
                }
                break;
            }
        }
    }
}

void drawParalaxBackground(SDL_Renderer *renderer, SDL_Texture *texture,
                           float xVelocity, float &scrollPos,
                           float scrollFactor, float deltaTime)
{
    scrollPos -= xVelocity * scrollFactor * deltaTime;
    if (scrollPos <= -texture->w)
    {
        scrollPos = 0;
    }

    SDL_FRect dst{.x = scrollPos,
                  .y = 30,
                  .w = texture->w * 2.0f,
                  .h = static_cast<float>(texture->h)};

    SDL_RenderTextureTiled(renderer, texture, nullptr, 1, &dst);
}
