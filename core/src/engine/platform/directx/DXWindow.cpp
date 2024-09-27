#include "engine/platform/directx/DXWindow.h"

#include <iostream>
#include <ostream>

DXWindow::DXWindow() : hwnd(nullptr), device(nullptr), context(nullptr), swapChain(nullptr), renderTargetView(nullptr) {}

DXWindow::~DXWindow() {
    if (renderTargetView) renderTargetView->Release();
    if (swapChain) swapChain->Release();
    if (context) context->Release();
    if (device) device->Release();
    if (hwnd) DestroyWindow(hwnd);
}

void DXWindow::Init(int width, int height, const char* title) {
    // Registrar la clase de ventana
    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "DirectXWindowClass";

    if (!RegisterClass(&wc)) {
        std::cerr << "Failed to register window class!" << std::endl;
        return;
    }

    // Crear la ventana
    hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!hwnd) {
        std::cerr << "Failed to create window!" << std::endl;
        return;
    }

    // Mostrar la ventana
    ShowWindow(hwnd, SW_SHOW);

    // Inicializar DirectX 11
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Width = width;
    swapDesc.BufferDesc.Height = height;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = hwnd;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &swapDesc, &swapChain, &device, nullptr, &context
    );

    if (FAILED(hr)) {
        std::cerr << "Failed to create DirectX 11 device and swap chain!" << std::endl;
        return;
    }

    // Crear render target view
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to get back buffer!" << std::endl;
        return;
    }

    hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();

    if (FAILED(hr)) {
        std::cerr << "Failed to create render target view!" << std::endl;
        return;
    }

    context->OMSetRenderTargets(1, &renderTargetView, nullptr);
}

void DXWindow::PollEvents() {
    MSG msg = { 0 };
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void DXWindow::Clear() {
    float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    context->ClearRenderTargetView(renderTargetView, color);
}

void DXWindow::SwapBuffers() {
    swapChain->Present(1, 0);
}

bool DXWindow::ShouldClose() {
    MSG msg = { 0 };
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return true;
        }
    }

    return false;
}
