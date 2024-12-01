#ifndef SCRIPT_H
#define SCRIPT_H

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>

#include "engine/core/ScriptManager.h"

class Script {
public:
    Script(const std::string& scriptName, const std::string& className);
    ~Script();

    void Start();
    void Update(float deltaTime);
    void CreateMonoBehaviour(void* transformPtr);

private:
    MonoDomain* domain;
    MonoAssembly* assembly;
    MonoImage* image;
    MonoObject* scriptObject;
    MonoClass* scriptClass;
};

#endif // SCRIPT_H
