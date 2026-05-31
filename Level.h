#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include "Config.h"

class Player;
class Orange;
class Rat;
class Boss;
class Projectile;
class TextureManager;

struct MovingPlatform {
    SDL_FRect rect;
    SDL_FPoint velocity;
    float leftBound;
    float rightBound;
    bool vertical;
};

class Level {
public:
    virtual ~Level() = default;
    virtual void reset() = 0;
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* r, int screenWidth, int screenHeight) = 0;
    virtual LevelId getId() const = 0;
    virtual LevelId nextLevel() const = 0;
};

class Level1 : public Level {
public:
    Level1(Player* player, TextureManager* tm);
    void reset() override;
    void update(float dt) override;
    void render(SDL_Renderer* r, int screenWidth, int screenHeight) override;
    LevelId getId() const override { return LevelId::Level1; }
    LevelId nextLevel() const override { return next_; }

private:
    Player* player_;
    TextureManager* tm_;
    std::vector<SDL_FRect> platforms_;
    MovingPlatform movingPlatform_;  
    SDL_FRect orangeCrate_{};
    LevelId next_{ LevelId::Level1 };
    void createLevel();
    void handleCollision(SDL_FRect& entityRect, SDL_FPoint& velocity, bool& onGround, float dt);
};

class Level2 : public Level {
public:
    Level2(Player* player, TextureManager* tm);
    void reset() override;
    void update(float dt) override;
    void render(SDL_Renderer* r, int screenWidth, int screenHeight) override;
    LevelId getId() const override { return LevelId::Level2; }
    LevelId nextLevel() const override { return next_; }
    void addProjectile(std::unique_ptr<Projectile> p);
    int getBossHits() const;

private:
    Player* player_;
    TextureManager* tm_;
    std::vector<SDL_FRect> platforms_;
    std::vector<std::unique_ptr<Orange>> oranges_;
    std::vector<std::unique_ptr<Rat>> rats_;
    std::vector<std::unique_ptr<Projectile>> projectiles_;
    std::unique_ptr<Boss> boss_;
    float orangeTimer_{ 0 };
    float ratTimer_{ 0 };
    float bossDelay_{ 2 };
    bool bossSpawned_{ false };
    LevelId next_{ LevelId::Level2 };
    void spawnOrange();
    void spawnRat();
};