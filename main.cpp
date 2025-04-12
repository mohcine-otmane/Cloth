#include <windows.h>
#include <math.h>
#include "Cloth.h"
#include "GuiControls.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

Cloth* cloth = nullptr;
bool isRunning = true;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateSimControls(hwnd);
            SendMessage(GetDlgItem(hwnd, ID_GRAVITY_SLIDER), TBM_SETPOS, TRUE, 50);
            SendMessage(GetDlgItem(hwnd, ID_STIFFNESS_SLIDER), TBM_SETPOS, TRUE, 50);
            SendMessage(GetDlgItem(hwnd, ID_DAMPING_SLIDER), TBM_SETPOS, TRUE, 50);
            return 0;

        case WM_DESTROY:
            isRunning = false;
            PostQuitMessage(0);
            return 0;

        case WM_LBUTTONDOWN:
            if (cloth) {
                cloth->HandleMouseDown(LOWORD(lParam), HIWORD(lParam));
            }
            return 0;

        case WM_MOUSEMOVE:
            if (cloth) {
                cloth->HandleMouseMove(LOWORD(lParam), HIWORD(lParam));
            }
            return 0;

        case WM_LBUTTONUP:
            if (cloth) {
                cloth->HandleMouseUp();
            }
            return 0;

        case WM_ERASEBKGND:
            return 1; // Prevent background erasing

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Create memory DC for double buffering
            HDC hdcMem = CreateCompatibleDC(hdc);
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
            
            // Clear the background
            FillRect(hdcMem, &rect, (HBRUSH)(COLOR_WINDOW + 1));
            
            // Draw the cloth
            if (cloth) {
                cloth->Draw(hdcMem);
            }
            
            // Copy the memory DC to the screen
            BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
            
            // Clean up
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_HSCROLL:
            if (cloth) {
                HWND slider = (HWND)lParam;
                int pos = SendMessage(slider, TBM_GETPOS, 0, 0);
                float value = pos / 100.0f;
                int sliderId = GetDlgCtrlID(slider);

                switch (sliderId) {
                    case ID_GRAVITY_SLIDER:
                        cloth->SetGravity(value);
                        UpdateSliderText(hwnd, sliderId, ID_GRAVITY_TEXT);
                        break;
                    case ID_STIFFNESS_SLIDER:
                        cloth->SetStiffness(value);
                        UpdateSliderText(hwnd, sliderId, ID_STIFFNESS_TEXT);
                        break;
                    case ID_DAMPING_SLIDER:
                        cloth->SetDamping(value);
                        UpdateSliderText(hwnd, sliderId, ID_DAMPING_TEXT);
                        break;
                    case ID_RESOLUTION_SLIDER: {
                        delete cloth;
                        cloth = Cloth::CreateWithResolution(pos);
                        cloth->FixPoint(0, 0);
                        cloth->FixPoint(pos - 1, 0);
                        UpdateSliderText(hwnd, sliderId, ID_RESOLUTION_TEXT);
                        break;
                    }
                }
            }
            return 0;

        case WM_CHAR:
            if (cloth) {
                switch (tolower(wParam)) {
                    case 'r':
                        cloth->Reset();
                        break;
                    case 'w':
                        {
                            bool isChecked = IsDlgButtonChecked(hwnd, ID_WIRE_TOGGLE) == BST_CHECKED;
                            CheckDlgButton(hwnd, ID_WIRE_TOGGLE, isChecked ? BST_UNCHECKED : BST_CHECKED);
                            cloth->SetWireVisibility(!isChecked);
                        }
                        break;
                }
            }
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_RESET_BUTTON && cloth) {
                cloth->Reset();
            }
            else if (LOWORD(wParam) == ID_WIRE_TOGGLE && cloth) {
                bool checked = (IsDlgButtonChecked(hwnd, ID_WIRE_TOGGLE) == BST_CHECKED);
                cloth->SetWireVisibility(checked);
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const char* CLASS_NAME = "ClothSimulation";
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW; // Add style flags for better redrawing
    
    RegisterClassA(&wc);
    
    // Calculate window size including borders
    RECT windowRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT + 170 }; // Add 170 pixels for controls
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    
    // Create the window
    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Cloth Simulation",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    
    if (hwnd == NULL) {
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Initialize the cloth
    cloth = new Cloth(20, 20, 20.0f);
    
    // Fix the top corners
    cloth->FixPoint(0, 0);
    cloth->FixPoint(19, 0);
    
    // Main loop
    MSG msg = {};
    LARGE_INTEGER frequency;
    LARGE_INTEGER lastTime;
    LARGE_INTEGER currentTime;
    
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);
    
    float accumulatedTime = 0.0f;
    const float targetFrameTime = 1.0f / 60.0f;
    
    while (isRunning) {
        // Handle messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        // Update simulation with fixed time step
        QueryPerformanceCounter(&currentTime);
        float dt = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
        lastTime = currentTime;
        
        accumulatedTime += dt;
        
        // Update simulation in fixed time steps
        while (accumulatedTime >= targetFrameTime) {
            if (cloth) {
                cloth->Update(targetFrameTime);
                
                // Add gentle wind force
                static float windTime = 0.0f;
                windTime += targetFrameTime;
                float windForce = 5.0f * sinf(windTime * 2.0f);
                cloth->AddForce(windForce, 0.0f);
            }
            accumulatedTime -= targetFrameTime;
        }
        
        // Redraw only when needed
        if (cloth) {
            InvalidateRect(hwnd, NULL, FALSE);
            UpdateWindow(hwnd); // Force immediate redraw
        }
        
        // Sleep to prevent excessive CPU usage
        Sleep(1);
    }
    
    delete cloth;
    return 0;
}