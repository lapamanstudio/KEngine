#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <functional>

using WindowResizeCallback = std::function<void(int newWidth, int newHeight)>;

class Window {
public:
    virtual ~Window() = default;

    virtual void Init(int width, int height, const char* title) = 0;

    virtual void PollEvents() = 0;

    virtual void Clear() = 0;

    virtual void SwapBuffers() = 0;

    virtual bool ShouldClose() = 0;

    virtual float GetTime() = 0;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    virtual void SetResizeCallback(WindowResizeCallback callback) = 0;
};

#endif // ENGINE_WINDOW_H
