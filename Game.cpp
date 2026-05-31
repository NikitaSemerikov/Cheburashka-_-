#include "Game.h"
#include "Player.h"
#include "Level.h"
#include "Projectile.h"
#include <iostream>
#include <chrono>

Game::Game() = default;

Game::~Game() {
    cleanup();
}

bool Game::init() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    if (!MIX_Init()) {
        std::cerr << "MIX_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    window_ = SDL_CreateWindow(
        "Cheburashka Platformer",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    if (!window_) {
        std::cerr << "CreateWindow error: " << SDL_GetError() << "\n";
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (!renderer_) {
        std::cerr << "CreateRenderer error: " << SDL_GetError() << "\n";
        return false;
    }

    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    font_ = TTF_OpenFont("assets/font.ttf", 24);
    if (!font_) {
        std::cerr << "Failed to load font for HUD: " << SDL_GetError() << std::endl;
    }

    levelMixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!levelMixer_) {
        std::cerr << "Failed to create level mixer device: " << SDL_GetError() << std::endl;
    }

    auto loadTex = [&](const std::string& id, const std::string& path) {
        SDL_Surface* surf = IMG_Load(path.c_str());
        if (!surf) {
            std::cerr << "IMG_Load error for " << path << ": " << SDL_GetError() << "\n";
            return;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
        SDL_DestroySurface(surf);
        if (!tex) {
            std::cerr << "CreateTextureFromSurface error: " << SDL_GetError() << "\n";
            return;
        }
        textures_.add(id, tex);
        };

    loadTex("cheb_idle", "assets/cheb.png");
    loadTex("cheb_jump_right", "assets/Jump_right.png");
    loadTex("cheb_jump_left", "assets/Jump_left.png");
    loadTex("cheb_walk_right1", "assets/Walk_right_1.png");
    loadTex("cheb_walk_right2", "assets/Walk_right_2.png");
    loadTex("cheb_walk_right3", "assets/Walk_right_3.png");
    loadTex("cheb_walk_right4", "assets/Walk_right_4.png");
    loadTex("cheb_walk_left1", "assets/Walk_left_1.png");
    loadTex("cheb_walk_left2", "assets/Walk_left_2.png");
    loadTex("cheb_walk_left3", "assets/Walk_left_3.png");
    loadTex("cheb_walk_left4", "assets/Walk_left_4.png");
    loadTex("orange", "assets/orange.png");
    loadTex("orange_crate", "assets/croco.png");
    loadTex("shapoklyak", "assets/shapok.png");
    loadTex("bg1", "assets/fon.png");
    loadTex("bg2", "assets/fon.png");
    loadTex("rat_right", "assets/rat_right.png");
    loadTex("rat_left", "assets/rat_left.png");

    loadHUDTextures();
    loadEndMenuTextures();

    player_ = std::make_unique<Player>(&textures_);
    level1_ = std::make_unique<Level1>(player_.get(), &textures_);
    level2_ = std::make_unique<Level2>(player_.get(), &textures_);

    currentLevel_ = level1_.get();
    currentId_ = LevelId::Level1;

    menu_ = std::make_unique<Menu>(&textures_, renderer_);

    running_ = true;
    return true;
}

void Game::loadEndMenuTextures() {
    endMenu_.restartHovered = false;
    endMenu_.quitHovered = false;

    int btnW = 250;
    int btnH = 60;
    int centerX = (WINDOW_WIDTH - btnW) / 2;

    endMenu_.restartButton = { (float)centerX, 350.0f, (float)btnW, (float)btnH };
    endMenu_.quitButton = { (float)centerX, 420.0f, (float)btnW, (float)btnH };

}

void Game::loadHUDTextures() {
    SDL_Surface* heartSurf = IMG_Load("assets/heart.png");
    if (heartSurf) {
        heartTexture_ = SDL_CreateTextureFromSurface(renderer_, heartSurf);
        SDL_DestroySurface(heartSurf);
    }
    else {
        std::cerr << "Failed to load heart.png" << std::endl;
    }

    SDL_Surface* orangeSurf = IMG_Load("assets/orange_hud.png");
    if (orangeSurf) {
        orangeTexture_ = SDL_CreateTextureFromSurface(renderer_, orangeSurf);
        SDL_DestroySurface(orangeSurf);
    }
    else {
        std::cerr << "Failed to load orange_hud.png" << std::endl;
    }

    SDL_Surface* bossHeartSurf = IMG_Load("assets/boss_heart.png");
    if (bossHeartSurf) {
        bossHeartTexture_ = SDL_CreateTextureFromSurface(renderer_, bossHeartSurf);
        SDL_DestroySurface(bossHeartSurf);
    }
    else {
        std::cerr << "Failed to load boss_heart.png" << std::endl;
    }
}

void Game::renderText(const std::string& text, float x, float y, SDL_Color color, float scaleX, float scaleY) {
    if (!font_) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font_, text.c_str(), (int)text.length(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    if (texture) {
        float texW, texH;
        SDL_GetTextureSize(texture, &texW, &texH);
        SDL_FRect dstRect = { x, y, texW * scaleX, texH * scaleY };
        SDL_RenderTexture(renderer_, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}

void Game::run() {
    using clock = std::chrono::high_resolution_clock;
    auto last = clock::now();

    while (running_) {
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        if (dt > 0.033f) dt = 0.033f;
        last = now;

        handleEvents();
        update(dt);
        render();
    }
}

void Game::startLevelMusic() {
    if (levelMixer_ && !levelMusicPlaying_) {
        std::string musicFile;
        if (currentId_ == LevelId::Level1) {
            musicFile = "assets/game.wav";
        }
        else if (currentId_ == LevelId::Level2) {
            musicFile = "assets/level2.wav";
        }
        else {
            return;
        }

        levelMusic_ = MIX_LoadAudio(levelMixer_, musicFile.c_str(), true);
        if (!levelMusic_) {
            std::cerr << "Failed to load level music from " << musicFile << ": " << SDL_GetError() << std::endl;
            return;
        }

        levelMusicTrack_ = MIX_CreateTrack(levelMixer_);
        if (levelMusicTrack_) {
            MIX_SetTrackAudio(levelMusicTrack_, levelMusic_);
            if (MIX_PlayTrack(levelMusicTrack_, -1)) {
                levelMusicPlaying_ = true;
                std::cout << "Level music started: " << musicFile << std::endl;
            }
            else {
                std::cerr << "Failed to play level music: " << SDL_GetError() << std::endl;
            }
        }
    }
}

void Game::stopLevelMusic() {
    if (levelMusicTrack_ && levelMusicPlaying_) {
        MIX_StopTrack(levelMusicTrack_, 0);
        levelMusicPlaying_ = false;
        std::cout << "Level music stopped" << std::endl;
    }

    if (levelMusicTrack_) {
        MIX_DestroyTrack(levelMusicTrack_);
        levelMusicTrack_ = nullptr;
    }
    if (levelMusic_) {
        MIX_DestroyAudio(levelMusic_);
        levelMusic_ = nullptr;
    }
}

void Game::handleEndMenuEvents(const SDL_Event& e) {
    if (e.type == SDL_EVENT_MOUSE_MOTION) {
        int mx = e.motion.x;
        int my = e.motion.y;

        endMenu_.restartHovered = (mx >= endMenu_.restartButton.x && mx <= endMenu_.restartButton.x + endMenu_.restartButton.w &&
            my >= endMenu_.restartButton.y && my <= endMenu_.restartButton.y + endMenu_.restartButton.h);

        endMenu_.quitHovered = (mx >= endMenu_.quitButton.x && mx <= endMenu_.quitButton.x + endMenu_.quitButton.w &&
            my >= endMenu_.quitButton.y && my <= endMenu_.quitButton.y + endMenu_.quitButton.h);
    }
    else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (endMenu_.restartHovered) {
            state_ = GameState::Menu;
            menu_->setMainMenu();
            menu_->startMusic();
            stopLevelMusic();
            startLevel(LevelId::Level1);
        }
        else if (endMenu_.quitHovered) {
            running_ = false;
        }
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            running_ = false;
        }

        if (state_ == GameState::Menu) {
            menu_->handleEvent(e);
            if (menu_->getResult() == Menu::Result::StartGame) {
                state_ = GameState::Playing;
                startLevel(LevelId::Level1);
                menu_->reset();
                menu_->setMainMenu();
                menu_->stopMusic();
                startLevelMusic();
            }
            else if (menu_->getResult() == Menu::Result::SelectLevel1) {
                state_ = GameState::Playing;
                startLevel(LevelId::Level1);
                menu_->reset();
                menu_->setMainMenu();
                menu_->stopMusic();
                startLevelMusic();
            }
            else if (menu_->getResult() == Menu::Result::SelectLevel2) {
                state_ = GameState::Playing;
                startLevel(LevelId::Level2);
                menu_->reset();
                menu_->setMainMenu();
                menu_->stopMusic();
                startLevelMusic();
            }
            else if (menu_->getResult() == Menu::Result::Quit) {
                running_ = false;
            }
        }
        else if (state_ == GameState::Playing) {
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    state_ = GameState::Menu;
                    menu_->setMainMenu();
                    menu_->startMusic();
                    stopLevelMusic();
                }
                if (e.key.key == SDLK_F) {
                    handleShooting();
                }
            }
        }
        else if (state_ == GameState::GameOver || state_ == GameState::Victory) {
            handleEndMenuEvents(e);
        }
    }
}

