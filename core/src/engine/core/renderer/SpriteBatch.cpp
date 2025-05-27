#include "engine/core/renderer/SpriteBatch.h"
#include "engine/core/renderer/ShaderLibrary.h"

#include <cmath>
#include <iostream>

SpriteBatch::SpriteBatch() {}

SpriteBatch::~SpriteBatch() {
    Shutdown();
}

void SpriteBatch::Init(size_t max) {
    maxSprites = max;
    vertices.resize(max * 4);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex2D),
                 nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);

    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)sizeof(glm::vec2));

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    CreateIndexBuffer();

    glBindVertexArray(0);
}

void SpriteBatch::Shutdown() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vao) glDeleteVertexArrays(1, &vao);
    vao = vbo = ebo = 0;
}

void SpriteBatch::Begin() { queue.clear(); }

void SpriteBatch::Submit(const SpriteData& sprite) { queue.push_back(sprite); }

void SpriteBatch::CreateIndexBuffer() {
    std::vector<GLuint> indexes(maxSprites * 6);
    for (size_t i = 0; i < maxSprites; ++i) {
        GLuint offset = (GLuint)(i * 4);

        indexes[i*6 + 0] = offset + 0;
        indexes[i*6 + 1] = offset + 1;
        indexes[i*6 + 2] = offset + 2;
        indexes[i*6 + 3] = offset + 2;
        indexes[i*6 + 4] = offset + 3;
        indexes[i*6 + 5] = offset + 0;
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), indexes.data(), GL_STATIC_DRAW);
}

void SpriteBatch::Flush(const glm::mat4& vp)
{
    if (queue.empty()) return;

    size_t vi = 0;
    for (const auto& sp : queue)
    {
        float c = std::cos(sp.rotation), s = std::sin(sp.rotation);
        glm::vec2 h = sp.size * 0.5f;

        auto rot = [&](float x, float y) {
            return glm::vec2(x * c - y * s, x * s + y * c) + sp.position;
        };

        glm::vec2 p0 = rot(-h.x, -h.y);
        glm::vec2 p1 = rot( h.x, -h.y);
        glm::vec2 p2 = rot( h.x,  h.y);
        glm::vec2 p3 = rot(-h.x,  h.y);

        vertices[vi + 0] = { p0, {0, 1} };
        vertices[vi + 1] = { p1, {1, 1} };
        vertices[vi + 2] = { p2, {1, 0} };
        vertices[vi + 3] = { p3, {0, 0} };
        vi += 4;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, queue.size() * 4 * sizeof(Vertex2D), vertices.data());

    auto sh = ShaderLibrary::Get("sprite");
    sh->Use();
    sh->SetMat4("uVP", vp);
    sh->SetInt ("uTex", 0);

    glBindVertexArray(vao);

    size_t first = 0;
    while (first < queue.size()) {
        GLuint tex = queue[first].textureID;
        size_t count = 0;
        for (size_t i = first;
             i < queue.size() && queue[i].textureID == tex; ++i)
            ++count;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(count * 6),
                       GL_UNSIGNED_INT,
                       (void*)(first * 6 * sizeof(GLuint)));

        first += count;
    }

    glBindVertexArray(0);
    queue.clear();
}
