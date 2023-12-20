// panel.h

#ifndef PANEL_H
#define PANEL_H

#include <windows.h>
#include <stdbool.h>

#define LEFT_PANEL 0
#define RIGHT_PANEL 1
#define TOP_PANEL 2
#define BOTTOM_PANEL 3

typedef struct {
    char* name;         // Name of the panel
    HWND hwnd;          // Handle of the panel
    int x, y;           // Position 
    int width, height;  // Dimensions
    int sideAsigned;    // 0 = left, 1 = right, 2 = top, 3 = bottom
    bool resizable;

    // Panel state
    bool resizing;
    bool hovering;
} Panel;

void RegisterPanelClass(HINSTANCE hInstance);
Panel* CreateNewPanel(char* name, HWND hwndParent, int sideAsigned);
void RemovePanel(Panel *panel);
void FreeAllPanels();

#endif
