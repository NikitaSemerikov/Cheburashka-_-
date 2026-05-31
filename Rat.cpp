#include "Rat.h"
#include "TextureManager.h"
#include "Config.h"

Rat::Rat(TextureManager* tm, float x, float y, float vx)
    : tm_(tm) {
    rect = { x, y, 48, 32 };
    velocity.x = vx;
    facingRight_ = (vx > 0);
}

void Rat::update(float dt) {
    rect.x += velocity.x * dt;
    if (velocity.x > 0) {
        facingRight_ = true;
    }
    else if (velocity.x < 0) {
        facingRight_ = false;
    }
}

void Rat::render(SDL_Renderer* r, int screenWidth, int screenHeight) {
    float scaleX = (float)screenWidth / WINDOW_WIDTH;
    float scaleY = (float)screenHeight / WINDOW_HEIGHT;

    std::string texName = facingRight_ ? "rat_right" : "rat_left";
    SDL_Texture* tex = tm_->get(texName);
    if (!tex) return;

    SDL_FRect scaled = { rect.x * scaleX, rect.y * scaleY, rect.w * scaleX, rect.h * scaleY };
    SDL_RenderTexture(r, tex, nullptr, &scaled);
}