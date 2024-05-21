#ifndef SHADER_HELPER_H
#define SHADER_HELPER_H

#include <GL/glew.h>

// Vertex Shader source code
const GLchar* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
)";

const GLchar* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 vertexColor;

void main() {
    FragColor = vertexColor;
}
)";

// Text rendering shaders
const GLchar* textShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;
uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

const GLchar* textFragmentShader = R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)";

#endif // SHADER_HELPER_H