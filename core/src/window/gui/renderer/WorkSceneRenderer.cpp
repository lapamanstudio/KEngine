#include <cstdio>

#include "WorkSceneRenderer.h"

const GLchar* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 vertexColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = model * projection * view * vec4(aPos, 1.0f);
    vertexColor = vec4(0.5 + 0.5 * aPos.x, 0.5 + 0.5 * aPos.y, 0.5 + 0.5 * aPos.z, 1.0f);
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
    createTriangle();
    setupFramebuffer();
    this->projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
}

WorkSceneRenderer::~WorkSceneRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void WorkSceneRenderer::createTriangle() {
    // Triangle vertices centered around (0, 0) in NDC
    float vertices[] = {
        // Positions
         100.0f,  0.0f, 0.0f, // Top vertex (centered)
         300.0f, 500.0f, 0.0f, // Bottom Left vertex
         100.0f, 500.0f, 0.0f  // Bottom Right vertex
    };
    unsigned int indices[] = {  // Triangle
        0, 1, 2  // vertices indices
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO
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

void WorkSceneRenderer::render(Camera* camera) {
    glUseProgram(shaderProgram);
    
    // Create a model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

    // Pass the model matrix to the shader
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Projection matrix uniform
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // View matrix uniform
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

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

    this->projection = glm::ortho(0.0f, static_cast<float>(newWidth), 0.0f, static_cast<float>(newHeight), -1.0f, 1.0f);

    setupFramebuffer();
}
