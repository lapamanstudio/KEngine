#include "editor/graphics/drivers/GLHelper.h"
#include "editor/graphics/fonts/Fonts.h"
#include "editor/core/utils/FileUtils.h"

#include <iostream>
#include <fstream>
#include <sstream>

GLuint GLHelper::currentShaderProgram = 0;

GLHelper::GLHelper(const char* vertexShaderSource, const char* fragmentShaderSource, const char* textShaderSource, const char* textFragmentSource) {
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    if (textShaderSource != nullptr && textFragmentSource != nullptr) {
        textShaderProgram = createShaderProgram(textShaderSource, textFragmentSource);

        // Init FreeType
        if (!initFreeType()) {
            std::cerr << "ERROR::FREETYPE: Could not initialize FreeType library\n";
        }
    }
}

GLHelper::~GLHelper() {
    glDeleteProgram(shaderProgram);
    glDeleteProgram(textShaderProgram);

    for (auto& character : Characters) {
        glDeleteTextures(1, &character.second.TextureID);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void GLHelper::prepareRender(int width, int height, int multiplier) {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, width * multiplier, height * multiplier);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLHelper::setupFramebuffer(GLuint& texture_id, int width, int height, int multiplier) {
    if (FBO) {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    int textureWidth = width * multiplier;
    int textureHeight = height * multiplier;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint GLHelper::createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    GLuint vertexShader = loadShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

GLuint GLHelper::getProgram() const {
    return shaderProgram;
}

GLuint GLHelper::loadShader(const GLchar* shader_code, GLenum shader_type) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_code, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

void GLHelper::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(currentShaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void GLHelper::setColor3f(const glm::vec3& color) {
    setColor3f(color.r, color.g, color.b);
}

void GLHelper::setColor3f(GLfloat r, GLfloat g, GLfloat b) {
    GLint colorLoc = glGetUniformLocation(currentShaderProgram, "vertexColor");
    glUniform4f(colorLoc, r, g, b, 1.0f);
}

void GLHelper::setColor4f(const glm::vec4& color) {
    setColor4f(color.r, color.g, color.b, color.a);
}

void GLHelper::setColor4f(const glm::vec3& color, GLfloat a) {
    setColor4f(color.r, color.g, color.b, a);
}

void GLHelper::setColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    GLint colorLoc = glGetUniformLocation(currentShaderProgram, "vertexColor");
    glUniform4f(colorLoc, r, g, b, a);
}

void GLHelper::setModelMatrix(const glm::mat4& model) {
    setMat4("model", model);
}

void GLHelper::setViewMatrix(const glm::mat4& view) {
    setMat4("view", view);
}

void GLHelper::setProjectionMatrix(const glm::mat4& projection) {
    setMat4("projection", projection);
}

void GLHelper::disableTexture() {
    glUniform1i(glGetUniformLocation(currentShaderProgram, "useTexture"), GL_FALSE);
}

void GLHelper::setTexture(unsigned int texture) {
    glUniform1i(glGetUniformLocation(currentShaderProgram, "useTexture"), GL_TRUE);
    glUniform1i(glGetUniformLocation(currentShaderProgram, "texture1"), texture);
}

bool GLHelper::initFreeType() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, FileUtils::GetDataFilePath("fonts\\Roboto-Regular.ttf").string().c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font from memory" << std::endl;
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (GLubyte c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

float GLHelper::getTextHeight(const std::string& text, float scale) {
    float height = 0.0f;
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        height = std::max(height, ch.Size.y * scale);
    }
    return height;
}

float GLHelper::getTextWidth(const std::string& text, float scale) {
    float width = 0.0f;
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        width += (ch.Advance >> 6) * scale;
    }
    return width;
}

void GLHelper::renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
    useShader(textShaderProgram);
    // glEnable(GL_CULL_FACE); // For some reason this line breaks the text rendering when the orthographic projection is inverted
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform3f(glGetUniformLocation(getCurrentShaderProgram(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void GLHelper::useShader(GLuint shaderProgram) {
    glUseProgram(shaderProgram);
    currentShaderProgram = shaderProgram;
}

void GLHelper::unuseShader() {
    glUseProgram(0);
    currentShaderProgram = 0;
}

GLuint GLHelper::getCurrentShaderProgram() {
    return currentShaderProgram;
}

GLuint GLHelper::getTextProgram() const {
    return textShaderProgram;
}
