#include "engine/core/renderer/ShaderLibrary.h"

#include <string>
#include <memory>

std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::s_Shaders{};

// Default shaders
static constexpr const char* SPRITE_VS = R"(
#version 330 core

layout(location=0) in vec2 inPos;
layout(location=1) in vec2 inUV;

out vec2 vUV;
uniform mat4 uVP;

void main() {
    gl_Position = uVP * vec4(inPos, 0.0, 1.0);
    vUV = inUV;
}
)";

static constexpr const char* SPRITE_FS = R"(
#version 330 core

in  vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTex;

void main() { FragColor = texture(uTex, vUV); }
)";

void ShaderLibrary::Init() {
    s_Shaders.emplace("sprite", std::make_shared<Shader>(SPRITE_VS, SPRITE_FS));
}

std::shared_ptr<Shader> ShaderLibrary::Get(std::string_view n) {
    return s_Shaders.at(std::string(n));
}
