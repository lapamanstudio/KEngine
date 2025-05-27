#pragma once

#include "engine/core/renderer/Shader.h"

#include <unordered_map>
#include <memory>
#include <string_view>

class ShaderLibrary {
public:
    static void Init();
    static std::shared_ptr<Shader> Get(std::string_view);

private:
    static std::unordered_map<std::string, std::shared_ptr<Shader>> s_Shaders;
};