void Game::handleShooting() {
    if (!player_->consumeOrange())
        return;

    float dir = (player_->velocity.x >= 0 ? 1.f : -1.f);
    float px = player_->rect.x + player_->rect.w / 2;
    float py = player_->rect.y + player_->rect.h / 2;

    if (currentId_ == LevelId::Level2) {
        Level2* lvl2 = dynamic_cast<Level2*>(currentLevel_);
        if (lvl2) {
            lvl2->addProjectile(std::make_unique<Projectile>(&textures_, px, py, dir * 450.f));
        }
    }
}

void Game::update(float dt) {
    if (state_ == GameState::Menu) {
        menu_->update(dt);
    }
    else if (state_ == GameState::Playing) {
        const bool* keys = SDL_GetKeyboardState(nullptr);
        player_->handleInput(keys, dt);
        currentLevel_->update(dt);

        LevelId next = currentLevel_->nextLevel();
        if (next != currentId_) {
            switchLevel(next);
        }

        if (currentId_ == LevelId::GameOver) {
            state_ = GameState::GameOver;
            stopLevelMusic();
        }
        else if (currentId_ == LevelId::Victory) {
            state_ = GameState::Victory;
            stopLevelMusic();
        }
    }
}

void Game::renderEndMenu(int screenWidth, int screenHeight) {
    float scaleX = (float)screenWidth / WINDOW_WIDTH;
    float scaleY = (float)screenHeight / WINDOW_HEIGHT;

    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 200);
    SDL_FRect bgRect = { 0, 0, (float)screenWidth, (float)screenHeight };
    SDL_RenderFillRect(renderer_, &bgRect);

    SDL_Color titleColor;
    std::string titleText;
    if (state_ == GameState::GameOver) {
        titleText = "GAME OVER";
        titleColor = { 255, 80, 80, 255 };
    }
    else {
        titleText = "VICTORY!";
        titleColor = { 80, 255, 80, 255 };
    }

    if (font_) {
        TTF_SetFontSize(font_, 48);
        SDL_Surface* titleSurf = TTF_RenderText_Blended(font_, titleText.c_str(), (int)titleText.length(), titleColor);
        TTF_SetFontSize(font_, 24);
        if (titleSurf) {
            SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer_, titleSurf);
            float texW, texH;
            SDL_GetTextureSize(titleTex, &texW, &texH);
            SDL_FRect titleRect = { ((float)screenWidth - texW * scaleX) / 2, 150 * scaleY, texW * scaleX, texH * scaleY };
            SDL_RenderTexture(renderer_, titleTex, nullptr, &titleRect);
            SDL_DestroyTexture(titleTex);
            SDL_DestroySurface(titleSurf);
        }
    }

    SDL_Color btnColor = endMenu_.restartHovered ?
        SDL_Color{ 80, 180, 80, 255 } : SDL_Color{ 40, 120, 40, 255 };
    SDL_SetRenderDrawColor(renderer_, btnColor.r, btnColor.g, btnColor.b, btnColor.a);
    SDL_RenderFillRect(renderer_, &endMenu_.restartButton);
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderRect(renderer_, &endMenu_.restartButton);

    btnColor = endMenu_.quitHovered ?
        SDL_Color{ 180, 80, 80, 255 } : SDL_Color{ 120, 40, 40, 255 };
    SDL_SetRenderDrawColor(renderer_, btnColor.r, btnColor.g, btnColor.b, btnColor.a);
    SDL_RenderFillRect(renderer_, &endMenu_.quitButton);
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderRect(renderer_, &endMenu_.quitButton);

    if (font_) {
        renderText("RESTART   ", endMenu_.restartButton.x + (endMenu_.restartButton.w - 150 * scaleX) / 2,
            endMenu_.restartButton.y + (endMenu_.restartButton.h - 20 * scaleY) / 2,
            { 255, 255, 255, 255 }, scaleX, scaleY);
        renderText("QUIT   ", endMenu_.quitButton.x + (endMenu_.quitButton.w - 100 * scaleX) / 2,
            endMenu_.quitButton.y + (endMenu_.quitButton.h - 20 * scaleY) / 2,
            { 255, 255, 255, 255 }, scaleX, scaleY);
    }
}

