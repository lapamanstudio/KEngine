#include "hwndmap/hwndmap.h"
#include <stdlib.h>

static PanelNode *panelList = NULL; // Lista global inicializada como vacía

void AddPanelToList(HWND hwnd, Panel *panel) {
    PanelNode *newNode = (PanelNode *)malloc(sizeof(PanelNode));
    newNode->hwnd = hwnd;
    newNode->panel = panel;
    newNode->next = panelList;
    panelList = newNode;
}

Panel *FindPanelInList(HWND hwnd) {
    PanelNode *current = panelList;
    while (current) {
        if (current->hwnd == hwnd) {
            return current->panel;
        }
        current = current->next;
    }
    return NULL;
}

void RemovePanelFromList(HWND hwnd) {
    PanelNode **current = &panelList;
    while (*current) {
        PanelNode *entry = *current;
        if (entry->hwnd == hwnd) {
            *current = entry->next;
            free(entry->panel->name);
            free(entry);
            return;
        }
        current = &entry->next;
    }
}

void RemoveAllPanelsFromList() {
    PanelNode *current = panelList;
    while (current) {
        PanelNode *entry = current;
        current = entry->next;
        free(entry->panel->name);
        free(entry);
    }
    panelList = NULL;
}