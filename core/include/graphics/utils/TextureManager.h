#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "graphics/drivers/GLHelper.h"

class TextureManager {
public:
    static void Init();
    static GLuint LoadTexture(const unsigned char* image, unsigned int image_len);
    static void renderTexture(GLuint texture, int x, int y, int w, int h);
private:
    static GLuint VAO;
    static GLuint VBO;
};

#endif // TEXTURE_MANAGER_H
