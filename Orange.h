#pragma once
#include "Entity.h"

class TextureManager;

class Orange : public Entity {
public:
    Orange(TextureManager* tm, float x, float y);

    void update(float dt) override;
    void render(SDL_Renderer* r, int screenWidth, int screenHeight) override;

private:
    TextureManager* tm_;
};