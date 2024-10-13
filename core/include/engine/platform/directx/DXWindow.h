#ifndef ENGINE_DXWINDOW_H
#define ENGINE_DXWINDOW_H

#include "engine/graphics/Window.h"

#include <d3d11.h>
#include <windows.h>

class DXWindow : public Window {
public:
    DXWindow();
    ~DXWindow();

    void Init(int width, int height, const char* title) override;
    void PollEvents() override;
    void Clear() override;
    void SwapBuffers() override;
    bool ShouldClose() override;
    float GetTime() override;

    int GetWidth() const override;
    int GetHeight() const override;

    void SetResizeCallback(WindowResizeCallback callback) override;

private:
    // Window
    HWND hwnd;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;

    bool isClosing;

    // Time
    LARGE_INTEGER frequency;
    LARGE_INTEGER startTime;

    int width;
    int height;

    WindowResizeCallback resizeCallback;

    void Resize(int newWidth, int newHeight);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif //ENGINE_DXWINDOW_H
