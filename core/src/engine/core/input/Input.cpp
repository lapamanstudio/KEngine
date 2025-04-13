#include "engine/core/input/Input.h"

#include <algorithm>
#include <cctype>

std::unordered_map<std::string, bool> Input::currentKeyStates;
std::unordered_map<std::string, bool> Input::previousKeyStates;

static std::string NormalizeKey(const std::string& key) {
    std::string result = key;
    if (!key.empty() && std::isalpha(key[0])) {
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    }
    return result;
}

void Input::Update() {
    previousKeyStates = currentKeyStates;
}

void Input::SetKeyState(const std::string& key, bool isPressed) {
    currentKeyStates[key] = isPressed;
}

bool Input::GetKey(const std::string& key) {
    return currentKeyStates[key];
}

bool Input::GetKeyDown(const std::string& key) {
    return currentKeyStates[key] && !previousKeyStates[key];
}

bool Input::GetKeyUp(const std::string& key) {
    return !currentKeyStates[key] && previousKeyStates[key];
}


// Internal API [KEngine.Input.Input]
bool Input::Internal_GetKey(MonoString* monoKey) {
    char* utf8Key = mono_string_to_utf8(monoKey);
    std::string keyStr = NormalizeKey(utf8Key);
    mono_free(utf8Key);
    return Input::GetKey(keyStr);
}

bool Input::Internal_GetKeyDown(MonoString* monoKey) {
    char* utf8Key = mono_string_to_utf8(monoKey);
    std::string keyStr = NormalizeKey(utf8Key);
    mono_free(utf8Key);
    return Input::GetKeyDown(keyStr);
}

bool Input::Internal_GetKeyUp(MonoString* monoKey) {
    char* utf8Key = mono_string_to_utf8(monoKey);
    std::string keyStr = NormalizeKey(utf8Key);
    mono_free(utf8Key);
    return Input::GetKeyUp(keyStr);
}
