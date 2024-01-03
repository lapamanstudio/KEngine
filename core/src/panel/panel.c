#include <windowsx.h>
#include <windows.h>
#include <stdio.h>

#include "panel/panel_util.h"
#include "panel/panel.h"
#include "hwndmap/hwndmap.h"

// Constants for UI elements
#define BUTTON_MINIMIZE 1001
#define BUTTON_HOVERING_COLOR RGB(100, 100, 100)
#define BUTTON_MINIMIZE_LINE_COLOR RGB(160, 160, 160)
#define BUTTON_OFFSET_RIGHT 25
#define BUTTON_OFFSET_TOP 5
#define BUTTON_WIDTH 20
#define BUTTON_HEIGHT 15

#define WM_MINIMIZE_PANEL WM_USER + 1

#define PANEL_MIN_SIZE 30
#define PANEL_MIN_SIZE_OFFSET 30
#define PANEL_DEFAULT_SIZE 200
#define PANEL_BORDER 10
#define PANEL_HEADER_HEIGHT 25

#define PANELS_BORDER_LINE_COLOR RGB(108, 108, 108)

#define PANEL_TITLE_TEXT_OFFSET_X 10
#define PANEL_TITLE_TEXT_OFFSET_Y 4
#define PANEL_TITLE_TEXT_COLOR RGB(230, 230, 230)
#define PANEL_TITLE_TEXT_OFFSET_BUTTON 10

#define PANEL_BACKGROUND_COLOR RGB(34, 34, 34)
#define PANEL_HEADER_HOVERING_COLOR RGB(37, 37, 37)

// Global variables for brushes
HBRUSH brushBackground;
HBRUSH brushHeaderHovering;

wchar_t* panelClassName = L"KCustomPanelClass";

// Function prototypes
void InitPanel(Panel *panel, HWND hwndParent, int sideAssigned, int resizable);
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void RegisterPanelClass(HINSTANCE hInstance);
HWND CreateCustomButton(HWND hwndParent, HINSTANCE hInstance, int x, int y, int width, int height, int id);
void FreeAllPanels();
void RemovePanel(Panel *panel);

