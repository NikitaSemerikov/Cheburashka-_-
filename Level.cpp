#include "Level.h"
#include "Player.h"
#include "Orange.h"
#include "Rat.h"
#include "Boss.h"
#include "Projectile.h"
#include "TextureManager.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

static bool intersects(const SDL_FRect& a, const SDL_FRect& b) {
    return !(a.x + a.w <= b.x || b.x + b.w <= a.x ||
        a.y + a.h <= b.y || b.y + b.h <= a.y);
}


Level1::Level1(Player* player, TextureManager* tm)
    : player_(player), tm_(tm) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    reset();
}

void Level1::reset() {
    platforms_.clear();
    createLevel();
    next_ = LevelId::Level1;
}

void Level1::createLevel() {
    platforms_.push_back({ 0, 650, (float)WINDOW_WIDTH, 30 });

    platforms_.push_back({ 150, 580, 150, 20 });
    platforms_.push_back({ 400, 500, 120, 20 });
    platforms_.push_back({ 900, 495, 120, 20 });
    platforms_.push_back({ 1050, 400, 120, 20 });

    movingPlatform_.rect = { 550, 500, 100, 20 };
    movingPlatform_.velocity = { 100, 0 };
    movingPlatform_.leftBound = 530;
    movingPlatform_.rightBound = 720;
    movingPlatform_.vertical = false;

    orangeCrate_ = { 1080, 270, 150, 150 };
}

void Level1::update(float dt) {
    movingPlatform_.rect.x += movingPlatform_.velocity.x * dt;
    if (movingPlatform_.rect.x <= movingPlatform_.leftBound) {
        movingPlatform_.rect.x = movingPlatform_.leftBound;
        movingPlatform_.velocity.x = -movingPlatform_.velocity.x;
    }
    else if (movingPlatform_.rect.x >= movingPlatform_.rightBound) {
        movingPlatform_.rect.x = movingPlatform_.rightBound;
        movingPlatform_.velocity.x = -movingPlatform_.velocity.x;
    }

    player_->applyGravity(dt);

    float newX = player_->rect.x + player_->velocity.x * dt;
    SDL_FRect newRectX = player_->rect;
    newRectX.x = newX;

    for (const auto& p : platforms_) {
        if (intersects(newRectX, p)) {
            if (player_->velocity.x > 0) newX = p.x - player_->rect.w;
            else if (player_->velocity.x < 0) newX = p.x + p.w;
            break;
        }
    }
    if (intersects(newRectX, movingPlatform_.rect)) {
        if (player_->velocity.x > 0) newX = movingPlatform_.rect.x - player_->rect.w;
        else if (player_->velocity.x < 0) newX = movingPlatform_.rect.x + movingPlatform_.rect.w;
    }
    player_->rect.x = newX;

    float newY = player_->rect.y + player_->velocity.y * dt;
    SDL_FRect newRectY = player_->rect;
    newRectY.y = newY;

    player_->onGround_ = false;

    for (const auto& p : platforms_) {
        if (intersects(newRectY, p)) {
            if (player_->velocity.y > 0) {
                newY = p.y - player_->rect.h;
                player_->velocity.y = 0;
                player_->onGround_ = true;
            }
            else if (player_->velocity.y < 0) {
                newY = p.y + p.h;
                player_->velocity.y = 0;
            }
            break;
        }
    }

    if (intersects(newRectY, movingPlatform_.rect)) {
        if (player_->velocity.y > 0) {
            newY = movingPlatform_.rect.y - player_->rect.h;
            player_->velocity.y = 0;
            player_->onGround_ = true;
            player_->rect.x += movingPlatform_.velocity.x * dt;
        }
        else if (player_->velocity.y < 0) {
            newY = movingPlatform_.rect.y + movingPlatform_.rect.h;
            player_->velocity.y = 0;
        }
    }

    player_->rect.y = newY;

   
    if (player_->rect.x < 0) player_->rect.x = 0;
    if (player_->rect.x + player_->rect.w > WINDOW_WIDTH) {
        player_->rect.x = WINDOW_WIDTH - player_->rect.w;
    }

    if (player_->rect.y + player_->rect.h > WINDOW_HEIGHT + 100) {
        player_->setPosition(100, 550);
        player_->velocity = { 0, 0 };
        player_->onGround_ = true;
    }


    if (player_->rect.y + player_->rect.h > 650 && player_->rect.y + player_->rect.h <= 660) {
        player_->rect.y = 650 - player_->rect.h;
        player_->velocity.y = 0;
        player_->onGround_ = true;
    }

 
    if (intersects(player_->rect, orangeCrate_)) {
        next_ = LevelId::Level2;
    }

    player_->update(dt);
}

