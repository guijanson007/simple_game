#include "game.h"
#include "SDL.h"

namespace Game
{
void Game::pushLayer(const std::vector<GameObject> &layer, size_t layerIndex)
{
    if (layerIndex >= mGameState->layers.size())
    {
        throw std::out_of_range("Layer index out of range");
    }
    mGameState->layers[layerIndex] = layer;
}

void Game::start()
{
    // Main game loop
    while (true)
    {
        SDL_Event event;
        while (mSDL.pollEvent(event))
        {
            handleEvent(event);
        }

        iterate();
    }
}

void Game::handleEvent(SDL_Event event)
{
    switch (event.type)
    {
        case SDL_EVENT_QUIT:
            exit(EXIT_SUCCESS);
        case SDL_EVENT_WINDOW_RESIZED:
            mSDL.updateWindowSize(event.window.data1, event.window.data2);
            break;
        case SDL_EVENT_KEY_DOWN:
        {
            handleKeyInput(event.key.scancode, true, mGameState->player());
            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            handleKeyInput(event.key.scancode, false, mGameState->player());
            if (event.key.scancode == SDL_SCANCODE_ESCAPE)
            {
                mGameState->debugMode = !mGameState->debugMode;
            }
            else if (event.key.scancode == SDL_SCANCODE_F)
            {
                mSDL.toggleFullscreen();
            }
            break;
        }
    }
}

void Game::iterate()
{
    static const Uint64 frequency = SDL_GetPerformanceFrequency();
    static Uint64 previous = SDL_GetPerformanceCounter();

    const Uint64 current = SDL_GetPerformanceCounter();
    const float deltaTime =
        static_cast<float>(current - previous) / static_cast<float>(frequency);
    previous = current;

    // update all objects
    for (auto &layer : mGameState->layers)
    {
        for (GameObject &obj : layer)
        {
            update(obj, deltaTime);
        }
    }

    // update bullets
    for (GameObject &bullet : mGameState->bullets)
    {
        update(bullet, deltaTime);
        if (bullet.currentAnimation != -1)
        {
            bullet.animations[bullet.currentAnimation].step(deltaTime);
        }
    }

    // calculate viewport position
    const GameObject &player = mGameState->player();
    mGameState->mapViewPort.x = player.position.x + SPRITE_SIZE / 2.0f -
                                mGameState->mapViewPort.w / 2.0f;

    // mGameState->mapViewPort.y =
    //     player.position.y + SPRITE_SIZE / 2.0f - mGameState->mapViewPort.h
    //     / 2.0f;

    // perform drawing commands
    mSDL.draw(deltaTime, player.velocity.x, TILE_SIZE);
}

void Game::handleKeyInput(SDL_Scancode key, bool keyDown, GameObject &obj)
{
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
}

void Game::update(GameObject &obj, float deltaTime)
{
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
            if (mSDL.isKeyPressed(SDL_SCANCODE_A))
            {
                currentDirection += -1;
            }
            if (mSDL.isKeyPressed(SDL_SCANCODE_D))
            {
                currentDirection += 1;
            }

            Timer &weaponTimer = obj.data.player.weaponTimer;
            weaponTimer.step(deltaTime);

            const auto handleShooting =
                [this, &obj, &weaponTimer](SDL_Texture *tex,
                                           SDL_Texture *shootTex, int animIndex,
                                           int shootAnimIndex)
            {
                if (mSDL.isKeyPressed(SDL_SCANCODE_J))
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
                        bullet.direction = mGameState->player().direction;
                        bullet.texture = mResources->texBullet;
                        bullet.currentAnimation =
                            mResources->ANIM_BULLET_MOVING;
                        bullet.collider = RectF(
                            0, 0, static_cast<float>(mResources->texBullet->h),
                            static_cast<float>(mResources->texBullet->h));
                        constexpr int yVariation = 60;
                        const float yVelocity =
                            SDL_rand(yVariation) - yVariation / 2.0f;
                        bullet.velocity = glm::vec2(
                            obj.velocity.x + 600.0f * obj.direction, yVelocity);
                        bullet.maxSpeedX = 1000.0f;
                        bullet.animations = mResources->bulletAnims;

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

                        // look for an inactive bullet to reuse, otherwise add a
                        // new one
                        bool foundInactive{false};
                        for (int i = 0;
                             i < mGameState->bullets.size() && !foundInactive;
                             ++i)
                        {
                            if (mGameState->bullets[i].data.bullet.state ==
                                BulletState::inactive)
                            {
                                mGameState->bullets[i] = bullet;
                                foundInactive = true;
                            }
                        }
                        // if no inactive slot was found, push a new bullet
                        if (!foundInactive)
                        {
                            mGameState->bullets.push_back(bullet);
                        }
                        MIX_PlayAudio(mResources->mixer,
                                      mResources->chunkShoot);
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
                            const float factor =
                                obj.velocity.x > 0 ? -1.5f : 1.5f;
                            float amount =
                                factor * obj.acceleration.x * deltaTime;
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
                    handleShooting(mResources->texIdle, mResources->texShoot,
                                   mResources->ANIM_PLAYER_IDLE,
                                   mResources->ANIM_PLAYER_SHOOT);
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
                        handleShooting(mResources->texSlide,
                                       mResources->texSlideShoot,
                                       mResources->ANIM_PLAYER_SLIDE,
                                       mResources->ANIM_PLAYER_SLIDE_SHOOT);
                    }
                    else
                    {
                        handleShooting(mResources->texRun,
                                       mResources->texRunShoot,
                                       mResources->ANIM_PLAYER_RUN,
                                       mResources->ANIM_PLAYER_RUN);
                    }
                    break;
                }
                case PlayerState::jumping:
                {
                    handleShooting(mResources->texRun, mResources->texRunShoot,
                                   mResources->ANIM_PLAYER_RUN,
                                   mResources->ANIM_PLAYER_RUN);
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
                if (obj.position.x - mGameState->mapViewPort.x < 0           ||      // left edge
                    obj.position.x - mGameState->mapViewPort.x > mSDL.getWindow().logW  ||     // right edge
                    obj.position.y - mGameState->mapViewPort.y < 0           ||    // top edge
                    obj.position.y - mGameState->mapViewPort.y > mSDL.getWindow().logH)      // bottom edge
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
                    glm::vec2 playerDir =
                        mGameState->player().position - obj.position;
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
                        obj.texture = mResources->texEnemy;
                        obj.currentAnimation = mResources->ANIM_ENEMY;
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
        for (auto &layer : mGameState->layers)
        {
            for (GameObject &objB : layer)
            {
                if (&obj != &objB)
                {
                    checkCollision(obj, objB, deltaTime);

                    if (objB.type == ObjectType::level)
                    {
                        // grounded sensor
                        const float feetY =
                            obj.position.y + obj.collider.y + obj.collider.h;
                        RectF sensor(obj.position.x + obj.collider.x,
                                     feetY - 1.0f, obj.collider.w, 2);
                        RectF rectB(objB.position.x + objB.collider.x,
                                    objB.position.y + objB.collider.y,
                                    objB.collider.w, objB.collider.h);

                        // A wall can overlap the sensor from the side. Only
                        // consider a tile ground when its top surface is at the
                        // player's feet.
                        constexpr float GROUND_TOLERANCE = 1.0f;
                        if (SDL::getRectIntersectionFloat(sensor, rectB)
                                .has_value() &&
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
}

void Game::collisionResponse(const RectF &rectA, const RectF &rectB,
                             RectF &rectC, GameObject &objA, GameObject &objB,
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
                        MIX_PlayAudio(mResources->mixer,
                                      mResources->chunkShootHit);
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
                            objB.texture = mResources->texEnemyHit;
                            objB.currentAnimation = mResources->ANIM_ENEMY_HIT;
                            enemyData.state = EnemyState::damaged;
                            enemyData.healthPoints -= 10;

                            if (enemyData.healthPoints <= 0)
                            {
                                enemyData.state = EnemyState::dead;
                                objB.texture = mResources->texEnemyDie;
                                objB.currentAnimation =
                                    mResources->ANIM_ENEMY_DIE;
                            }
                            MIX_PlayAudio(mResources->mixer,
                                          mResources->chunkEnemyHit);
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
                    objA.texture = mResources->texBulletHit;
                    objA.currentAnimation = mResources->ANIM_BULLET_HIT;
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

void Game::checkCollision(GameObject &a, GameObject &b, float deltaTime)
{
    RectF rectA(a.position.x + a.collider.x, a.position.y + a.collider.y,
                a.collider.w, a.collider.h);
    RectF rectB(b.position.x + b.collider.x, b.position.y + b.collider.y,
                b.collider.w, b.collider.h);

    std::optional<RectF> rectC = SDL::getRectIntersectionFloat(rectA, rectB);

    if (rectC.has_value())
    {
        collisionResponse(rectA, rectB, rectC.value(), a, b, deltaTime);
    }
}
} // namespace Game
