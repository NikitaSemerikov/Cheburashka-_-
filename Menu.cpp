#include "Menu.h"
#include <iostream>

Menu::Menu(TextureManager* tm, SDL_Renderer* renderer)
    : tm_(tm), renderer_(renderer) {

 
    mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer_) {
        std::cerr << "Failed to create mixer device: " << SDL_GetError() << std::endl;
    }


    font_ = TTF_OpenFont("assets/font.ttf", 28);
    if (!font_) {
        std::cerr << "Failed to load font" << std::endl;
    }

  
    SDL_Surface* bgSurf = IMG_Load("assets/bg11.png");
    if (bgSurf) {
        bgTexture_ = SDL_CreateTextureFromSurface(renderer_, bgSurf);
        SDL_DestroySurface(bgSurf);
    }

   
    if (mixer_) {
        menuMusic_ = MIX_LoadAudio(mixer_, "assets/fonmusic.mp3", true);
        if (!menuMusic_) {
            std::cerr << "Failed to load menu music: " << SDL_GetError() << std::endl;
        }
        else {
           
            musicTrack_ = MIX_CreateTrack(mixer_);
            if (musicTrack_) {
                MIX_SetTrackAudio(musicTrack_, menuMusic_);
            }
            else {
                std::cerr << "Failed to create music track: " << SDL_GetError() << std::endl;
            }
        }
    }

    createMainMenuButtons();
    createLevelSelectButtons();


    startMusic();
}

Menu::~Menu() {
    cleanup();
}

void Menu::startMusic() {
    if (musicTrack_ && !musicPlaying_) {

        if (MIX_PlayTrack(musicTrack_, -1)) {
            musicPlaying_ = true;
            std::cout << "Menu music started" << std::endl;
        }
        else {
            std::cerr << "Failed to play music track: " << SDL_GetError() << std::endl;
        }
    }
}

void Menu::stopMusic() {
    if (musicTrack_ && musicPlaying_) {
        MIX_StopTrack(musicTrack_, 0);
        musicPlaying_ = false;
        std::cout << "Menu music stopped" << std::endl;
    }
}

void Menu::createMainMenuButtons() {
    int btnW = 350;
    int btnH = 55;
    int centerX = (WINDOW_WIDTH - btnW) / 2;

    startButton_.rect = { (float)centerX, 350.0f, (float)btnW, (float)btnH };
    startButton_.text = "START GAME";
    startButton_.hovered = false;
    startButton_.textTexture = renderTextToTexture(startButton_.text, { 255, 255, 255, 255 });

    levelSelectButton_.rect = { (float)centerX, 420.0f, (float)btnW, (float)btnH };
    levelSelectButton_.text = "LEVEL SELECT";
    levelSelectButton_.hovered = false;
    levelSelectButton_.textTexture = renderTextToTexture(levelSelectButton_.text, { 255, 255, 255, 255 });

    quitButton_.rect = { (float)centerX, 490.0f, (float)btnW, (float)btnH };
    quitButton_.text = "QUIT";
    quitButton_.hovered = false;
    quitButton_.textTexture = renderTextToTexture(quitButton_.text, { 255, 255, 255, 255 });
}

void Menu::createLevelSelectButtons() {
    int btnW = 250;
    int btnH = 55;
    int centerX = (WINDOW_WIDTH - btnW) / 2;

    level1Button_.rect = { (float)centerX, 350.0f, (float)btnW, (float)btnH };
    level1Button_.text = "LEVEL 1";
    level1Button_.hovered = false;
    level1Button_.textTexture = renderTextToTexture(level1Button_.text, { 255, 255, 255, 255 });

    level2Button_.rect = { (float)centerX, 420.0f, (float)btnW, (float)btnH };
    level2Button_.text = "LEVEL 2";
    level2Button_.hovered = false;
    level2Button_.textTexture = renderTextToTexture(level2Button_.text, { 255, 255, 255, 255 });

    backButton_.rect = { (float)centerX, 490.0f, (float)btnW, (float)btnH };
    backButton_.text = "BACK";
    backButton_.hovered = false;
    backButton_.textTexture = renderTextToTexture(backButton_.text, { 255, 255, 255, 255 });
}

