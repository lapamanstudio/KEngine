#ifndef HWNDMAP_H
#define HWNDMAP_H

#include <windows.h>

#include "panel/panel.h"

typedef struct PanelNode {
    HWND hwnd;
    Panel *panel;
    struct PanelNode *next;
} PanelNode;

void AddPanelToList(HWND hwnd, Panel *panel);
Panel *FindPanelInList(HWND hwnd);
void RemovePanelFromList(HWND hwnd);
void RemoveAllPanelsFromList();
PanelNode* GetPanelList();
int GetNumberPanelsInSide(int side);
void SetWidthToPanelsInSide(int side, int width);

#endif // HWNDMAP_H
