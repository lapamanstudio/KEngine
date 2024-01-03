#include "panel/panel_util.h"

Panel* GetPanelBelow(Panel* panel) {
    PanelNode* current = GetPanelList();
    while (current) {
        if (current->panel && current->panel->sideAssigned == panel->sideAssigned && current->panel->y == panel->y + panel->height) {
            return current->panel;
        }
        current = current->next;
    }
    return NULL;
}

Panel* GetPanelAbove(Panel* panel) {
    PanelNode* current = GetPanelList();
    while (current) {
        if (current->panel && current->panel->sideAssigned == panel->sideAssigned && current->panel->y + current->panel->height == panel->y) {
            return current->panel;
        }
        current = current->next;
    }
    return NULL;
}