SDL_Texture* Menu::renderTextToTexture(const std::string& text, SDL_Color color) {
    if (!font_) return nullptr;

    SDL_Surface* surface = TTF_RenderText_Blended(font_, text.c_str(), (int)text.length(), color);
    if (!surface) {
        std::cerr << "TTF_RenderText_Blended error" << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_DestroySurface(surface);
    return texture;
}

void Menu::updateButtonHover(int mouseX, int mouseY) {
    if (currentScreen_ == Screen::Main) {
        startButton_.hovered = (mouseX >= startButton_.rect.x && mouseX <= startButton_.rect.x + startButton_.rect.w &&
            mouseY >= startButton_.rect.y && mouseY <= startButton_.rect.y + startButton_.rect.h);

        levelSelectButton_.hovered = (mouseX >= levelSelectButton_.rect.x && mouseX <= levelSelectButton_.rect.x + levelSelectButton_.rect.w &&
            mouseY >= levelSelectButton_.rect.y && mouseY <= levelSelectButton_.rect.y + levelSelectButton_.rect.h);

        quitButton_.hovered = (mouseX >= quitButton_.rect.x && mouseX <= quitButton_.rect.x + quitButton_.rect.w &&
            mouseY >= quitButton_.rect.y && mouseY <= quitButton_.rect.y + quitButton_.rect.h);
    }
    else if (currentScreen_ == Screen::LevelSelect) {
        level1Button_.hovered = (mouseX >= level1Button_.rect.x && mouseX <= level1Button_.rect.x + level1Button_.rect.w &&
            mouseY >= level1Button_.rect.y && mouseY <= level1Button_.rect.y + level1Button_.rect.h);

        level2Button_.hovered = (mouseX >= level2Button_.rect.x && mouseX <= level2Button_.rect.x + level2Button_.rect.w &&
            mouseY >= level2Button_.rect.y && mouseY <= level2Button_.rect.y + level2Button_.rect.h);

        backButton_.hovered = (mouseX >= backButton_.rect.x && mouseX <= backButton_.rect.x + backButton_.rect.w &&
            mouseY >= backButton_.rect.y && mouseY <= backButton_.rect.y + backButton_.rect.h);
    }
}

void Menu::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_EVENT_MOUSE_MOTION) {
        updateButtonHover(e.motion.x, e.motion.y);
    }
    else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            if (currentScreen_ == Screen::Main) {
                if (startButton_.hovered) {
                    result_ = Result::StartGame;
                }
                else if (levelSelectButton_.hovered) {
                    currentScreen_ = Screen::LevelSelect;
                }
                else if (quitButton_.hovered) {
                    result_ = Result::Quit;
                }
            }
            else if (currentScreen_ == Screen::LevelSelect) {
                if (level1Button_.hovered) {
                    result_ = Result::SelectLevel1;
                }
                else if (level2Button_.hovered) {
                    result_ = Result::SelectLevel2;
                }
                else if (backButton_.hovered) {
                    currentScreen_ = Screen::Main;
                }
            }
        }
    }
}

void Menu::update(float dt) {

}

void Menu::renderButton(const Button& button) {
    SDL_Color bgColor;
    if (button.hovered) {
        bgColor = { 80, 180, 80, 255 };
    }
    else {
        bgColor = { 40, 120, 40, 255 };
    }

    SDL_SetRenderDrawColor(renderer_, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer_, &button.rect);

    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderRect(renderer_, &button.rect);

    if (button.textTexture) {
        float texW, texH;
        SDL_GetTextureSize(button.textTexture, &texW, &texH);

        SDL_FRect textRect = {
            button.rect.x + (button.rect.w - texW) / 2,
            button.rect.y + (button.rect.h - texH) / 2,
            texW, texH
        };
        SDL_RenderTexture(renderer_, button.textTexture, nullptr, &textRect);
    }
}

void Menu::render() {
    SDL_SetRenderDrawColor(renderer_, 20, 20, 40, 255);
    SDL_RenderClear(renderer_);

    if (bgTexture_) {
        SDL_FRect bgRect = { 0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT };
        SDL_RenderTexture(renderer_, bgTexture_, nullptr, &bgRect);
    }

 
    if (font_) {
        SDL_Color titleColor = { 255, 100, 50, 255 };
        TTF_SetFontSize(font_, 48);
        SDL_Surface* titleSurf = TTF_RenderText_Blended(font_, "CHEBURASHKA", 11, titleColor);
        TTF_SetFontSize(font_, 28);

        if (titleSurf) {
            SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer_, titleSurf);
            float texW, texH;
            SDL_GetTextureSize(titleTex, &texW, &texH);
            SDL_FRect titleRect = { (WINDOW_WIDTH - texW) / 2, 120, texW, texH };
            SDL_RenderTexture(renderer_, titleTex, nullptr, &titleRect);
            SDL_DestroyTexture(titleTex);
            SDL_DestroySurface(titleSurf);
        }
    }

    if (currentScreen_ == Screen::Main) {
        renderButton(startButton_);
        renderButton(levelSelectButton_);
        renderButton(quitButton_);
    }
    else if (currentScreen_ == Screen::LevelSelect) {
        renderButton(level1Button_);
        renderButton(level2Button_);
        renderButton(backButton_);
    }
}

void Menu::cleanup() {
    if (startButton_.textTexture) SDL_DestroyTexture(startButton_.textTexture);
    if (levelSelectButton_.textTexture) SDL_DestroyTexture(levelSelectButton_.textTexture);
    if (quitButton_.textTexture) SDL_DestroyTexture(quitButton_.textTexture);
    if (level1Button_.textTexture) SDL_DestroyTexture(level1Button_.textTexture);
    if (level2Button_.textTexture) SDL_DestroyTexture(level2Button_.textTexture);
    if (backButton_.textTexture) SDL_DestroyTexture(backButton_.textTexture);
    if (bgTexture_) SDL_DestroyTexture(bgTexture_);
    if (font_) TTF_CloseFont(font_);

    if (musicTrack_) MIX_DestroyTrack(musicTrack_);
    if (menuMusic_) MIX_DestroyAudio(menuMusic_);
    if (mixer_) MIX_DestroyMixer(mixer_);
}