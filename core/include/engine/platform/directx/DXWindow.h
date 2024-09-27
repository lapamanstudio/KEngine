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

private:
    HWND hwnd;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;

    bool isClosing;
};

#endif //ENGINE_DXWINDOW_H
