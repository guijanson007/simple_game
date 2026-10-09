#include "SDL.h"
#include "gameObject.h"
#include "gameState.h"
#include "platform.h"

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

Event SDL::pollEvent()
{
    SDL_Event sdlEvent;
    SDL_PollEvent(&sdlEvent);

    Event event;

    switch (sdlEvent.type)
    {
        case SDL_EVENT_QUIT:
            event.type = EventType::Quit;
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            event.type = EventType::WindowResized;
            event.window.first = sdlEvent.window.data1;
            event.window.second = sdlEvent.window.data2;
            break;
        case SDL_EVENT_KEY_DOWN:
            event.type = EventType::KeyDown;
            event.key.scancode = sdlEvent.key.scancode;
            break;
        case SDL_EVENT_KEY_UP:
            event.type = EventType::KeyUp;
            event.key.scancode = sdlEvent.key.scancode;
            break;
        default:
            event.type = EventType::NoEvent;
    }
    return event;
}

void SDL::drawParalaxBackground(TextureId texture, float xVelocity,
                                float &scrollPos, float scrollFactor,
                                float deltaTime)
{
    SDL_Texture *sdlTexture = mResources->textures[static_cast<int>(texture)];

    scrollPos -= xVelocity * scrollFactor * deltaTime;
    if (scrollPos <= -sdlTexture->w)
    {
        scrollPos = 0;
    }

    SDL_FRect dst{.x = scrollPos,
                  .y = 30,
                  .w = sdlTexture->w * 2.0f,
                  .h = static_cast<float>(sdlTexture->h)};
    SDL_RenderTextureTiled(mRenderer.get(), sdlTexture, nullptr, 1, &dst);
}

void SDL::drawObject(Game::GameObject &obj, float width, float height,
                     float deltaTime)
{
    SDL_Texture *sdlTexture =
        mResources->textures[static_cast<int>(obj.texture)];

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
        SDL_RenderTextureRotated(mRenderer.get(), sdlTexture, &src, &dest, 0,
                                 nullptr, flipMode);
    }
    else
    {
        // flash object with a redish tint
        SDL_SetTextureColorModFloat(sdlTexture, 2.5f, 1.0f, 1.0f);
        SDL_RenderTextureRotated(mRenderer.get(), sdlTexture, &src, &dest, 0,
                                 nullptr, flipMode);
        SDL_SetTextureColorModFloat(sdlTexture, 1.0f, 1.0f, 1.0f);

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

std::optional<RectF> getRectIntersectionFloat(const RectF &a, const RectF &b)
{
    SDL_FRect intersection;
    SDL_FRect rectA = a.toSDLFRect();
    SDL_FRect rectB = b.toSDLFRect();

    if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &intersection))
    {
        RectF result(intersection);
        return result;
    }
    return {};
}

} // namespace SDL
