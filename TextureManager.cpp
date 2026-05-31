#include "TextureManager.h"
#include <iostream>

TextureManager::~TextureManager() {
    clear();
}

void TextureManager::add(const std::string& id, SDL_Texture* tex) {
    if (!tex) {
        std::cerr << "TextureManager::add: null texture for id " << id << "\n";
        return;
    }
    textures_[id] = tex;
}

SDL_Texture* TextureManager::get(const std::string& id) const {
    auto it = textures_.find(id);
    if (it != textures_.end())
        return it->second;
    return nullptr;
}

void TextureManager::clear() {
    for (auto& [id, tex] : textures_) {
        if (tex) SDL_DestroyTexture(tex);
    }
    textures_.clear();
}