#include "engine/graphics/WindowFactory.h"

int main() {
    Window* window = WindowFactory::CreateRenderWindow(RenderAPI::OpenGL);
    window->Init(1280, 720, "Game Engine");

    while (!window->ShouldClose()) {
        window->PollEvents();
        window->Clear();


        window->SwapBuffers();
    }

    delete window;
    return 0;
}
