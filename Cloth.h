#pragma once
#include <windows.h>
#include <vector>
#include <cmath>

struct PointMass {
    float x, y;         // Position
    float vx, vy;       // Velocity
    float fx, fy;       // Force
    float mass; 
    bool isFixed;       // Whether this point is fixed in place
    bool isDragged;     // Whether this point is being dragged by mouse
    float prevX, prevY;    // Previous position for interpolation
    float renderX, renderY; // Interpolated position for rendering
};

struct Spring {
    int point1, point2; // Indices of connected point masses
    float restLength;
    float stiffness;
    float damping;
    bool broken;        // New: track if spring is broken
    float maxStretch;   // New: maximum stretch ratio before breaking
    int stressFrames;   // Track consecutive high-stress frames
    static const int STRESS_THRESHOLD = 30; // Frames before breaking
    float getBreakThreshold() const {
        return (point2 - point1 == 1) ? 30.5f : 20.8f; // Higher threshold for structural springs
    }
};

struct Face {
    int p1, p2, p3;  // Indices of three points forming a triangle
};

class Cloth {
private:
    std::vector<PointMass> points;
    std::vector<Spring> springs;
    std::vector<Face> faces;
    int width, height;
    float spacing;
    int draggedPoint;   // Index of the point being dragged
    float mouseX, mouseY; // Current mouse position
    float gravityForce;
    float springStiffness;
    float springDamping;
    bool showWires;
    float accumulator;     // Time accumulator for interpolation

    void InitializeSprings();
    void InitializeFaces();
    void ApplySpringForces();
    void ApplyGravity();
    void UpdatePositions(float dt);
    void HandleCollisions();
    void DrawSpring(HDC hdc, const Spring& spring, const PointMass& p1, const PointMass& p2);
    void DrawFace(HDC hdc, const Face& face);
    void HandleSelfCollisions();  // New: self-collision detection
    float GetNonlinearForce(float stretch) const;  // New: non-linear spring force
    void CheckSpringBreaking();  // New: check for spring breaks
    bool CheckPointProximity(const PointMass& p1, const PointMass& p2) const;
    void ResetSpringStress(Spring& spring);
    void UpdateSpringStress(Spring& spring, float stretch);
    COLORREF GetFaceColor(const Face& face) const;
    void UpdateInterpolation(float alpha);

public:
    Cloth(int width, int height, float spacing);
    ~Cloth();

    void Update(float dt, float alpha = 1.0f);
    void Draw(HDC hdc);
    void AddForce(float fx, float fy);
    void FixPoint(int x, int y);
    void HandleMouseDown(int x, int y);
    void HandleMouseMove(int x, int y);
    void HandleMouseUp();
    void SetMaxStretch(float ratio);  // New: set max stretch ratio
    void SetGravity(float g);
    void SetStiffness(float s);
    void SetDamping(float d);
    void Reset();
    void SetWireVisibility(bool visible) { showWires = visible; }
    bool GetWireVisibility() const { return showWires; }
    void SetResolution(int newWidth, int newHeight);
    static Cloth* CreateWithResolution(int resolution);
};