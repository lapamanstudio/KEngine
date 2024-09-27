#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

class Window {
public:
    virtual ~Window() = default;

    virtual void Init(int width, int height, const char* title) = 0;

    virtual void PollEvents() = 0;

    virtual void Clear() = 0;

    virtual void SwapBuffers() = 0;

    virtual bool ShouldClose() = 0;
};

#endif // ENGINE_WINDOW_H
