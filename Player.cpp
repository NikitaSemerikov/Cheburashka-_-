#include "Player.h"
#include "TextureManager.h"
#include <iostream>
#include <cmath>

Player::Player(TextureManager* tm) : tm_(tm) {
    rect = { 100.0f, 550.0f, 80.0f, 64.0f };
}

void Player::handleInput(const bool* keys, float dt) {
    bool left = keys[SDL_SCANCODE_A];
    bool right = keys[SDL_SCANCODE_D];
    bool jump = keys[SDL_SCANCODE_SPACE];

    if (left) {
        velocity.x = -speed_;
        if (onGround_) state_ = State::WalkingLeft;
        else state_ = State::JumpingLeft;
    }
    else if (right) {
        velocity.x = speed_;
        if (onGround_) state_ = State::WalkingRight;
        else state_ = State::JumpingRight;
    }
    else {
        velocity.x = 0;
        if (onGround_) state_ = State::Idle;
    }

    if (jump && onGround_) {
        velocity.y = jumpVelocity_;
        onGround_ = false;
        if (left) state_ = State::JumpingLeft;
        else state_ = State::JumpingRight;
    }
}

void Player::applyGravity(float dt) {
    velocity.y += 1500.0f * dt;
}

void Player::update(float dt) {
   
    if (state_ == State::WalkingLeft || state_ == State::WalkingRight) {
        animTimer_ += dt;
        if (animTimer_ >= frameDuration_) {
            animTimer_ = 0.0f;
            walkFrame_ = (walkFrame_ + 1) % walkFrameCount_;
        }
    }
    else {
        animTimer_ = 0.0f;
        walkFrame_ = 0;
    }
}

void Player::render(SDL_Renderer* r, int screenWidth, int screenHeight) {
    float scaleX = (float)screenWidth / WINDOW_WIDTH;
    float scaleY = (float)screenHeight / WINDOW_HEIGHT;

    SDL_Texture* tex = nullptr;
    std::string texName;

    switch (state_) {
    case State::Idle: texName = "cheb_idle"; break;
    case State::WalkingRight: texName = "cheb_walk_right" + std::to_string(walkFrame_ + 1); break;
    case State::WalkingLeft: texName = "cheb_walk_left" + std::to_string(walkFrame_ + 1); break;
    case State::JumpingRight: texName = "cheb_jump_right"; break;
    case State::JumpingLeft: texName = "cheb_jump_left"; break;
    }

    tex = tm_->get(texName);
    if (!tex) return;

    float visualW = 64.0f;
    float visualH = 64.0f;
    float visualX = rect.x + (rect.w - visualW) / 2.0f;
    float visualY = rect.y + (rect.h - visualH);

    SDL_FRect visualRect = {
        visualX * scaleX,
        visualY * scaleY,
        visualW * scaleX,
        visualH * scaleY
    };
    SDL_RenderTexture(r, tex, nullptr, &visualRect);
}

bool Player::consumeOrange() {
    if (oranges_ > 0) {
        --oranges_;
        return true;
    }
    return false;
}