#include "Cloth.h"
#include <cmath>

Cloth::Cloth(int width, int height, float spacing)
    : width(width), height(height), spacing(spacing), draggedPoint(-1) {
    // Initialize point masses
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            PointMass pm;
            pm.x = x * spacing + 100.0f; // Offset to make it visible
            pm.y = y * spacing + 100.0f;
            pm.vx = 0;
            pm.vy = 0;
            pm.fx = 0;
            pm.fy = 0;
            pm.mass = 1.0f;
            pm.isFixed = false;
            pm.isDragged = false;
            points.push_back(pm);
        }
    }

    InitializeSprings();
}

Cloth::~Cloth() {}

void Cloth::InitializeSprings() {
    // Create structural springs
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int current = y * width + x;

            // Horizontal springs
            if (x < width - 1) {
                Spring s;
                s.point1 = current;
                s.point2 = current + 1;
                s.restLength = spacing;
                s.stiffness = 2000.0f;  // Increased stiffness
                s.damping = 0.5f;       // Increased damping
                springs.push_back(s);
            }

            // Vertical springs
            if (y < height - 1) {
                Spring s;
                s.point1 = current;
                s.point2 = current + width;
                s.restLength = spacing;
                s.stiffness = 2000.0f;  // Increased stiffness
                s.damping = 0.5f;       // Increased damping
                springs.push_back(s);
            }

            // Diagonal springs
            if (x < width - 1 && y < height - 1) {
                Spring s;
                s.point1 = current;
                s.point2 = current + width + 1;
                s.restLength = spacing * std::sqrt(2.0f);
                s.stiffness = 1000.0f;  // Increased stiffness
                s.damping = 0.5f;       // Increased damping
                springs.push_back(s);

                s.point1 = current + 1;
                s.point2 = current + width;
                springs.push_back(s);
            }
        }
    }
}

void Cloth::Update(float dt) {
    // Limit time step for stability
    dt = std::min(dt, 0.016f); // Cap at 60 FPS

    // Reset forces
    for (auto& point : points) {
        point.fx = 0;
        point.fy = 0;
    }

    ApplyGravity();
    ApplySpringForces();
    HandleCollisions();
    UpdatePositions(dt);
}

void Cloth::ApplyGravity() {
    const float gravity = 500.0f; // Reduced gravity
    for (auto& point : points) {
        if (!point.isFixed && !point.isDragged) {
            point.fy += gravity * point.mass;
        }
    }
}

void Cloth::ApplySpringForces() {
    for (const auto& spring : springs) {
        PointMass& p1 = points[spring.point1];
        PointMass& p2 = points[spring.point2];

        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float length = std::sqrt(dx * dx + dy * dy);
        
        if (length < 0.0001f) continue;
        
        float displacement = length - spring.restLength;
        float relativeVelocityX = p2.vx - p1.vx;
        float relativeVelocityY = p2.vy - p1.vy;

        // Hooke's law with damping
        float force = spring.stiffness * displacement;
        float dampingForce = spring.damping * (relativeVelocityX * dx + relativeVelocityY * dy) / length;
        float totalForce = force + dampingForce;

        float fx = (dx / length) * totalForce;
        float fy = (dy / length) * totalForce;

        if (!p1.isFixed && !p1.isDragged) {
            p1.fx += fx;
            p1.fy += fy;
        }
        if (!p2.isFixed && !p2.isDragged) {
            p2.fx -= fx;
            p2.fy -= fy;
        }
    }
}

void Cloth::HandleCollisions() {
    const float windowWidth = 800.0f;
    const float windowHeight = 600.0f;
    const float restitution = 0.3f; // Reduced restitution for less bouncy collisions

    for (auto& point : points) {
        if (point.isFixed || point.isDragged) continue;

        // Bottom collision
        if (point.y > windowHeight - 20) {
            point.y = windowHeight - 20;
            point.vy = -point.vy * restitution;
            point.vx *= 0.8f; // Add friction
        }

        // Top collision
        if (point.y < 20) {
            point.y = 20;
            point.vy = -point.vy * restitution;
            point.vx *= 0.8f; // Add friction
        }

        // Right collision
        if (point.x > windowWidth - 20) {
            point.x = windowWidth - 20;
            point.vx = -point.vx * restitution;
            point.vy *= 0.8f; // Add friction
        }

        // Left collision
        if (point.x < 20) {
            point.x = 20;
            point.vx = -point.vx * restitution;
            point.vy *= 0.8f; // Add friction
        }
    }
}