void Level1::render(SDL_Renderer* r, int screenWidth, int screenHeight) {
    float scaleX = (float)screenWidth / WINDOW_WIDTH;
    float scaleY = (float)screenHeight / WINDOW_HEIGHT;


    SDL_SetRenderDrawColor(r, 139, 90, 43, 255);
    for (const auto& p : platforms_) {
        
        if (p.y == 650) continue;

        SDL_FRect scaled = { p.x * scaleX, p.y * scaleY, p.w * scaleX, p.h * scaleY };
        SDL_RenderFillRect(r, &scaled);
        SDL_SetRenderDrawColor(r, 200, 150, 100, 255);
        SDL_RenderRect(r, &scaled);
        SDL_SetRenderDrawColor(r, 139, 90, 43, 255);
    }


    SDL_SetRenderDrawColor(r, 70, 130, 200, 255);
    SDL_FRect mpScaled = {
        movingPlatform_.rect.x * scaleX,
        movingPlatform_.rect.y * scaleY,
        movingPlatform_.rect.w * scaleX,
        movingPlatform_.rect.h * scaleY
    };
    SDL_RenderFillRect(r, &mpScaled);
    SDL_SetRenderDrawColor(r, 100, 180, 250, 255);
    SDL_RenderRect(r, &mpScaled);

 
    SDL_Texture* crateTex = tm_->get("orange_crate");
    if (crateTex) {
        SDL_FRect crateScaled = {
            orangeCrate_.x * scaleX,
            orangeCrate_.y * scaleY,
            orangeCrate_.w * scaleX,
            orangeCrate_.h * scaleY
        };
        SDL_RenderTexture(r, crateTex, nullptr, &crateScaled);
    }
}



Level2::Level2(Player* player, TextureManager* tm)
    : player_(player), tm_(tm) {
    reset();
}

void Level2::reset() {
 
    platforms_.clear();
    platforms_.push_back({ 0, 650, 1280, 70 });

    oranges_.clear();
    rats_.clear();
    projectiles_.clear();
    boss_.reset();

    orangeTimer_ = 0;
    ratTimer_ = 0;
    bossDelay_ = 2;
    bossSpawned_ = false;
    next_ = LevelId::Level2;
}

void Level2::spawnOrange() {
    float x = static_cast<float>(rand() % (WINDOW_WIDTH - 32));
    oranges_.push_back(std::make_unique<Orange>(tm_, x, -32));
}

void Level2::spawnRat() {
    bool left = rand() % 2;
    float x = left ? -50.0f : WINDOW_WIDTH + 50.0f;
    float vx = left ? 120.0f : -120.0f;
    rats_.push_back(std::make_unique<Rat>(tm_, x, 650.0f - 32.0f, vx));
}

int Level2::getBossHits() const {
    return boss_ ? boss_->getHits() : 0;
}

void Level2::addProjectile(std::unique_ptr<Projectile> p) {
    projectiles_.push_back(std::move(p));
}

