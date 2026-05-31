#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

class TextureManager {
public:
    TextureManager() = default;
    ~TextureManager();

    void add(const std::string& id, SDL_Texture* tex);
    SDL_Texture* get(const std::string& id) const;
    void clear();

private:
    std::unordered_map<std::string, SDL_Texture*> textures_;
};