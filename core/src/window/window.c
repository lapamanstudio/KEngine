#include <string.h>
#include <stdio.h>

#include "window/window.h"
#include "config.h"

const char g_szClassName[] = "KEngineClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;

    // Step 1: Register the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }

    // Window name
    int size = strlen(GAME_ENGINE_NAME) + strlen(GAME_ENGINE_VERSION) + 4;
    char* windowName = malloc(size);
    if (windowName == NULL) {
        perror("Error allocating memory for window name");
        return NULL;
    }

    snprintf(windowName, size, "%s - %s", GAME_ENGINE_NAME, GAME_ENGINE_VERSION);
    
    // Get dimensions of the screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate x and y position of the window
    int windowWidth = screenWidth / 2;
    int windowHeight = screenHeight / 2;
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    // Step 2: Create the window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        windowName, // Title
        WS_OVERLAPPEDWINDOW,
        posX, posY, windowWidth, windowHeight, // Size and position
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }

    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(hwnd);

    free(windowName);

    return hwnd;
}
