#pragma once
#include "Entity.h"
#include "Config.h"

class TextureManager;

class Player : public Entity {
public:
    Player(TextureManager* tm);

    void handleInput(const bool* keys, float dt);
    void applyGravity(float dt);
    void resolveGround(float y);
    void setGround(bool ground) { onGround_ = ground; }

    void update(float dt) override;
    void render(SDL_Renderer* r, int screenWidth, int screenHeight) override;

    int getHealth() const { return health_; }
    void damage() { if (health_ > 0) --health_; }
    void resetHealth() { health_ = 9; oranges_ = 0; } 

    int getOrangeCount() const { return oranges_; }
    void addOrange() { ++oranges_; }
    bool consumeOrange();

    void setPosition(float x, float y) { rect.x = x; rect.y = y; }

    bool onGround_{ false };

private:
    TextureManager* tm_;

    int health_{ 100 };
    int oranges_{ 0 };

    float speed_ = 400.0f;
    float jumpVelocity_ = -550.0f;

    enum class State {
        Idle, WalkingLeft, WalkingRight, JumpingLeft, JumpingRight
    };
    State state_ = State::Idle;
    float animTimer_ = 0.0f;
    int walkFrame_ = 0;
    int walkFrameCount_ = 4;
    float frameDuration_ = 0.12f;
};