#include "engine/core/objects/components/Script.h"

#include <ostream>
#include <iostream>

Script::Script(const std::string& scriptName) {
    domain = mono_jit_init("GameEngine");
    mono_config_parse(NULL);

    // Load the C# script assembly
    assembly = mono_domain_assembly_open(domain, scriptName.c_str());
    if (!assembly) {
        std::cerr << "Failed to load script assembly" << std::endl;
        return;
    }

    MonoImage* image = mono_assembly_get_image(assembly);
    MonoClass* scriptClass = mono_class_from_name(image, "Namespace", "ClassName");
    
    // Create an instance of the C# script class
    scriptObject = mono_object_new(domain, scriptClass);
    mono_runtime_object_init(scriptObject);

    // Get the Update method from C# script
    updateMethod = mono_class_get_method_from_name(scriptClass, "Update", 1);
}

Script::~Script() {
    mono_jit_cleanup(domain);
}

void Script::Start() {
    if (startMethod) {
        mono_runtime_invoke(startMethod, scriptObject, NULL, NULL);
    }
}

void Script::Update(float deltaTime) {
    if (updateMethod) {
        void* args[1] = { &deltaTime };
        mono_runtime_invoke(updateMethod, scriptObject, args, NULL);
    }
}
