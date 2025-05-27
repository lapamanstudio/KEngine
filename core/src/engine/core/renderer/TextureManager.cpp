#include "engine/core/renderer/TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "engine/thirdparty/stb_image.h"

#include <fstream>
#include <iostream>

TextureManager::~TextureManager() {
    UnloadAll();
}

GLuint TextureManager::LoadTexture(const fs::path& path, bool useCache) {
    // Check cache
    if (useCache) {
        auto it = mTextureCache.find(path);
        if (it != mTextureCache.end()) {
            return it->second;
        }
    }

    // Check if the file exists
    std::ifstream file(path);
    if (!file.good()) {
        std::cerr << "Texture file not found: " << path << std::endl;
        return 0;
    }

    int width, height, nrChannels;
    const std::string pathStr = path.string();
    unsigned char* data = stbi_load(pathStr.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;
    }

    // Create OpenGL texture
    GLuint texID = createGLTexture(data, width, height, nrChannels);

    stbi_image_free(data);

    // Si se va a cachear
    if (useCache && texID != 0) {
        mTextureCache[path] = texID;
    }

    return texID;
}

void TextureManager::UnloadTexture(const fs::path& path) {
    auto it = mTextureCache.find(path);
    if (it != mTextureCache.end()) {
        glDeleteTextures(1, &it->second);
        mTextureCache.erase(it);
    }
}

void TextureManager::UnloadAll() {
    for (auto& kv : mTextureCache) {
        glDeleteTextures(1, &kv.second);
    }
    mTextureCache.clear();
}

GLuint TextureManager::createGLTexture(unsigned char* data, int width, int height, int nrChannels) {
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGB;
    if (nrChannels == 4) format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return textureID;
}
