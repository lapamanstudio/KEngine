#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>

class Script {
public:
    Script(const std::string& scriptName);
    ~Script();
    
    void Start();
    void Update(float deltaTime);

private:
    MonoDomain* domain;
    MonoAssembly* assembly;
    MonoObject* scriptObject;

    // Methods
    MonoMethod* startMethod;
    MonoMethod* updateMethod;
};

#endif // SCRIPT_H