void Level2::update(float dt) {

    player_->applyGravity(dt);

    
    float newX = player_->rect.x + player_->velocity.x * dt;
    SDL_FRect newRectX = player_->rect;
    newRectX.x = newX;

    for (const auto& p : platforms_) {
        if (intersects(newRectX, p)) {
            if (player_->velocity.x > 0) newX = p.x - player_->rect.w;
            else if (player_->velocity.x < 0) newX = p.x + p.w;
            break;
        }
    }
    player_->rect.x = newX;

    float newY = player_->rect.y + player_->velocity.y * dt;
    SDL_FRect newRectY = player_->rect;
    newRectY.y = newY;

    player_->onGround_ = false;
    for (const auto& p : platforms_) {
        if (intersects(newRectY, p)) {
            if (player_->velocity.y > 0) {
                newY = p.y - player_->rect.h;
                player_->velocity.y = 0;
                player_->onGround_ = true;
            }
            else if (player_->velocity.y < 0) {
                newY = p.y + p.h;
                player_->velocity.y = 0;
            }
            break;
        }
    }
    player_->rect.y = newY;

  
    if (player_->rect.x < 0) player_->rect.x = 0;
    if (player_->rect.x + player_->rect.w > WINDOW_WIDTH) {
        player_->rect.x = WINDOW_WIDTH - player_->rect.w;
    }

 
    if (player_->rect.y + player_->rect.h > WINDOW_HEIGHT + 100) {
        player_->setPosition(100, 550);
        player_->velocity = { 0, 0 };
        player_->onGround_ = true;
    }

    
    orangeTimer_ -= dt;
    if (orangeTimer_ <= 0) {
        spawnOrange();
        orangeTimer_ = 1.0f + (rand() % 100) / 100.0f;
    }

   
    ratTimer_ -= dt;
    if (ratTimer_ <= 0) {
        spawnRat();
        ratTimer_ = 1.5f + (rand() % 150) / 100.0f;
    }

   
    if (!bossSpawned_) {
        bossDelay_ -= dt;
        if (bossDelay_ <= 0) {
            boss_ = std::make_unique<Boss>(tm_, 1100.0f, 550.0f);
            bossSpawned_ = true;
        }
    }

   
    for (auto& o : oranges_) o->update(dt);
    for (auto& r : rats_) r->update(dt);
    for (auto& p : projectiles_) p->update(dt);

    
    for (auto& o : oranges_) {
        if (o->alive && intersects(player_->rect, o->rect)) {
            o->alive = false;
            player_->addOrange();
        }
    }

    for (auto& r : rats_) {
        if (r->alive && intersects(player_->rect, r->rect)) {
            r->alive = false;
            player_->damage();
            if (player_->getHealth() <= 0) next_ = LevelId::GameOver;
        }
    }

  
    for (auto& p : projectiles_) {
        if (!p->alive) continue;
        for (auto& r : rats_) {
            if (r->alive && intersects(p->rect, r->rect)) {
                r->alive = false;
                p->alive = false;
            }
        }
    }

    
    if (boss_) {
        for (auto& p : projectiles_) {
            if (p->alive && intersects(p->rect, boss_->rect)) {
                boss_->hit();
                p->alive = false;
            }
        }
        if (boss_->getHits() >= 3) next_ = LevelId::Victory;
    }

    oranges_.erase(std::remove_if(oranges_.begin(), oranges_.end(),
        [](auto& o) { return !o->alive || o->rect.y > WINDOW_HEIGHT; }),
        oranges_.end());
    rats_.erase(std::remove_if(rats_.begin(), rats_.end(),
        [](auto& r) { return !r->alive; }),
        rats_.end());
    projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(),
        [](auto& p) { return !p->alive; }),
        projectiles_.end());

    player_->update(dt);
}

void Level2::render(SDL_Renderer* r, int screenWidth, int screenHeight) {
    float scaleX = (float)screenWidth / WINDOW_WIDTH;
    float scaleY = (float)screenHeight / WINDOW_HEIGHT;

    
    for (auto& o : oranges_) o->render(r, screenWidth, screenHeight);
    for (auto& rt : rats_) rt->render(r, screenWidth, screenHeight);
    for (auto& p : projectiles_) p->render(r, screenWidth, screenHeight);
    if (boss_) boss_->render(r, screenWidth, screenHeight);
}