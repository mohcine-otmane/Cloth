#pragma once
#include <windows.h>
#include <vector>
#include <cmath>

struct PointMass {
    float x, y;         // Position
    float vx, vy;       // Velocity
    float fx, fy;       // Force
    float mass;         // Mass
    bool isFixed;       // Whether this point is fixed in place
    bool isDragged;     // Whether this point is being dragged by mouse
};

struct Spring {
    int point1, point2; // Indices of connected point masses
    float restLength;   // Rest length of the spring
    float stiffness;    // Spring stiffness
    float damping;      // Spring damping
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
}; 