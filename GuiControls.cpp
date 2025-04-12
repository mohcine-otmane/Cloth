#include "GuiControls.h"
#include <CommCtrl.h>
#include <windowsx.h>
#include <cstdio>
#pragma comment(lib, "comctl32.lib")

#ifndef TBM_GETPOS
#define TBM_GETPOS (WM_USER)
#endif

void CreateSimControls(HWND hwnd) {
    // Initialize Common Controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    const int LABEL_WIDTH = 80;
    const int SLIDER_WIDTH = 200;
    const int CONTROL_HEIGHT = 25;
    const int VALUE_WIDTH = 50;
    const int MARGIN = 10;
    const int START_X = 20;
    const int START_Y = 20;

    // Create gravity controls
    CreateWindowEx(0, "STATIC", "Gravity:", WS_CHILD | WS_VISIBLE,
        START_X, START_Y, LABEL_WIDTH, CONTROL_HEIGHT,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, TRACKBAR_CLASS, "",
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
        START_X + LABEL_WIDTH, START_Y, SLIDER_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_GRAVITY_SLIDER, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "STATIC", "50%", WS_CHILD | WS_VISIBLE | SS_RIGHT,
        START_X + LABEL_WIDTH + SLIDER_WIDTH + MARGIN, START_Y,
        VALUE_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_GRAVITY_TEXT, GetModuleHandle(NULL), NULL);

    // Create stiffness controls (Y + 35)
    CreateWindowEx(0, "STATIC", "Stiffness:", WS_CHILD | WS_VISIBLE,
        START_X, START_Y + 35, LABEL_WIDTH, CONTROL_HEIGHT,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, TRACKBAR_CLASS, "",
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
        START_X + LABEL_WIDTH, START_Y + 35, SLIDER_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_STIFFNESS_SLIDER, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "STATIC", "50%", WS_CHILD | WS_VISIBLE | SS_RIGHT,
        START_X + LABEL_WIDTH + SLIDER_WIDTH + MARGIN, START_Y + 35,
        VALUE_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_STIFFNESS_TEXT, GetModuleHandle(NULL), NULL);

    // Create damping controls (Y + 70)
    CreateWindowEx(0, "STATIC", "Damping:", WS_CHILD | WS_VISIBLE,
        START_X, START_Y + 70, LABEL_WIDTH, CONTROL_HEIGHT,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, TRACKBAR_CLASS, "",
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
        START_X + LABEL_WIDTH, START_Y + 70, SLIDER_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_DAMPING_SLIDER, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "STATIC", "50%", WS_CHILD | WS_VISIBLE | SS_RIGHT,
        START_X + LABEL_WIDTH + SLIDER_WIDTH + MARGIN, START_Y + 70,
        VALUE_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_DAMPING_TEXT, GetModuleHandle(NULL), NULL);

    // Create buttons (Y + 105)
    CreateWindowEx(0, "BUTTON", "&Reset (R)",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        START_X, START_Y + 105, 100, 30,
        hwnd, (HMENU)ID_RESET_BUTTON, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "BUTTON", "Show &Wires (W)",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        START_X + 120, START_Y + 105, 120, 30,
        hwnd, (HMENU)ID_WIRE_TOGGLE, GetModuleHandle(NULL), NULL);

    // Create resolution controls (Y + 140)
    CreateWindowEx(0, "STATIC", "Resolution:", WS_CHILD | WS_VISIBLE,
        START_X, START_Y + 140, LABEL_WIDTH, CONTROL_HEIGHT,
        hwnd, NULL, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, TRACKBAR_CLASS, "",
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS,
        START_X + LABEL_WIDTH, START_Y + 140, SLIDER_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_RESOLUTION_SLIDER, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "STATIC", "20", WS_CHILD | WS_VISIBLE | SS_RIGHT,
        START_X + LABEL_WIDTH + SLIDER_WIDTH + MARGIN, START_Y + 140,
        VALUE_WIDTH, CONTROL_HEIGHT,
        hwnd, (HMENU)ID_RESOLUTION_TEXT, GetModuleHandle(NULL), NULL);

    // Initialize sliders
    SendMessage(GetDlgItem(hwnd, ID_GRAVITY_SLIDER), TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    SendMessage(GetDlgItem(hwnd, ID_STIFFNESS_SLIDER), TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    SendMessage(GetDlgItem(hwnd, ID_DAMPING_SLIDER), TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    SendMessage(GetDlgItem(hwnd, ID_RESOLUTION_SLIDER), TBM_SETRANGE, TRUE, MAKELONG(10, 40));

    // Set initial values
    SendMessage(GetDlgItem(hwnd, ID_GRAVITY_SLIDER), TBM_SETPOS, TRUE, 50);
    SendMessage(GetDlgItem(hwnd, ID_STIFFNESS_SLIDER), TBM_SETPOS, TRUE, 50);
    SendMessage(GetDlgItem(hwnd, ID_DAMPING_SLIDER), TBM_SETPOS, TRUE, 50);
    SendMessage(GetDlgItem(hwnd, ID_RESOLUTION_SLIDER), TBM_SETPOS, TRUE, 20);
    CheckDlgButton(hwnd, ID_WIRE_TOGGLE, BST_CHECKED);
}

void UpdateSliderText(HWND hwnd, int sliderId, int textId) {
    int pos = SendMessage(GetDlgItem(hwnd, sliderId), TBM_GETPOS, 0, 0);
    char buffer[16];
    sprintf(buffer, "%d%%", pos);
    SetDlgItemText(hwnd, textId, buffer);
}

void UpdateSimParameters(HWND hwnd, int controlId) {
    // Implementation will be added later when needed
}