void Cloth::UpdatePositions(float dt) {
    const float damping = 0.95f; // Reduced damping
    for (auto& point : points) {
        if (point.isFixed || point.isDragged) continue;

        // Verlet integration with velocity damping
        float ax = point.fx / point.mass;
        float ay = point.fy / point.mass;

        // Update velocity with damping
        point.vx = (point.vx + ax * dt) * damping;
        point.vy = (point.vy + ay * dt) * damping;

        // Limit velocity for stability
        float maxVelocity = 1000.0f;
        float velocity = std::sqrt(point.vx * point.vx + point.vy * point.vy);
        if (velocity > maxVelocity) {
            float scale = maxVelocity / velocity;
            point.vx *= scale;
            point.vy *= scale;
        }

        // Update position
        point.x += point.vx * dt;
        point.y += point.vy * dt;
    }

    // Update dragged point position
    if (draggedPoint != -1) {
        points[draggedPoint].x = mouseX;
        points[draggedPoint].y = mouseY;
        points[draggedPoint].vx = 0;
        points[draggedPoint].vy = 0;
    }
}

void Cloth::DrawSpring(HDC hdc, const Spring& spring, const PointMass& p1, const PointMass& p2) {
    // Calculate spring stretch for color
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float length = std::sqrt(dx * dx + dy * dy);
    float stretch = length / spring.restLength;

    // Color based on stretch
    int r = (int)(255 * std::min(1.0f, stretch));
    int g = (int)(255 * std::max(0.0f, 1.0f - stretch));
    int b = 0;

    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(r, g, b));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, (int)p1.x, (int)p1.y, NULL);
    LineTo(hdc, (int)p2.x, (int)p2.y);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void Cloth::Draw(HDC hdc) {
    // Draw springs
    for (const auto& spring : springs) {
        const PointMass& p1 = points[spring.point1];
        const PointMass& p2 = points[spring.point2];
        DrawSpring(hdc, spring, p1, p2);
    }

    // Draw points
    for (const auto& point : points) {
        HBRUSH hBrush;
        if (point.isFixed) {
            hBrush = CreateSolidBrush(RGB(255, 0, 0)); // Red for fixed points
        } else if (point.isDragged) {
            hBrush = CreateSolidBrush(RGB(0, 255, 0)); // Green for dragged points
        } else {
            hBrush = CreateSolidBrush(RGB(0, 0, 0)); // Black for normal points
        }

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Ellipse(hdc, (int)point.x - 3, (int)point.y - 3,
                (int)point.x + 3, (int)point.y + 3);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);
    }
}

void Cloth::AddForce(float fx, float fy) {
    for (auto& point : points) {
        if (!point.isFixed && !point.isDragged) {
            point.fx += fx;
            point.fy += fy;
        }
    }
}

void Cloth::FixPoint(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        points[y * width + x].isFixed = true;
    }
}

void Cloth::HandleMouseDown(int x, int y) {
    float minDist = 10.0f;
    draggedPoint = -1;

    for (size_t i = 0; i < points.size(); i++) {
        float dx = points[i].x - x;
        float dy = points[i].y - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < minDist) {
            minDist = dist;
            draggedPoint = i;
        }
    }

    if (draggedPoint != -1) {
        points[draggedPoint].isDragged = true;
        mouseX = x;
        mouseY = y;
    }
}

void Cloth::HandleMouseMove(int x, int y) {
    if (draggedPoint != -1) {
        mouseX = x;
        mouseY = y;
    }
}

void Cloth::HandleMouseUp() {
    if (draggedPoint != -1) {
        points[draggedPoint].isDragged = false;
        draggedPoint = -1;
    }
} 