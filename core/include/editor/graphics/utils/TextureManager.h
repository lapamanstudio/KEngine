#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "editor/graphics/drivers/GLHelper.h"

class TextureManager {
public:
    static void Init();
    
    static GLuint LoadTextureFromDataFile(const char* image_path);
    static GLuint LoadTextureAsset(const char* image_path);

    static void renderTexture(GLuint texture, int x, int y, int w, int h);
private:
    static GLuint VAO;
    static GLuint VBO;
    static GLuint EBO;

    static GLuint LoadTexture(const char* image, bool useCache);

    static std::unordered_map<std::string, GLuint> textureCache; // File-based textures
    static std::unordered_map<std::size_t, GLuint> memoryTextureCache; // Memory-based textures
};

#endif // TEXTURE_MANAGER_H
