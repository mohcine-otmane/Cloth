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

class Cloth {
private:
    std::vector<PointMass> points;
    std::vector<Spring> springs;
    int width, height;
    float spacing;
    int draggedPoint;   // Index of the point being dragged
    float mouseX, mouseY; // Current mouse position

    void InitializeSprings();
    void ApplySpringForces();
    void ApplyGravity();
    void UpdatePositions(float dt);
    void HandleCollisions();
    void DrawSpring(HDC hdc, const Spring& spring, const PointMass& p1, const PointMass& p2);
    void HandleSelfCollisions();  // New: self-collision detection
    float GetNonlinearForce(float stretch) const;  // New: non-linear spring force
    void CheckSpringBreaking();  // New: check for spring breaks
    bool CheckPointProximity(const PointMass& p1, const PointMass& p2) const;
    void ResetSpringStress(Spring& spring);
    void UpdateSpringStress(Spring& spring, float stretch);

public:
    Cloth(int width, int height, float spacing);
    ~Cloth();

    void Update(float dt);
    void Draw(HDC hdc);
    void AddForce(float fx, float fy);
    void FixPoint(int x, int y);
    void HandleMouseDown(int x, int y);
    void HandleMouseMove(int x, int y);
    void HandleMouseUp();
    void SetMaxStretch(float ratio);  // New: set max stretch ratio
};