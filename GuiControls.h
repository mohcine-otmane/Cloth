#pragma once
#include <windows.h>

// Add manifest for modern controls
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Control IDs
#define ID_GRAVITY_SLIDER 101
#define ID_STIFFNESS_SLIDER 102
#define ID_DAMPING_SLIDER 103
#define ID_RESET_BUTTON 104

// Trackbar constants if not defined
#ifndef TBM_SETRANGE
#define TBM_SETRANGE (WM_USER+6)
#endif

#ifndef TBM_SETPOS
#define TBM_SETPOS (WM_USER+5)
#endif

#ifndef TBM_GETPOS
#define TBM_GETPOS (WM_USER)
#endif

// GUI helper functions
void CreateSimControls(HWND hwnd);
void UpdateSimParameters(HWND hwnd, int controlId);
