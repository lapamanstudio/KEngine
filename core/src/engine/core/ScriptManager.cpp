#include "engine/core/ScriptManager.h"
#include "engine/core/objects/components/Transform.h"

#include <iostream>
#include <ostream>

MonoClass* ScriptManager::scriptSDK = nullptr;

MonoDomain* ScriptManager::InitializeMono() {
    MonoDomain* domain = mono_jit_init("GameEngine");
    mono_config_parse(NULL);
    RegisterInternalCalls();

    MonoAssembly* assembly = mono_domain_assembly_open(domain, "Kengine.NET.SDK.dll");

    MonoImage* image = mono_assembly_get_image(assembly);
    ScriptManager::scriptSDK = mono_class_from_name(image, "KEngine", "MonoBehaviour");

    return domain;
}

void ScriptManager::RegisterInternalCalls() {
    std::cout << "Registering internal calls for KEngine.Transform" << std::endl;
    mono_add_internal_call("KEngine.Transform::Internal_GetPosition", (void*)Internal_GetPosition);
    mono_add_internal_call("KEngine.Transform::Internal_SetPosition", (void*)Internal_SetPosition);
    std::cout << "Registered Internal_GetPosition and Internal_SetPosition" << std::endl;
}

void ScriptManager::Internal_GetPosition(void* nativeTransformPtr, Vector2* outPosition) {
    Transform* transform = (Transform*)nativeTransformPtr;
    if (transform) {
        *outPosition = transform->GetPosition();
    }
}

void ScriptManager::Internal_SetPosition(void* nativeTransformPtr, Vector2* position) {
    static int first = 1;

    if (first != 3) {
        first += 1;
        printf("[Internal_SetPosition] Transform pointer: %p\n", nativeTransformPtr); 
        std::cout << "Setting position to " << position->x << ", " << position->y << std::endl;
    }
    Transform* transform = (Transform*)nativeTransformPtr;
    if (transform) {
        transform->SetPosition(position->x, position->y);
    }
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