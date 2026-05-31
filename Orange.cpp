#include "Orange.h"
#include "TextureManager.h"
#include "Config.h"

Orange::Orange(TextureManager* tm, float x, float y)
    : tm_(tm) {
    rect = { x, y, 32, 32 };
}

void Orange::update(float dt) {
    velocity.y += 1500 * dt;
    rect.y += velocity.y * dt;
}

void Orange::render(SDL_Renderer* r, int screenWidth, int screenHeight) {
    float scaleX = (float)screenWidth / WINDOW_WIDTH;
    float scaleY = (float)screenHeight / WINDOW_HEIGHT;

    SDL_FRect scaled = { rect.x * scaleX, rect.y * scaleY, rect.w * scaleX, rect.h * scaleY };
    SDL_RenderTexture(r, tm_->get("orange"), nullptr, &scaled);
}