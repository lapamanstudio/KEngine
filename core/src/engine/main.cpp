#include "engine/graphics/WindowFactory.h"

#include "engine/core/EngineSceneManager.h"
#include "engine/core/objects/GameObject.h"
#include "engine/core/renderer/GraphicHelper.h"

#include <iostream>
#include <ostream>
#include <cstdio> // Include for printf

int main() {
    // Setup window
    Window* window = WindowFactory::CreateRenderWindow(RenderAPI::OpenGL);
    window->Init(800, 600, "Game Engine");

    // Initialize Graphics
    GraphicHelper::Init(RenderAPI::OpenGL);

    // Load Mono C# assembly
    MonoDomain* domain = mono_jit_init("GameEngine");
    if (!domain) {
        std::cerr << "Failed to initialize Mono" << std::endl;
        return -1;
    }

    mono_config_parse(NULL);

    // Load default scene   
    EngineSceneManager sceneManager = EngineSceneManager();

    CameraViewComponent* camera = new CameraViewComponent(800, 600);
    sceneManager.SetMainCamera(camera);

    window->SetResizeCallback([camera](int newWidth, int newHeight) {
        camera->OnResize(newWidth, newHeight);
    });

    // Initialize delta time and FPS counters
    float lastTime = window->GetTime();
    float fpsTimeCounter = 0.0f;
    int frames = 0;

    // Main loop
    while (!window->ShouldClose()) {
        // Delta time
        float currentTime = window->GetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Update FPS counters
        fpsTimeCounter += deltaTime;
        frames++;

        // Print FPS
        if (fpsTimeCounter >= 1.0f) {
            float fps = frames / fpsTimeCounter;
            printf("FPS: %.2f\n", fps);

            fpsTimeCounter = 0.0f;
            frames = 0;
        }

        window->PollEvents();

        sceneManager.Update(deltaTime);
        
        window->Clear();

        sceneManager.Render();

        window->SwapBuffers();
    }

    delete camera;
    delete window;
    return 0;
}
