#pragma once

#include "engine/core/utils/FileUtils.h"

#include <gl/glew.h>
#include <unordered_map>

namespace fs = std::filesystem;

class TextureManager {
public:
    TextureManager() = default;
    ~TextureManager();

    GLuint LoadTexture(const fs::path& path, bool useCache = true);

    void UnloadTexture(const fs::path& path);
    void UnloadAll();

private:
    std::unordered_map<fs::path, GLuint> mTextureCache;

    GLuint createGLTexture(unsigned char* data, int width, int height, int nrChannels); // TODO This must be moved to other generic class because DX and OpenGL have different implementations
};
