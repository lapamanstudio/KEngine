#ifndef GLHELPER_H
#define GLHELPER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

class GLHelper {
public:
    GLHelper(const char* vertexPath, const char* fragmentPath, const char* textVertexPath, const char* textFragmentPath);
    ~GLHelper();

    GLuint getProgram() const;
    GLuint getTextProgram() const;

    static void setMat4(const std::string& name, const glm::mat4& mat);
    static void setColor3f(const glm::vec3& color);
    static void setColor3f(GLfloat r, GLfloat g, GLfloat b);
    static void setColor4f(const glm::vec4& color);
    static void setColor4f(const glm::vec3& color, GLfloat a);
    static void setColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    static void setModelMatrix(const glm::mat4& model);
    static void setViewMatrix(const glm::mat4& view);
    static void setProjectionMatrix(const glm::mat4& projection);
    static void useShader(GLuint shaderProgram);
    static void unuseShader();
    static GLuint getCurrentShaderProgram();

    bool initFreeType();
    float getTextWidth(const std::string& text, float scale);
    float getTextHeight(const std::string& text, float scale);
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);

private:
    GLuint shaderProgram;
    GLuint textShaderProgram;

    static GLuint currentShaderProgram;

    GLuint loadShader(const GLchar* path, GLenum shaderType);
    GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

    struct Character {
        GLuint TextureID;  // ID
        glm::ivec2 Size;   // Size
        glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
        GLuint Advance;    // Offset to advance to next character
    };

    std::map<GLchar, Character> Characters;
    GLuint VAO, VBO;
};

#endif
