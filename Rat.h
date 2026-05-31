#pragma once
#include "Entity.h"

class TextureManager;

class Rat : public Entity {
public:
    Rat(TextureManager* tm, float x, float y, float vx);

    void update(float dt) override;
    void render(SDL_Renderer* r, int screenWidth, int screenHeight) override;

private:
    TextureManager* tm_;
    bool facingRight_{ true };
};