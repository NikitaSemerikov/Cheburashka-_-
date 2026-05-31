#include "Projectile.h"
#include "TextureManager.h"
#include "Config.h"

Projectile::Projectile(TextureManager* tm, float x, float y, float vx)
    : tm_(tm) {
    rect = { x, y, 24, 24 };
    velocity.x = vx;
}

void Projectile::update(float dt) {
    rect.x += velocity.x * dt;
    if (rect.x < -50 || rect.x > WINDOW_WIDTH + 50)
        alive = false;
}

void Projectile::render(SDL_Renderer* r, int screenWidth, int screenHeight) {
    float scaleX = (float)screenWidth / WINDOW_WIDTH;
    float scaleY = (float)screenHeight / WINDOW_HEIGHT;

    SDL_FRect scaled = { rect.x * scaleX, rect.y * scaleY, rect.w * scaleX, rect.h * scaleY };
    SDL_RenderTexture(r, tm_->get("orange"), nullptr, &scaled);
}