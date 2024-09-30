#include "engine/graphics/WindowFactory.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>


int main() {
    // Setup window
    Window* window = WindowFactory::CreateRenderWindow(RenderAPI::OpenGL);
    window->Init(1280, 720, "Game Engine");

    // Load Mono C# assembly
    MonoDomain* domain = mono_jit_init("GameEngine");
    if (!domain) {
        std::cerr << "Failed to initialize Mono" << std::endl, -1;
        exit(-1);
    }

    mono_config_parse(NULL);

    // Main loop
    float lastTime = 0;
    while (!window->ShouldClose()) {
        // Delta time
        float currentTime = window->GetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        window->PollEvents();
        
        
        window->Clear();


        window->SwapBuffers();
    }

    delete window;
    return 0;
}
