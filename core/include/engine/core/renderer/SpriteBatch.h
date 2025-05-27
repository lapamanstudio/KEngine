#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <string>

struct Vertex2D { glm::vec2 pos; glm::vec2 uv; };

struct SpriteData {
    GLuint     textureID;
    glm::vec2  position;
    glm::vec2  size;
    float      rotation; // radians
};

class SpriteBatch {
public:
    SpriteBatch();
    ~SpriteBatch();

    void Init(size_t maxSprites = 1000);
    void Shutdown();

    void Begin();
    void Submit(const SpriteData& sprite);

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    size_t maxSprites = 0;

    std::vector<Vertex2D> vertices;
    std::vector<SpriteData> queue;

    GLuint mCurrentTexture = 0;

    void CreateIndexBuffer();
    void Flush(const glm::mat4& vp);
};
