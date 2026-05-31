#pragma once
#include <SDL3/SDL.h>

class Entity {
public:
    SDL_FRect rect{};
    SDL_FPoint velocity{ 0,0 };
    bool alive{ true };

    virtual ~Entity() = default;
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* r, int screenWidth, int screenHeight) = 0;
};