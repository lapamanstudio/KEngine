#include "graphics/utils/TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "graphics/utils/stb_image.h"
#include "core/FileUtils.h"

#include <unordered_map>
#include <iostream>
#include <fstream>

GLuint TextureManager::VAO = 0;
GLuint TextureManager::VBO = 0;
GLuint TextureManager::EBO = 0;

std::unordered_map<std::string, GLuint> TextureManager::textureCache;
std::unordered_map<std::size_t, GLuint> TextureManager::memoryTextureCache;

void TextureManager::Init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int iconIndices[] = { 0, 1, 2, 2, 3, 0 };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iconIndices), iconIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

GLuint TextureManager::LoadTexture(const char* image_path) {
    std::string full_image_path = GetDataFilePath(image_path);
    auto it = textureCache.find(full_image_path);
    if (it != textureCache.end()) {
        return it->second;
    }

    std::ifstream file(full_image_path);
    if (!file.good()) return 0;

    int width, height, nrChannels;
    unsigned char* data = stbi_load(full_image_path.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture from memory." << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    textureCache[full_image_path] = textureID;

    return textureID;
}

GLuint TextureManager::LoadTexture(const unsigned char* image, unsigned int image_len) {
    // Generate unique key to check in cache
    std::size_t key = std::hash<const unsigned char*>{}(image) ^ std::hash<unsigned int>{}(image_len);

    auto it = memoryTextureCache.find(key);
    if (it != memoryTextureCache.end()) {
        return it->second;
    }

    int width, height, nrChannels;
    unsigned char* data = stbi_load_from_memory(image, image_len, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture from memory." << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    memoryTextureCache[key] = textureID;

    return textureID;
}

void TextureManager::renderTexture(GLuint texture, int x, int y, int w, int h) {
    if (texture == 0) return;

    float iconVertices[] = {
        static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f, 1.0f,
        static_cast<float>(x + w), static_cast<float>(y), 0.0f, 1.0f, 1.0f,
        static_cast<float>(x + w), static_cast<float>(y + h), 0.0f, 1.0f, 0.0f,
        static_cast<float>(x), static_cast<float>(y + h), 0.0f, 0.0f, 0.0f
    };

    // Get previous VAO
    GLint previousVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(iconVertices), iconVertices, GL_DYNAMIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLHelper::setTexture(0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Restore previous VAO
    glBindVertexArray(previousVAO);
}
