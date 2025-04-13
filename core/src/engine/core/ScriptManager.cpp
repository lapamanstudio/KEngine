#include "engine/core/ScriptManager.h"
#include "engine/core/objects/components/Transform.h"
#include "engine/core/input/Input.h"

#include <iostream>
#include <ostream>

MonoClass* ScriptManager::scriptSDK = nullptr;

MonoDomain* ScriptManager::InitializeMono() {
    MonoDomain* domain = mono_jit_init("GameEngine");
    mono_config_parse(NULL);
    RegisterInternalCalls();

    MonoAssembly* assembly = mono_domain_assembly_open(domain, "KEngine.dll");

    MonoImage* image = mono_assembly_get_image(assembly);
    ScriptManager::scriptSDK = mono_class_from_name(image, "KEngine", "MonoBehaviour");

    return domain;
}

void ScriptManager::RegisterInternalCalls() {
    std::cout << "Registering internal calls for KEngine.Transform" << std::endl;

    // KEngine.Components.Transform
    mono_add_internal_call("KEngine.Components.Transform::Internal_GetPosition", (void*)Transform::Internal_GetPosition);
    mono_add_internal_call("KEngine.Components.Transform::Internal_SetPosition", (void*)Transform::Internal_SetPosition);

    std::cout << "Registered KEngine.Components.Transform" << std::endl;

    // KEngine.Input.Input
    mono_add_internal_call("KEngine.Input.Input::GetKeyDown", (void*)Input::Internal_GetKeyDown);
    mono_add_internal_call("KEngine.Input.Input::GetKeyUp", (void*)Input::Internal_GetKeyUp);
    mono_add_internal_call("KEngine.Input.Input::GetKey", (void*)Input::Internal_GetKey);

    std::cout << "Registered KEngine.Input.Input" << std::endl;
}

MonoClass* ScriptManager::GetScriptSDK() {
    return scriptSDK;
}

MonoMethod* ScriptManager::GetMethodByName(MonoClass* klass, const std::string& methodName, int paramCount) {
    while (klass) {
        MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), paramCount);
        if (method) {
            return method;
        }
        klass = mono_class_get_parent(klass);
    }
    return nullptr;
}