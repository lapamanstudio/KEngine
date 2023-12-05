// panel.h

#ifndef PANEL_H
#define PANEL_H

#include <windows.h>

typedef struct {
    HWND hwnd;          // Handle of the panel
    int x, y;           // Position 
    int width, height;  // Dimensions
    HBRUSH background;  // Background
    HWND hButton;
} Panel;

void InitPanel(Panel *panel, HWND hwndParent, int x, int y, int width, int height);
void SetPanelBackground(Panel *panel, COLORREF color);
void AddButtonToPanel(Panel *panel, LPCWSTR text, int x, int y, int width, int height);
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