void Game::render() {
    if (state_ == GameState::Menu) {
        menu_->render();
    }
    else if (state_ == GameState::Playing) {
        int screenWidth, screenHeight;
        SDL_GetWindowSize(window_, &screenWidth, &screenHeight);

        if (currentId_ == LevelId::GameOver || currentId_ == LevelId::Victory) {
            SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
            SDL_RenderClear(renderer_);
            SDL_RenderPresent(renderer_);
            return;
        }

        SDL_Texture* bg = (currentId_ == LevelId::Level1) ? textures_.get("bg1") : textures_.get("bg2");

        if (bg) {
            SDL_FRect dst{ 0, 0, (float)screenWidth, (float)screenHeight };
            SDL_RenderTexture(renderer_, bg, nullptr, &dst);
        }
        else {
            SDL_SetRenderDrawColor(renderer_, 30, 30, 60, 255);
            SDL_RenderClear(renderer_);
        }

        currentLevel_->render(renderer_, screenWidth, screenHeight);
        player_->render(renderer_, screenWidth, screenHeight);
        renderHUD(screenWidth, screenHeight);
    }
    else if (state_ == GameState::GameOver || state_ == GameState::Victory) {
        int screenWidth, screenHeight;
        SDL_GetWindowSize(window_, &screenWidth, &screenHeight);
        renderEndMenu(screenWidth, screenHeight);
    }

    SDL_RenderPresent(renderer_);
}

