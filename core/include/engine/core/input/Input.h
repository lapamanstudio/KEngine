#pragma once

#include <unordered_map>
#include <string>
#include <mono/jit/jit.h>

class Input
{
public:
    static bool GetKeyDown(const std::string& keyCode);
    static bool GetKeyUp(const std::string& keyCode);
    static bool GetKey(const std::string& keyCode);

    static bool Internal_GetKeyDown(MonoString* monoKey);
    static bool Internal_GetKeyUp(MonoString* monoKey);
    static bool Internal_GetKey(MonoString* monoKey);

    static void SetKeyState(const std::string& key, bool isPressed);
    static void Update();

private:
    static std::unordered_map<std::string, bool> currentKeyStates;
    static std::unordered_map<std::string, bool> previousKeyStates;
};
