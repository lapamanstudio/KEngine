#ifndef SCRIPT_MANAGER_H
#define SCRIPT_MANAGER_H

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>
#include <string>


#include "engine/core/objects/components/Vector2.h"

class ScriptManager {
public:
    static MonoDomain* InitializeMono();
    static void RegisterInternalCalls();

    static MonoClass* GetScriptSDK();
    static MonoMethod* GetMethodByName(MonoClass* scriptClass, const std::string& methodName, int paramCount = -1);

    static void ShutdownMono() {
        MonoDomain* domain = InitializeMono();
        if (domain) {
            mono_jit_cleanup(domain);
            domain = nullptr;
        }
    }
private:
    static MonoClass* scriptSDK;
};

#endif // SCRIPT_MANAGER_H