void Game::renderHUD(int screenWidth, int screenHeight) {
    float scaleX = (float)screenWidth / WINDOW_WIDTH;
    float scaleY = (float)screenHeight / WINDOW_HEIGHT;

    float iconSize = 32.0f * scaleX;
    float spacing = 10.0f * scaleX;
    float startX = 20 * scaleX;
    float startY = 20 * scaleY;
    SDL_Color textColor = { 255, 255, 255, 255 };

    int hp = player_->getHealth();
    if (heartTexture_) {
        SDL_FRect heartRect = { startX, startY, iconSize, iconSize };
        SDL_RenderTexture(renderer_, heartTexture_, nullptr, &heartRect);
    }
    std::string hpText = "x " + std::to_string(hp);
    renderText(hpText, startX + iconSize + spacing, startY + (iconSize - 20 * scaleY) / 2, textColor, scaleX, scaleY);

    int oranges = player_->getOrangeCount();
    float orangeY = startY + iconSize + 15 * scaleY;
    if (orangeTexture_) {
        SDL_FRect orangeRect = { startX, orangeY, iconSize, iconSize };
        SDL_RenderTexture(renderer_, orangeTexture_, nullptr, &orangeRect);
    }
    std::string orangeText = "x " + std::to_string(oranges);
    renderText(orangeText, startX + iconSize + spacing, orangeY + (iconSize - 20 * scaleY) / 2, textColor, scaleX, scaleY);

    Level2* lvl2 = dynamic_cast<Level2*>(currentLevel_);
    if (lvl2) {
        int bossHits = lvl2->getBossHits();
        int bossHealth = 3 - bossHits;
        if (bossHealth < 0) bossHealth = 0;

        float bossY = orangeY + iconSize + 15 * scaleY;
        if (bossHeartTexture_) {
            SDL_FRect bossHeartRect = { startX, bossY, iconSize, iconSize };
            SDL_RenderTexture(renderer_, bossHeartTexture_, nullptr, &bossHeartRect);
        }
        std::string bossText = "x " + std::to_string(bossHealth);
        renderText(bossText, startX + iconSize + spacing, bossY + (iconSize - 20 * scaleY) / 2, textColor, scaleX, scaleY);
    }
}

void Game::switchLevel(LevelId id) {
    currentId_ = id;

    if (id == LevelId::Level1) {
        level1_->reset();
        currentLevel_ = level1_.get();
    }
    else if (id == LevelId::Level2) {
        level2_->reset();
        currentLevel_ = level2_.get();
    }

    if (state_ == GameState::Playing) {
        stopLevelMusic();
        startLevelMusic();
    }
}

void Game::startLevel(LevelId id) {
    player_->resetHealth();
    player_->setPosition(100, 550);
    player_->velocity = { 0, 0 };

    if (id == LevelId::Level1) {
        level1_->reset();
        currentLevel_ = level1_.get();
        currentId_ = LevelId::Level1;
    }
    else if (id == LevelId::Level2) {
        level2_->reset();
        currentLevel_ = level2_.get();
        currentId_ = LevelId::Level2;
    }
}

void Game::cleanup() {
    textures_.clear();
    menu_.reset();

    if (heartTexture_) SDL_DestroyTexture(heartTexture_);
    if (orangeTexture_) SDL_DestroyTexture(orangeTexture_);
    if (bossHeartTexture_) SDL_DestroyTexture(bossHeartTexture_);
    if (font_) TTF_CloseFont(font_);

    stopLevelMusic();
    if (levelMixer_) MIX_DestroyMixer(levelMixer_);

    MIX_Quit();
    TTF_Quit();

    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
    SDL_Quit();
}