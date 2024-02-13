#include <cstdio>

#include "WorkSceneRenderer.h"

const GLchar* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 vertexColor;
void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    vertexColor = vec4(0.5f + 0.5f * aPos.x, 0.5f + 0.5f * aPos.y, 0.5f + 0.5f * aPos.z, 1.0f);
}
)";

const GLchar* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec4 vertexColor;
void main() {
    FragColor = vertexColor;
}
)";

WorkSceneRenderer::WorkSceneRenderer(int posX, int posY, int width, int height) : posX(posX), posY(posY), width(width), height(height), texture_id(0), shaderProgram(0), VAO(0), VBO(0), EBO(0), FBO(0) {
    createShaders();
    setupFramebuffer();
}

WorkSceneRenderer::~WorkSceneRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void WorkSceneRenderer::setupFramebuffer() {
    if (FBO)
    {
      glDeleteFramebuffers(GL_FRAMEBUFFER, &FBO);
      glDeleteTextures(1, &texture_id);
      texture_id = 0;
    }

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    GLenum buffers[4] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(texture_id, buffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float vertices[] = {
        0.0f, 0.5f, 0.0f,  // top right
        -0.5f, -0.5f, 0.0f,  // bottom right
        1.0f, -0.5f, 0.0f,  // bottom left
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  // first Triangle
    };

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void WorkSceneRenderer::createShaders() {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", infoLog);
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s\n", infoLog);
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED: %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void WorkSceneRenderer::render() {
    glUseProgram(shaderProgram);

    // For some reason the vertices are inverted when drawn, so to correct it I re-invert the vertices again
    glLoadIdentity();
    glScalef(1.0f, -1.0f, 1.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(posX, posY, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint WorkSceneRenderer::getTexture() const {
    return texture_id;
}

void WorkSceneRenderer::updateSize(int newX, int newY, int newWidth, int newHeight) {
    this->posX = newX;
    this->posY = newY;
    this->width = newWidth;
    this->height = newHeight;

    setupFramebuffer();
}
