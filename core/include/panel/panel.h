// panel.h

#ifndef PANEL_H
#define PANEL_H

#include <windows.h>
#include <stdbool.h>

typedef struct {
    HWND hwnd;          // Handle of the panel
    int x, y;           // Position 
    int width, height;  // Dimensions
    HBRUSH background;  // Background
    bool isLeftAligned;
    bool resizing;
} Panel;

void RegisterPanelClass(HINSTANCE hInstance);
Panel* CreateNewPanel(HWND hwndParent, bool isLeftAligned);
void RemovePanel(Panel *panel);

#endif
