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
#define ID_WIRE_TOGGLE 105
#define ID_RESOLUTION_SLIDER 106
#define ID_RESOLUTION_TEXT 107
#define ID_GRAVITY_TEXT 108
#define ID_STIFFNESS_TEXT 109
#define ID_DAMPING_TEXT 110
#define ID_QUALITY_SLIDER 111
#define ID_QUALITY_TEXT 112
#define ID_FPS_TEXT 113
#define ID_PRESET_HIGH 114
#define ID_PRESET_MEDIUM 115
#define ID_PRESET_LOW 116

// Add optimization preset struct
struct SimulationPreset {
    int resolution;
    float gravity;
    float stiffness;
    float damping;
    bool showWires;
};

extern const SimulationPreset HIGH_PRESET;
extern const SimulationPreset MEDIUM_PRESET;
extern const SimulationPreset LOW_PRESET;

void ApplyPreset(HWND hwnd, const SimulationPreset& preset);

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
void UpdateSliderText(HWND hwnd, int sliderId, int textId);
