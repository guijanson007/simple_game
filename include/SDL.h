#pragma once

// SDL
#include "SDL_API.h"
#include "gameObject.h"
#include "platform.h"
#include "resources.h"

// STL includes
#include <SDL3/SDL_scancode.h>
#include <memory>
#include <optional>
#include <span>

namespace Game
{
struct GameState;
} // namespace Game

struct Resources;

namespace SDL
{

// clang-format off
inline KeyInput keyInputFromSDLScancode(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_LEFT:  case SDL_SCANCODE_A:     return KeyInput::Left;
        case SDL_SCANCODE_RIGHT: case SDL_SCANCODE_D:     return KeyInput::Right;
        case SDL_SCANCODE_SPACE:                          return KeyInput::Jump;
        case SDL_SCANCODE_J:                              return KeyInput::Shoot;
        case SDL_SCANCODE_F:                              return KeyInput::ToggleFullscreen;
        case SDL_SCANCODE_ESCAPE:                         return KeyInput::Escape;
        default:                                          return KeyInput::None;
    }
}

inline SDL_Scancode scancodeFromKeyInput(KeyInput key) {
    switch (key) {
        case KeyInput::Left:             return SDL_SCANCODE_A;
        case KeyInput::Right:            return SDL_SCANCODE_D;
        case KeyInput::Jump:             return SDL_SCANCODE_SPACE;
        case KeyInput::Shoot:            return SDL_SCANCODE_J;
        case KeyInput::ToggleFullscreen: return SDL_SCANCODE_F;
        case KeyInput::Escape:           return SDL_SCANCODE_ESCAPE;
        default:                         return SDL_SCANCODE_UNKNOWN;
    }
}
//clang-format on

struct Window
{
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> sdlWindow;
    int width, height, logW, logH;
    bool fullscreen;

    Window()
        : sdlWindow(nullptr, SDL_DestroyWindow),
          width{640},
          height{320},
          logW{640},
          logH{320},
          fullscreen{false}
    {
    }
};

std::optional<RectF> getRectIntersectionFloat(const RectF &a, const RectF &b);

class SDL //: public Platform
{
  public:
    SDL(Resources *resources, Game::GameState *gameState);
    ~SDL() = default;

    Event pollEvent();

    void drawTexture(TextureId textureId, const RectF &src, const RectF &dst)
    {
        SDL_Texture *texture =
            mResources->textures[static_cast<int>(textureId)];
        SDL_FRect srcF = src.toSDLFRect();
        SDL_FRect dstF = dst.toSDLFRect();

        SDL_RenderTexture(mRenderer.get(), texture, &srcF, &dstF);
    }

    void drawTexture(TextureId textureId)
    {
        SDL_Texture *texture =
            mResources->textures[static_cast<int>(textureId)];
        SDL_RenderTexture(mRenderer.get(), texture, nullptr, nullptr);
    }
    void drawTexture(TextureId textureId, const RectF &dst)
    {
        SDL_Texture *texture =
            mResources->textures[static_cast<int>(textureId)];
        SDL_FRect dstF = dst.toSDLFRect();
        SDL_RenderTexture(mRenderer.get(), texture, nullptr, &dstF);
    }
    void drawObject(Game::GameObject &obj, float width, float height,
                    float deltaTime);

    void drawTiles(std::span<const Game::GameObject> tiles,
                   const RectF &mapViewPort)
    {
        for (const auto &obj : tiles)
        {
            SDL_Texture *sdlTexture =
                mResources->textures[static_cast<int>(obj.texture)];
            RectF ds(obj.position.x - mapViewPort.x,
                     obj.position.y - mapViewPort.y,
                     static_cast<float>(sdlTexture->w),
                     static_cast<float>(sdlTexture->h));
            drawTexture(obj.texture, ds);
        }
    }

    void renderDebugText(int x, int y, const std::string &text)
    {
        SDL_RenderDebugText(mRenderer.get(), x, y, text.c_str());
    }

    void presentRenderer() { SDL_RenderPresent(mRenderer.get()); }

    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        SDL_SetRenderDrawColor(mRenderer.get(), r, g, b, a);
    }

    void clear() { SDL_RenderClear(mRenderer.get()); }

    void drawParalaxBackground(TextureId texture, float xVelocity,
                               float &scrollPos, float scrollFactor,
                               float deltaTime);

    void updateWindowSize(int width, int height)
    {
        mWindow->width = width;
        mWindow->height = height;
    }

    void toggleFullscreen()
    {
        mWindow->fullscreen = !mWindow->fullscreen;
        SDL_SetWindowFullscreen(mWindow->sdlWindow.get(), mWindow->fullscreen);
    }

    [[nodiscard]] SDL_Renderer &getRenderer() const { return *mRenderer; }

    [[nodiscard]] Window &getWindow() const { return *mWindow; }

    [[nodiscard]] bool isKeyPressed(KeyInput key) const
    {
        SDL_Scancode sdlKey = scancodeFromKeyInput(key);
        return mKeys[sdlKey] != 0;
    }

  private:
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> mRenderer;
    std::shared_ptr<Resources> mResources;
    std::unique_ptr<Window> mWindow;
    std::shared_ptr<Game::GameState> mGameState;
    const bool *mKeys;
};
} // namespace SDL
