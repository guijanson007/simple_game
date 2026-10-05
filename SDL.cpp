#include "SDL.h"
#include "resources.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <format>

namespace SDL
{

SDL::SDL(Resources *resources, Game::GameState *gameState)
    : mRenderer(nullptr, SDL_DestroyRenderer),
      mKeys(SDL_GetKeyboardState(nullptr)),
      mResources(resources),
      mGameState(gameState),
      mWindow(std::make_unique<Window>())
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        throw "Error initializing SDL";

    // temporary raw pointers to interface with SDL
    SDL_Window *rawWindow = nullptr;
    SDL_Renderer *rawRenderer = nullptr;

    // global state initialization
    mWindow->width = 640;
    mWindow->height = 320;
    mWindow->logW = 640;
    mWindow->logH = 320;

    SDL_CreateWindowAndRenderer("SDL Window", mWindow->width, mWindow->height,
                                SDL_WINDOW_RESIZABLE, &rawWindow, &rawRenderer);

    if (!rawWindow || !rawRenderer)
    {
        throw "Error creating Window and Renderer.";
    }

    // initialize uniquer ptrs from raw pointers
    mWindow->sdlWindow.reset(rawWindow);
    mRenderer.reset(rawRenderer);

    if (!SDL_SetRenderVSync(mRenderer.get(), true))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR",
                                 "Error enabling VSync.",
                                 mWindow->sdlWindow.get());
        throw "Error enabling VSync";
    }

    // initialize SDL_mixer and create its SDL3 audio device
    if (!MIX_Init())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error initializing SDL_mixer",
                                 mWindow->sdlWindow.get());
        throw "Error initializing SDL_mixer";
    }
    mResources->mixer =
        MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mResources->mixer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error creating audio device",
                                 mWindow->sdlWindow.get());
        MIX_Quit();
        throw "Error creating audio device";
    }
    MIX_SetMixerGain(mResources->mixer, 0.05f);

    // load game assets after the renderer and audio mixer have been created
    mResources->loadResources(*mRenderer);

    // configure presentation
    SDL_SetRenderLogicalPresentation(mRenderer.get(), mWindow->logW,
                                     mWindow->logH,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    MIX_PlayTrack(mResources->musicTrack, 0);
}

bool SDL::pollEvent(SDL_Event &event)
{
    return SDL_PollEvent(&event);
}

void SDL::draw(float deltaTime, float playerVelocityX, int tileSize)
{
    SDL_SetRenderDrawColor(mRenderer.get(), 30, 30, 30, 255);
    SDL_RenderClear(mRenderer.get());

    // draw background images
    SDL_RenderTexture(mRenderer.get(), mResources->texBg1, nullptr, nullptr);
    drawParalaxBackground(mResources->texBg4, playerVelocityX,
                          mGameState->bg4Scroll, 0.075f, deltaTime);
    drawParalaxBackground(mResources->texBg3, playerVelocityX,
                          mGameState->bg3Scroll, 0.150f, deltaTime);
    drawParalaxBackground(mResources->texBg2, playerVelocityX,
                          mGameState->bg2Scroll, 0.3f, deltaTime);

    // draw background tiles
    for (Game::GameObject &obj : mGameState->backgroundTiles)
    {
        SDL_FRect ds{.x = obj.position.x - mGameState->mapViewPort.x,
                     .y = obj.position.y - mGameState->mapViewPort.y,
                     .w = static_cast<float>(obj.texture->w),
                     .h = static_cast<float>(obj.texture->h)};
        SDL_RenderTexture(mRenderer.get(), obj.texture, nullptr, &ds);
    }

    // draw all objects
    for (auto &layer : mGameState->layers)
    {
        for (Game::GameObject &obj : layer)
        {
            drawObject(obj, tileSize, tileSize, deltaTime);
        }
    }

    // draw bullets
    for (Game::GameObject &bullet : mGameState->bullets)
    {
        if (bullet.data.bullet.state != Game::BulletState::inactive)
        {
            drawObject(bullet, bullet.collider.w, bullet.collider.h, deltaTime);
        }
    }

    // draw foreground tiles
    for (Game::GameObject &obj : mGameState->foregroundTiles)
    {
        SDL_FRect ds{.x = obj.position.x - mGameState->mapViewPort.x,
                     .y = obj.position.y - mGameState->mapViewPort.y,
                     .w = static_cast<float>(obj.texture->w),
                     .h = static_cast<float>(obj.texture->h)};
        SDL_RenderTexture(mRenderer.get(), obj.texture, nullptr, &ds);
    }

    if (mGameState->debugMode)
    {
        // display some debug info
        SDL_SetRenderDrawColor(mRenderer.get(), 255, 255, 255, 255);
        SDL_RenderDebugText(
            mRenderer.get(), 5, 5,
            std::format(
                "State: {}, Bullets: {}, Grounded: {}",
                static_cast<int>(mGameState->player().data.player.state),
                mGameState->bullets.size(), mGameState->player().grounded)
                .c_str());
    }

    SDL_RenderPresent(mRenderer.get());
}

void SDL::drawParalaxBackground(SDL_Texture *texture, float xVelocity,
                                float &scrollPos, float scrollFactor,
                                float deltaTime)
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
    SDL_RenderTextureTiled(mRenderer.get(), texture, nullptr, 1, &dst);
}

void SDL::drawObject(Game::GameObject &obj, float width, float height,
                     float deltaTime)
{
    float srcX =
        obj.currentAnimation != -1
            ? obj.animations[obj.currentAnimation].currentFrame() * width
            : (obj.spriteFrame - 1) * width;

    SDL_FRect src = {.x = srcX, .y = 0, .w = width, .h = height};
    SDL_FRect dest = {.x = obj.position.x - mGameState->mapViewPort.x,
                      .y = obj.position.y - mGameState->mapViewPort.y,
                      .w = width,
                      .h = height};

    SDL_FlipMode flipMode =
        obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    if (!obj.shouldFlash)
    {
        SDL_RenderTextureRotated(mRenderer.get(), obj.texture, &src, &dest, 0,
                                 nullptr, flipMode);
    }
    else
    {
        // flash object with a redish tint
        SDL_SetTextureColorModFloat(obj.texture, 2.5f, 1.0f, 1.0f);
        SDL_RenderTextureRotated(mRenderer.get(), obj.texture, &src, &dest, 0,
                                 nullptr, flipMode);
        SDL_SetTextureColorModFloat(obj.texture, 1.0f, 1.0f, 1.0f);

        if (obj.flashTimer.step(deltaTime))
        {
            obj.shouldFlash = false;
        }
    }

    if (mGameState->debugMode)
    {
        SDL_FRect rectA = {.x = obj.position.x + obj.collider.x -
                                mGameState->mapViewPort.x,
                           .y = obj.position.y + obj.collider.y,
                           .w = obj.collider.w,
                           .h = obj.collider.h};
        SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mRenderer.get(), 255, 0, 0, 150);
        SDL_RenderFillRect(mRenderer.get(), &rectA);
        SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_NONE);
    }
}

} // namespace SDL
