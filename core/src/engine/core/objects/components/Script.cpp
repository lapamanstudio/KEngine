#include "engine/core/objects/components/Script.h"

#include <ostream>
#include <iostream>

Script::Script(const std::string& scriptName, const std::string& className) {
    domain = ScriptManager::InitializeMono();

    assembly = mono_domain_assembly_open(domain, (scriptName + ".dll").c_str());
    if (!assembly) {
        std::cerr << "Failed to load script assembly" << std::endl;
        return;
    }
    image = mono_assembly_get_image(assembly);

    // TODO GameScripts is a temporary namespace, will be changed to something else
    scriptClass = mono_class_from_name(image, "GameScripts", className.c_str());
    if (!scriptClass) {
        std::cerr << "Failed to find class '" << className << "' in namespace 'GameScripts'" << std::endl;
        return;
    }

    scriptObject = mono_object_new(domain, scriptClass);
    if (!scriptObject) {
        std::cerr << "Failed to instantiate script object for class '" << className << "'" << std::endl;
        return;
    }

    mono_runtime_object_init(scriptObject);
}

Script::~Script() {
    mono_jit_cleanup(domain);
}

void Script::Start() {
    MonoMethod* startMethod = ScriptManager::GetMethodByName(scriptClass, "Start");
    if (startMethod) {
        mono_runtime_invoke(startMethod, scriptObject, NULL, NULL);
    }
}

void Script::Update(float deltaTime) {
    MonoMethod* updateMethod = ScriptManager::GetMethodByName(scriptClass, "Update");
    if (updateMethod) {
        void* args[1] = { &deltaTime };
        mono_runtime_invoke(updateMethod, scriptObject, args, NULL);
    }
}

void Script::CreateMonoBehaviour(void* transformPtr) {
    if (!domain) {
        std::cerr << "Mono domain is not initialized." << std::endl;
        return;
    }

    MonoMethod* createMethod = ScriptManager::GetMethodByName(scriptClass, "CreateMonoBehaviour", 1);
    if (!createMethod) {
        std::cerr << "Failed to find 'CreateMonoBehaviour' method in script class." << std::endl;
        return;
    }

    MonoClass* intptrClass = mono_class_from_name(mono_get_corlib(), "System", "IntPtr");
    if (!intptrClass) {
        std::cerr << "Failed to get 'System.IntPtr' class from mscorlib." << std::endl;
        return;
    }

    void* args[1];
    args[0] = &transformPtr;

    MonoObject* exception = NULL;
    mono_runtime_invoke(createMethod, scriptObject, args, &exception);

    if (exception) {
        MonoString* excStr = mono_object_to_string(exception, NULL);
        char* excCStr = mono_string_to_utf8(excStr);
        std::cerr << "Exception occurred during 'CreateMonoBehaviour' invocation: " << excCStr << std::endl;
        mono_free(excCStr);
        return;
    }
}
