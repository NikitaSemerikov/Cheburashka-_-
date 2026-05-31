#include "Boss.h"
#include "TextureManager.h"
#include "Config.h"

Boss::Boss(TextureManager* tm, float x, float y)
    : tm_(tm) {
    rect = { x, y, 96, 96 };
}

void Boss::render(SDL_Renderer* r, int screenWidth, int screenHeight) {
    float scaleX = (float)screenWidth / WINDOW_WIDTH;
    float scaleY = (float)screenHeight / WINDOW_HEIGHT;

    SDL_FRect scaled = { rect.x * scaleX, rect.y * scaleY, rect.w * scaleX, rect.h * scaleY };
    SDL_RenderTexture(r, tm_->get("shapoklyak"), nullptr, &scaled);
}