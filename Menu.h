#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "Config.h"
#include "TextureManager.h"

class Menu {
public:
    Menu(TextureManager* tm, SDL_Renderer* renderer);
    ~Menu();

    void handleEvent(const SDL_Event& e);
    void update(float dt);
    void render();

    enum class Result {
        Running,
        StartGame,
        Quit,
        SelectLevel1,
        SelectLevel2,
        BackToMain
    };

    Result getResult() const { return result_; }
    void reset() { result_ = Result::Running; }

    void setMainMenu() { currentScreen_ = Screen::Main; }
    void setLevelSelect() { currentScreen_ = Screen::LevelSelect; }

    void startMusic();
    void stopMusic();

private:
    enum class Screen {
        Main,
        LevelSelect
    };

    Screen currentScreen_ = Screen::Main;

    TextureManager* tm_;
    SDL_Renderer* renderer_;
    Result result_ = Result::Running;

    struct Button {
        SDL_FRect rect;
        std::string text;
        bool hovered;
        SDL_Texture* textTexture;
    };

    Button startButton_;
    Button levelSelectButton_;
    Button quitButton_;

    Button level1Button_;
    Button level2Button_;
    Button backButton_;

    TTF_Font* font_{ nullptr };
    SDL_Texture* bgTexture_{ nullptr };

    MIX_Mixer* mixer_{ nullptr };
    MIX_Audio* menuMusic_{ nullptr };
    MIX_Track* musicTrack_{ nullptr };
    bool musicPlaying_{ false };

    void updateButtonHover(int mouseX, int mouseY);
    void renderButton(const Button& button);
    SDL_Texture* renderTextToTexture(const std::string& text, SDL_Color color);
    void createMainMenuButtons();
    void createLevelSelectButtons();
    void cleanup();
};