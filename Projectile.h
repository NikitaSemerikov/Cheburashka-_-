#pragma once
#include "Entity.h"
#pragma once
#include "Entity.h"

class TextureManager;

class Projectile : public Entity {
public:
    Projectile(TextureManager* tm, float x, float y, float vx);

    void update(float dt) override;
    void render(SDL_Renderer* r, int screenWidth, int screenHeight) override;

private:
    TextureManager* tm_;
};