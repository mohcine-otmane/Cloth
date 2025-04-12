#include "GuiControls.h"
#include <CommCtrl.h>
#include <windowsx.h>
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

    // Create sliders and set their ranges
    HWND gravitySlider = CreateWindowEx(0, TRACKBAR_CLASS, "Gravity",
        WS_CHILD | WS_VISIBLE | TBS_HORZ,
        10, 10, 200, 30,
        hwnd, (HMENU)ID_GRAVITY_SLIDER, 
        GetModuleHandle(NULL), NULL);
    SendMessage(gravitySlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    SendMessage(gravitySlider, TBM_SETPOS, TRUE, 50);

    HWND stiffnessSlider = CreateWindowEx(0, TRACKBAR_CLASS, "Stiffness",
        WS_CHILD | WS_VISIBLE | TBS_HORZ,
        10, 50, 200, 30,
        hwnd, (HMENU)ID_STIFFNESS_SLIDER,
        GetModuleHandle(NULL), NULL);
    SendMessage(stiffnessSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    SendMessage(stiffnessSlider, TBM_SETPOS, TRUE, 50);

    HWND dampingSlider = CreateWindowEx(0, TRACKBAR_CLASS, "Damping",
        WS_CHILD | WS_VISIBLE | TBS_HORZ,
        10, 90, 200, 30,
        hwnd, (HMENU)ID_DAMPING_SLIDER,
        GetModuleHandle(NULL), NULL);
    SendMessage(dampingSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    SendMessage(dampingSlider, TBM_SETPOS, TRUE, 50);

    CreateWindowEx(0, "BUTTON", "Reset",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 130, 100, 30,
        hwnd, (HMENU)ID_RESET_BUTTON,
        GetModuleHandle(NULL), NULL);
}

void UpdateSimParameters(HWND hwnd, int controlId) {
    // Implementation will be added later when needed
}
