#pragma once

// SDL
#include "SDL_API.h"

// own includes
#include "gameObject.h"
#include "gameState.h"
#include "resources.h"

// STL includes
#include <SDL3/SDL_scancode.h>
#include <memory>

namespace SDL
{
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

class SDL
{
  public:
    SDL(Resources *resources, Game::GameState *gameState);
    ~SDL() = default;

    bool pollEvent(SDL_Event &event);

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

    void draw(float deltaTime, float playerVelocityX, int tileSize);

    [[nodiscard]] SDL_Renderer &getRenderer() const { return *mRenderer; }

    [[nodiscard]] Window &getWindow() const { return *mWindow; }

    [[nodiscard]] bool isKeyPressed(SDL_Scancode key) const
    {
        return mKeys[key];
    }

  private:
    void drawParalaxBackground(SDL_Texture *texture, float xVelocity,
                               float &scrollPos, float scrollFactor,
                               float deltaTime);

    void drawObject(Game::GameObject &obj, float width, float height,
                    float deltaTime);

    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> mRenderer;
    std::shared_ptr<Resources> mResources;
    std::unique_ptr<Window> mWindow;
    std::shared_ptr<Game::GameState> mGameState;
    const bool *mKeys;
};
} // namespace SDL
