#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Config.h"
#include "TextureManager.h"
#include "Menu.h"

class Player;
class Level;
class Level1;
class Level2;

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();

    enum class GameState {
        Menu,
        Playing,
        GameOver,
        Victory
    };

    void startLevelMusic();
    void stopLevelMusic();

    TTF_Font* getFont() const { return font_; }

private:
    SDL_Window* window_{ nullptr };
    SDL_Renderer* renderer_{ nullptr };

    TextureManager textures_;
    std::unique_ptr<Player> player_;
    std::unique_ptr<Level1> level1_;
    std::unique_ptr<Level2> level2_;
    Level* currentLevel_{ nullptr };
    LevelId currentId_{ LevelId::Level1 };

    std::unique_ptr<Menu> menu_;
    GameState state_ = GameState::Menu;

    struct EndMenu {
        SDL_FRect restartButton;
        SDL_FRect quitButton;
        bool restartHovered;
        bool quitHovered;
        SDL_Texture* restartTexture;
        SDL_Texture* quitTexture;
        SDL_Texture* titleTexture;
    };
    EndMenu endMenu_;
    bool endMenuInitialized_{ false };

    TTF_Font* font_{ nullptr };

    SDL_Texture* heartTexture_{ nullptr };
    SDL_Texture* orangeTexture_{ nullptr };
    SDL_Texture* bossHeartTexture_{ nullptr };

    MIX_Mixer* levelMixer_{ nullptr };
    MIX_Audio* levelMusic_{ nullptr };
    MIX_Track* levelMusicTrack_{ nullptr };
    bool levelMusicPlaying_{ false };

    bool running_{ false };

    void handleEvents();
    void update(float dt);
    void render();
    void renderHUD(int screenWidth, int screenHeight);
    void renderEndMenu(int screenWidth, int screenHeight);
    void switchLevel(LevelId id);
    void startLevel(LevelId id);
    void cleanup();
    void handleShooting();
    void loadHUDTextures();
    void loadEndMenuTextures();
    void renderText(const std::string& text, float x, float y, SDL_Color color, float scaleX, float scaleY);
    void handleEndMenuEvents(const SDL_Event& e);
};