// Register the panel class with customized window procedure
void RegisterPanelClass(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = PanelProc;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = panelClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) {
        MessageBox(NULL, "Panel Class Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    brushBackground = CreateSolidBrush(PANEL_BACKGROUND_COLOR);
    brushHeaderHovering = CreateSolidBrush(PANEL_HEADER_HOVERING_COLOR);
}

// Create and initialize a new panel
Panel* CreateNewPanel(char* name, HWND hwndParent, int sideAssigned, int resizable) {
    Panel* newPanel = (Panel*)malloc(sizeof(Panel));
    if (!newPanel) return NULL;

    newPanel->name = strdup(name);
    if (!newPanel->name) {
        free(newPanel);
        return NULL;
    }

    InitPanel(newPanel, hwndParent, sideAssigned, resizable);
    AddPanelToList(newPanel->hwnd, newPanel);

    return newPanel;
}

// Initialize the panel with specified dimensions and position
void InitPanel(Panel *panel, HWND hwndParent, int sideAssigned, int resizable) {
    int defaultSize = PANEL_DEFAULT_SIZE;
    panel->sideAssigned = sideAssigned;
    panel->resizingHorizontal = false;
    panel->resizingVertical = false;
    panel->hovering = false;
    panel->resizable = resizable;
    panel->hoveringMinimizeButton = false;

    RECT parentRect;
    GetClientRect(hwndParent, &parentRect);

    int assignedPanels = GetNumberPanelsInSide(sideAssigned) + 1;

    int totalHeight = parentRect.bottom - parentRect.top;
    int basePanelHeight = totalHeight / assignedPanels;
    int extraHeight = totalHeight % assignedPanels; // Rest

    PanelNode* current = GetPanelList();
    int yPos = 0;
    int panelCount = 0;

    while(current) {
        if (current->panel && current->panel->sideAssigned == sideAssigned) {
            panelCount++;
            int currentPanelHeight = basePanelHeight;

            // Add 1 extra pixel
            if (panelCount > assignedPanels - extraHeight) {
                currentPanelHeight++;
            }

            current->panel->height = currentPanelHeight;
            current->panel->y = yPos;
            yPos += currentPanelHeight;

            MoveWindow(current->panel->hwnd, current->panel->x, current->panel->y, current->panel->width, current->panel->height, TRUE);
        }
        current = current->next;
    }

    // Configure the new panel
    panel->x = (sideAssigned == LEFT_PANEL) ? 0 : GetSystemMetrics(SM_CXSCREEN) - defaultSize;
    panel->y = yPos;
    panel->height = basePanelHeight + (panelCount <= extraHeight ? 1 : 0);
    panel->width = defaultSize;

    // Create the panel
    panel->hwnd = CreateWindowExW(
        0,
        panelClassName,
        NULL,
        WS_CHILD | WS_VISIBLE,
        panel->x, panel->y, panel->width, panel->height,
        hwndParent,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
        NULL
    );

    // Create the minimize button
    if (resizable) {
        panel->minimizeButton = CreateCustomButton(
            panel->hwnd,
            (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
            panel->width - BUTTON_OFFSET_RIGHT, BUTTON_OFFSET_TOP, BUTTON_WIDTH, BUTTON_HEIGHT,
            BUTTON_MINIMIZE
        );
    }
}


// Custom button creation
HWND CreateCustomButton(HWND hwndParent, HINSTANCE hInstance, int x, int y, int width, int height, int id) {
    HWND hwndButton = CreateWindow(
        "BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        x, y, width, height, 
        hwndParent, (HMENU)(INT_PTR) id, hInstance, NULL
    );
    SetWindowLongPtr(hwndButton, GWLP_WNDPROC, (LONG_PTR)ButtonProc);
    return hwndButton;
}

// Cleanup function to free allocated resources
void FreeAllPanels() {
    if (brushBackground) DeleteObject(brushBackground);
    if (brushHeaderHovering) DeleteObject(brushHeaderHovering);
    RemoveAllPanelsFromList();
}

// Remove a panel and free its resources
void RemovePanel(Panel *panel) {
    RemovePanelFromList(panel->hwnd);
    free(panel);
}

void UpdatePanelsPosition(HWND windowHwnd) {
    PanelNode* current = GetPanelList();
    RECT rect;
    GetClientRect(windowHwnd, &rect);

    int screenWidth = rect.right - rect.left;
    int screenHeight = rect.bottom - rect.top;
    int maxWidth = screenWidth / 3;

    while (current) {
        Panel* panel = current->panel;
        if (panel) {
            switch(panel->sideAssigned) {
                case LEFT_PANEL:
                case RIGHT_PANEL: {
                    int totalAvailableHeight = screenHeight;
                    int fixedHeightTotal = 0;
                    int numDynamicPanels = 0;

                    // Calculate the total height of the fixed panels and the number of dynamic panels
                    PanelNode* panelNode = GetPanelList();
                    while (panelNode) {
                        Panel* iterPanel = panelNode->panel;
                        if (iterPanel && iterPanel->sideAssigned == panel->sideAssigned) {
                            iterPanel->width = min(iterPanel->width, maxWidth);
                            if (!iterPanel->resizable) {
                                fixedHeightTotal += iterPanel->height;
                            } else {
                                numDynamicPanels++;
                            }
                        }
                        panelNode = panelNode->next;
                    }

                    // Height available for the dynamic panels
                    totalAvailableHeight -= fixedHeightTotal;
                    int baseDynamicPanelHeight = totalAvailableHeight / numDynamicPanels;
                    int remainingHeight = totalAvailableHeight % numDynamicPanels;

                    // Asign the new position and height to the panels
                    int yPos = 0;
                    panelNode = GetPanelList();
                    while (panelNode) {
                        Panel* iterPanel = panelNode->panel;
                        if (iterPanel && iterPanel->sideAssigned == panel->sideAssigned) {
                            iterPanel->x = (panel->sideAssigned == RIGHT_PANEL) ? screenWidth - iterPanel->width : 0;
                            iterPanel->y = yPos;

                            if (iterPanel->resizable) {
                                iterPanel->height = baseDynamicPanelHeight + (remainingHeight > 0 ? 1 : 0);
                                if (remainingHeight > 0) {
                                    remainingHeight--;
                                }
                            }

                            MoveWindow(iterPanel->hwnd, iterPanel->x, iterPanel->y, iterPanel->width, iterPanel->height, TRUE);
                            yPos += iterPanel->height;
                        }
                        panelNode = panelNode->next;
                    }
                    break;
                }
                case TOP_PANEL:
                    panel->x = 0;
                    panel->y = 0;
                    break;
                case BOTTOM_PANEL:
                    panel->x = 0;
                    panel->y = screenHeight - panel->height;
                    break;
            }
            MoveWindow(panel->hwnd, panel->x, panel->y, panel->width, panel->height, TRUE);
        }
        current = current->next;
    }
}


// Window procedure for handling panel messages
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Retrieve the pointer to the Panel structure stored in GWLP_USERDATA
    Panel *panel = FindPanelInList(hwnd);

    if (panel == NULL)
        return DefWindowProc(hwnd, uMsg, wParam, lParam);

    switch (uMsg) {
        // Resizing panel
        case WM_SETCURSOR: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            if (!panel->resizable) break;

            RECT windowRect;
            GetClientRect(GetParent(panel->hwnd), &windowRect);
            int screenHeight = windowRect.bottom - windowRect.top;

            switch(panel->sideAssigned) {
                case LEFT_PANEL: {
                    if (pt.x >= panel->width - PANEL_BORDER && pt.x <= panel->width) {
                        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                        return TRUE;
                    } else {
                        if (panel->y + panel->height != screenHeight && pt.y <= panel->height && pt.y >= panel->height - PANEL_BORDER) {
                            SetCursor(LoadCursor(NULL, IDC_SIZENS));
                            return TRUE;
                        }
                    }
                    break;
                }
                case RIGHT_PANEL: {
                    if (pt.x >= 0 && pt.x <= PANEL_BORDER) {
                        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                        return TRUE;
                    } else {
                        if (panel->y + panel->height != screenHeight && pt.y <= panel->height && pt.y >= panel->height - PANEL_BORDER) {
                            SetCursor(LoadCursor(NULL, IDC_SIZENS));
                            return TRUE;
                        }
                    }
                    break;
                }
                case TOP_PANEL: {
                    if (pt.y >= panel->height - PANEL_BORDER && pt.y <= panel->height) {
                        SetCursor(LoadCursor(NULL, IDC_SIZENS));
                        return TRUE;
                    }
                    break;
                }
                case BOTTOM_PANEL: {
                    if (pt.y >= 0 && pt.y <= PANEL_BORDER) {
                        SetCursor(LoadCursor(NULL, IDC_SIZENS));
                        return TRUE;
                    }
                    break;
                }
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            // Verify that the cursor is in the border
            if (!panel->resizable) break;

            RECT windowRect;
            GetClientRect(GetParent(panel->hwnd), &windowRect);
            int screenHeight = windowRect.bottom - windowRect.top;

            switch (panel->sideAssigned) {
                case LEFT_PANEL: {
                    if (pt.x >= panel->width - PANEL_BORDER && pt.x <= panel->width) {
                        SetCapture(hwnd);  // Capture the mouse
                        panel->resizingHorizontal = TRUE;  // Mark that the panel is being resized
                    } else {
                        if (panel->y + panel->height != screenHeight - 1 && pt.y <= panel->height && pt.y >= panel->height - PANEL_BORDER) {
                            SetCapture(hwnd);
                            panel->resizingVertical = TRUE;
                        }
                    }
                    break;
                }
                case RIGHT_PANEL: {
                    if (pt.x >= 0 && pt.x <= PANEL_BORDER) {
                        SetCapture(hwnd);  // Capture the mouse
                        panel->resizingHorizontal = TRUE;  // Mark that the panel is being resized
                    } else {
                        if (panel->y + panel->height != screenHeight - 1 && pt.y <= panel->height && pt.y >= panel->height - PANEL_BORDER) {
                            SetCapture(hwnd);
                            panel->resizingVertical = TRUE;
                        }
                    }
                    break;
                }
                case TOP_PANEL: {
                    if (pt.y >= panel->height - PANEL_BORDER && pt.y <= panel->height) {
                        SetCapture(hwnd);  // Capture the mouse
                        panel->resizingHorizontal = TRUE;  // Mark that the panel is being resized
                    }
                    break;
                }
                case BOTTOM_PANEL: {
                    if (pt.y >= 0&& pt.y <= PANEL_BORDER) {
                        SetCapture(hwnd);  // Capture the mouse
                        panel->resizingHorizontal = TRUE;  // Mark that the panel is being resized
                    }
                    break;
                }
            }
            
            break;
        }
        case WM_MOUSEMOVE: {
            if (panel->resizingVertical && (wParam & MK_LBUTTON)) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                
                // La altura nueva no debe ser más grande que la altura actual del panel más la altura del panel de abajo menos la altura mínima del panel de abajo.
                Panel* belowPanel = GetPanelBelow(panel);
                if (belowPanel) {
                    int maxNewHeight = panel->height + belowPanel->height - PANEL_HEADER_HEIGHT;
                    int newHeight = min(pt.y, maxNewHeight);
                    
                    // Asegurarse de que la nueva altura no sea menor que el tamaño mínimo del panel.
                    newHeight = max(newHeight, PANEL_HEADER_HEIGHT);
                    
                    // Ajustar la altura y la posición y del panel de abajo.
                    int heightDifference = newHeight - panel->height;
                    belowPanel->height -= heightDifference;
                    belowPanel->y += heightDifference;
                    
                    // Establecer la nueva altura para este panel.
                    panel->height = newHeight;
                    
                    // Actualizar los cambios con DeferWindowPos para minimizar los redibujados y el parpadeo.
                    HDWP hdwp = BeginDeferWindowPos(GetNumberPanelsInSide(panel->sideAssigned));
                    hdwp = DeferWindowPos(hdwp, panel->hwnd, NULL, panel->x, panel->y, panel->width, panel->height, SWP_NOZORDER | SWP_NOACTIVATE);
                    hdwp = DeferWindowPos(hdwp, belowPanel->hwnd, NULL, belowPanel->x, belowPanel->y, belowPanel->width, belowPanel->height, SWP_NOZORDER | SWP_NOACTIVATE);
                    EndDeferWindowPos(hdwp);
                }
                break;
            } else if (panel->resizingHorizontal && (wParam & MK_LBUTTON)) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);

                RECT parentRect;
                GetClientRect(GetParent(hwnd), &parentRect);
                int maxWidth = (parentRect.right - parentRect.left) / 3;

                switch(panel->sideAssigned) {
                    case LEFT_PANEL: {
                        int newWidth = max(PANEL_MIN_SIZE, min(maxWidth, pt.x));
                        SetWidthToPanelsInSide(LEFT_PANEL, newWidth);
                        break;
                    }
                    case RIGHT_PANEL: {
                        panel->width =  max(PANEL_MIN_SIZE + 3, min(maxWidth, panel->width - pt.x));
                        panel->x = parentRect.right - parentRect.left - panel->width;
                        SetWidthToPanelsInSide(RIGHT_PANEL, panel->width);
                        SetXToPanelsInSide(RIGHT_PANEL, panel->x);
                        break;
                    }
                    case TOP_PANEL: {
                        panel->height = max(PANEL_MIN_SIZE, pt.y);
                        break;
                    }
                    case BOTTOM_PANEL: {
                        panel->height = max(PANEL_MIN_SIZE, panel->height - pt.y);
                        RECT rect;
                        GetClientRect(GetParent(hwnd), &rect);
                        panel->y = rect.bottom - rect.top - panel->height;
                        break;
                    }
                }

                HDWP hdwp = BeginDeferWindowPos(GetNumberPanelsInSide(panel->sideAssigned));

                PanelNode* current = GetPanelList();
                while (current) {
                    if (current->panel && current->panel->sideAssigned == panel->sideAssigned) {
                        hdwp = DeferWindowPos(hdwp, current->panel->hwnd, NULL, current->panel->x, current->panel->y, current->panel->width, current->panel->height, SWP_NOZORDER | SWP_NOACTIVATE);
                    }
                    current = current->next;
                }

                EndDeferWindowPos(hdwp);
            } else {
                if (!panel->hovering) {
                    // Cursor is hovering the panel
                    panel->hovering = TRUE;

                    TRACKMOUSEEVENT tme; // Removed when MOUSELEAVE
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hwnd;
                    TrackMouseEvent(&tme);

                    RECT rect = {0, 0, panel->width, PANEL_HEADER_HEIGHT};
                    InvalidateRect(hwnd, &rect, FALSE);
                }
            }
            break;
        }
        case WM_MOUSEHOVER: {
            panel->hovering = TRUE;
            // Cursor is hovering any panel component
            RECT rect = {0, 0, panel->width, PANEL_HEADER_HEIGHT};
            InvalidateRect(hwnd, &rect, FALSE);
            break;
        }
        case WM_MOUSELEAVE: {
            // Cursor out of the panel
            panel->hovering = FALSE;
            RECT rect = {0, 0, panel->width, PANEL_HEADER_HEIGHT};
            InvalidateRect(hwnd, &rect, FALSE);
            break;
        }
        case WM_LBUTTONUP: {
            if (panel->resizingHorizontal || panel->resizingVertical) {
                ReleaseCapture();  // Release the mouse
                panel->resizingVertical = FALSE;  // Mark that the panel is not being resized
                panel->resizingHorizontal = FALSE;  // Mark that the panel is not being resized

                RECT rect = {0, 0, panel->width, PANEL_HEADER_HEIGHT};
                InvalidateRect(hwnd, &rect, FALSE);
            }
            break;
        }
        case WM_ERASEBKGND:
            return (LRESULT)1; // Mark that the background has been erased, but don't draw anything
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect;
            GetClientRect(hwnd, &rect);

            // Double buffering
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HBITMAP oldBitmap = SelectObject(memDC, memBitmap);

            // Body
            FillRect(memDC, &rect, brushBackground);

            // Header
            RECT headerRect = {0, 0, rect.right, PANEL_HEADER_HEIGHT};
            HBRUSH headerBrush = (panel->hovering) ? brushHeaderHovering : brushBackground;

            FillRect(memDC, &headerRect, headerBrush);

            // Text of header
            SetTextColor(memDC, PANEL_TITLE_TEXT_COLOR);
            SetBkMode(memDC, TRANSPARENT);
            RECT textRect = {PANEL_TITLE_TEXT_OFFSET_X, PANEL_TITLE_TEXT_OFFSET_Y, rect.right - BUTTON_WIDTH - PANEL_TITLE_TEXT_OFFSET_BUTTON, PANEL_HEADER_HEIGHT};
            DrawText(memDC, panel->name, -1, &textRect, DT_LEFT);

            // Borders
            HPEN hPen = CreatePen(PS_SOLID, 2, PANELS_BORDER_LINE_COLOR);
            HPEN hOldPen = SelectObject(memDC, hPen);
            switch(panel->sideAssigned) {
                case LEFT_PANEL:
                    MoveToEx(memDC, panel->width, 0, NULL);
                    LineTo(memDC, panel->width, panel->height);
                    MoveToEx(memDC, 0, panel->height, NULL);
                    LineTo(memDC, panel->width, panel->height);
                    break;
                case TOP_PANEL:
                    MoveToEx(memDC, 0, panel->height - 1, NULL);
                    LineTo(memDC, panel->width, panel->height - 1);
                    break;
                case RIGHT_PANEL:
                    MoveToEx(memDC, 0, 0, NULL);
                    LineTo(memDC, 0, panel->height);
                    MoveToEx(memDC, 0, panel->height, NULL);
                    LineTo(memDC, panel->width, panel->height);
                    break;
                case BOTTOM_PANEL:
                    MoveToEx(memDC, 0, 0, NULL);
                    LineTo(memDC, panel->width, 0);
                    break;
            }
            
            // Copy the contents of the memory DC to the screen
            BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

            // Cleaning
            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_SIZE: {
            if (panel) {
                int newWidth = LOWORD(lParam); // New width of the panel

                RECT oldButtonRect;
                GetClientRect(panel->minimizeButton, &oldButtonRect);
                MapWindowPoints(panel->minimizeButton, hwnd, (LPPOINT)&oldButtonRect, 2);
                InvalidateRect(panel->hwnd, NULL, TRUE); // TRUE = Erase background

                int buttonPosX = newWidth - BUTTON_OFFSET_RIGHT;
                MoveWindow(panel->minimizeButton, buttonPosX, BUTTON_OFFSET_TOP, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
                InvalidateRect(hwnd, &oldButtonRect, TRUE); // TRUE = Erase background
            }
            return 0;
        }
        case WM_MINIMIZE_PANEL: {
            switch (panel->sideAssigned) {
                case LEFT_PANEL:
                case RIGHT_PANEL: {
                    // Check if there are more panels in the same side and if they are minimized reduce the width of the panel
                    // otherwises, reduce the height of the panel (depending if is at the top, mid or bottom)
                    int numPanels = GetNumberPanelsInSide(panel->sideAssigned);

                    // RECT window
                    RECT windowRect;
                    GetClientRect(GetParent(hwnd), &windowRect);

                    int screenWidth = windowRect.right - windowRect.left;

                    if (panel->width < PANEL_MIN_SIZE + PANEL_MIN_SIZE_OFFSET) {
                        if (panel->sideAssigned == RIGHT_PANEL) {
                            SetXToPanelsInSide(panel->sideAssigned, screenWidth - PANEL_DEFAULT_SIZE);
                        }
                        SetWidthToPanelsInSide(panel->sideAssigned, PANEL_DEFAULT_SIZE);
                        // Force repaint
                        PanelNode* current = GetPanelList();
                        while (current) {
                            if (current->panel && current->panel->sideAssigned == panel->sideAssigned) {
                                MoveWindow(current->panel->hwnd, current->panel->x, current->panel->y, current->panel->width, current->panel->height, TRUE);
                            }
                            current = current->next;
                        }
                        break;
                    }

                    if (numPanels == 1) {
                        if (panel->width < PANEL_MIN_SIZE + PANEL_MIN_SIZE_OFFSET) {
                            if (panel->sideAssigned == RIGHT_PANEL) {
                                panel->x = screenWidth - PANEL_DEFAULT_SIZE - 3;
                            }
                            panel->width = PANEL_DEFAULT_SIZE;
                        } else {
                            if (panel->sideAssigned == RIGHT_PANEL) {
                                panel->x = screenWidth - PANEL_MIN_SIZE - 3; // 3 to look better in the right side
                            }
                            panel->width = PANEL_MIN_SIZE + (panel->sideAssigned == RIGHT_PANEL ? 3 : 0);
                        }
                        MoveWindow(panel->hwnd, panel->x, panel->y, panel->width, panel->height, TRUE);
                    } else {
                        int minimizedPanels = 0;
                        PanelNode* current = GetPanelList();
                        while (current) {
                            if (current->panel && current->panel->sideAssigned == panel->sideAssigned && current->panel->height == PANEL_HEADER_HEIGHT) {
                                minimizedPanels++;
                            }
                            current = current->next;
                        }

                        if (minimizedPanels == numPanels - 1 && panel->height != PANEL_HEADER_HEIGHT) {
                            // Last to minimize
                            if (panel->sideAssigned == RIGHT_PANEL) {
                                // RECT window
                                RECT windowRect;
                                GetClientRect(GetParent(hwnd), &windowRect);

                                int screenWidth = windowRect.right - windowRect.left;

                                SetXToPanelsInSide(panel->sideAssigned, screenWidth - PANEL_MIN_SIZE - 3); // 3 to look better in the right side
                            }
                            SetWidthToPanelsInSide(panel->sideAssigned, PANEL_MIN_SIZE + 3);
                            
                            // Force repaint
                            PanelNode* current = GetPanelList();
                            while (current) {
                                if (current->panel && current->panel->sideAssigned == panel->sideAssigned) {
                                    MoveWindow(current->panel->hwnd, current->panel->x, current->panel->y, current->panel->width, current->panel->height, TRUE);
                                }
                                current = current->next;
                            }
                        } else if (panel->height != PANEL_HEADER_HEIGHT) {
                            // If no panel below, adjust the current panel to minimize it completely
                            RECT screenRect;
                            GetClientRect(GetParent(hwnd), &screenRect);

                            Panel* belowPanel = GetPanelBelow(panel);
                            if (belowPanel) {
                                belowPanel->y = panel->y + PANEL_HEADER_HEIGHT;
                                belowPanel->height += panel->height - PANEL_HEADER_HEIGHT;
                                MoveWindow(belowPanel->hwnd, belowPanel->x, belowPanel->y, belowPanel->width, belowPanel->height, TRUE);
                            } else {
                                Panel* abovePanel = GetPanelAbove(panel);
                                if (abovePanel) {
                                    abovePanel->height += panel->height - PANEL_HEADER_HEIGHT;
                                    panel->y = panel->height + panel->y - PANEL_HEADER_HEIGHT;
                                    MoveWindow(abovePanel->hwnd, abovePanel->x, abovePanel->y, abovePanel->width, abovePanel->height, TRUE);
                                }
                            }

                            panel->height = PANEL_HEADER_HEIGHT;

                            MoveWindow(panel->hwnd, panel->x, panel->y, panel->width, panel->height, TRUE);
                        }
                        break;
                    }
                }
                case TOP_PANEL: {
                    panel->resizable = !panel->resizable;
                    break;
                }
                case BOTTOM_PANEL: {
                    panel->resizable = !panel->resizable;
                    break;
                }
            }
            break;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Panel* panel = FindPanelInList(GetParent(hwnd));

    switch (uMsg) {
        case WM_SETCURSOR: {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
        }
        case WM_MOUSEMOVE: {
            if (!panel->hoveringMinimizeButton) {
                panel->hoveringMinimizeButton = TRUE;
            }
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            SendMessage(GetParent(hwnd), WM_MOUSEHOVER, 0, 0);
            break;
        }
        case WM_MOUSELEAVE: {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = GetParent(hwnd);
            TrackMouseEvent(&tme);
            panel->hoveringMinimizeButton = FALSE;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case WM_LBUTTONDOWN: {
            SendMessage(GetParent(hwnd), WM_MINIMIZE_PANEL, 0, 0);
            break;
        }
        case WM_ERASEBKGND:
            return (LRESULT)1; // Mark that the background has been erased, but don't draw anything
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            // Double buffering
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HBITMAP oldBitmap = SelectObject(memDC, memBitmap);

            SetBkColor(memDC, panel->hovering ? PANEL_HEADER_HOVERING_COLOR : PANEL_BACKGROUND_COLOR);
            HBRUSH hBrush = CreateSolidBrush(GetBkColor(memDC));
            FillRect(memDC, &rect, hBrush);
            DeleteObject(hBrush);

            // Border
            int cornerRadius = 5;

            HBRUSH backgroundBrush;
            if (panel->hoveringMinimizeButton) {
                backgroundBrush = CreateSolidBrush(BUTTON_HOVERING_COLOR);
            } else if (panel->hovering) {
                backgroundBrush = brushHeaderHovering;
            } else {
                backgroundBrush = brushBackground;
            }

            HBRUSH noBorderBrush = backgroundBrush;
            HPEN transparentPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));

            HGDIOBJ originalBrush = SelectObject(memDC, noBorderBrush);
            HGDIOBJ originalPen = SelectObject(memDC, transparentPen);

            RoundRect(memDC, rect.left, rect.top, rect.right, rect.bottom, cornerRadius, cornerRadius);

            SelectObject(memDC, originalBrush);
            SelectObject(memDC, originalPen);
            DeleteObject(transparentPen);

            // Horizontal line
            HPEN hPen = CreatePen(PS_SOLID, 2, BUTTON_MINIMIZE_LINE_COLOR); 
            HGDIOBJ oldPen = SelectObject(memDC, hPen);
            MoveToEx(memDC, rect.left + 4, rect.bottom / 2 - 1, NULL);
            LineTo(memDC, rect.right - 6, rect.bottom / 2);

            BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            SelectObject(hdc, oldPen);
            DeleteObject(hPen);

            EndPaint(hwnd, &ps);
            break;
        }
    }
    return CallWindowProc(DefWindowProc, hwnd, uMsg, wParam, lParam);
}
