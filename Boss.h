#pragma once
#include "Entity.h"

class TextureManager;

class Boss : public Entity {
public:
    Boss(TextureManager* tm, float x, float y);

    void update(float dt) override {}
    void render(SDL_Renderer* r, int screenWidth, int screenHeight) override;

    void hit() { ++hits_; }
    int getHits() const { return hits_; }

private:
    TextureManager* tm_;
    int hits_{ 0 };
};