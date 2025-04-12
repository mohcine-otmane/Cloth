#include "Cloth.h"
#include <cmath>

Cloth::Cloth(int width, int height, float spacing)
    : width(width), height(height), spacing(spacing), draggedPoint(-1), gravityForce(500.0f), springStiffness(8000.0f), springDamping(2.0f) {
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
    InitializeFaces();
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
                s.stiffness = springStiffness;
                s.damping = springDamping;
                springs.push_back(s);
            }

            // Vertical springs
            if (y < height - 1) {
                Spring s;
                s.point1 = current;
                s.point2 = current + width;
                s.restLength = spacing;
                s.stiffness = springStiffness;
                s.damping = springDamping;
                springs.push_back(s);
            }

            // Diagonal springs
            if (x < width - 1 && y < height - 1) {
                Spring s;
                s.point1 = current;
                s.point2 = current + width + 1;
                s.restLength = spacing * std::sqrt(2.0f);
                s.stiffness = springStiffness / 2.0f;
                s.damping = springDamping * 0.75f;
                springs.push_back(s);

                s.point1 = current + 1;
                s.point2 = current + width;
                springs.push_back(s);
            }
        }
    }

    for (auto& spring : springs) {
        spring.broken = false;
        spring.stressFrames = 0;
        // Higher break threshold for structural springs
        spring.maxStretch = spring.getBreakThreshold();
    }
}

void Cloth::InitializeFaces() {
    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            int topLeft = y * width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (y + 1) * width + x;
            int bottomRight = bottomLeft + 1;

            // Add two triangles for each grid cell
            Face f1 = {topLeft, bottomLeft, topRight};
            Face f2 = {bottomLeft, bottomRight, topRight};
            faces.push_back(f1);
            faces.push_back(f2);
        }
    }
}

void Cloth::ResetSpringStress(Spring& spring) {
    spring.stressFrames = 0;
}

void Cloth::UpdateSpringStress(Spring& spring, float stretch) {
    float stressThreshold = 0.8f * spring.maxStretch;

    if (stretch > stressThreshold) {
        spring.stressFrames++;
    } else {
        spring.stressFrames = std::max(0, spring.stressFrames - 2); // Recover twice as fast
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
    CheckSpringBreaking();
    HandleSelfCollisions();
    HandleCollisions();
    UpdatePositions(dt);
}

void Cloth::ApplyGravity() {
    for (auto& point : points) {
        if (!point.isFixed && !point.isDragged) {
            point.fy += gravityForce * point.mass;
        }
    }
}

void Cloth::ApplySpringForces() {
    for (const auto& spring : springs) {
        if (spring.broken) continue;

        PointMass& p1 = points[spring.point1];
        PointMass& p2 = points[spring.point2];

        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float length = std::sqrt(dx * dx + dy * dy);

        if (length < 0.0001f) continue;

        float stretch = length / spring.restLength;
        float force = spring.stiffness * GetNonlinearForce(stretch);

        float relativeVelocityX = p2.vx - p1.vx;
        float relativeVelocityY = p2.vy - p1.vy;

        // Hooke's law with damping
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

void Cloth::CheckSpringBreaking() {
    for (auto& spring : springs) {
        if (spring.broken) continue;

        const PointMass& p1 = points[spring.point1];
        const PointMass& p2 = points[spring.point2];

        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float length = std::sqrt(dx * dx + dy * dy);
        float stretch = length / spring.restLength;

        UpdateSpringStress(spring, stretch);

        if (stretch > spring.maxStretch && spring.stressFrames >= Spring::STRESS_THRESHOLD) {
            spring.broken = true;
        }
    }
}

float Cloth::GetNonlinearForce(float stretch) const {
    const float linearRegion = 1.2f;  // Reduced from 1.5
    if (stretch <= linearRegion) {
        return (stretch - 1.0f) * 1.5f;  // Increased linear response
    } else {
        float excess = stretch - linearRegion;
        return (linearRegion - 1.0f) * 1.5f + excess * 2.5f;  // Increased non-linear response
    }
}

void Cloth::HandleSelfCollisions() {
    const float minDistance = spacing * 0.5f;

    for (size_t i = 0; i < points.size(); i++) {
        for (size_t j = i + 1; j < points.size(); j++) {
            if (CheckPointProximity(points[i], points[j])) {
                float dx = points[j].x - points[i].x;
                float dy = points[j].y - points[i].y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < minDistance && dist > 0.0001f) {
                    float moveRatio = (minDistance - dist) / (2.0f * dist);
                    float moveX = dx * moveRatio;
                    float moveY = dy * moveRatio;

                    if (!points[i].isFixed && !points[i].isDragged) {
                        points[i].x -= moveX;
                        points[i].y -= moveY;
                    }
                    if (!points[j].isFixed && !points[j].isDragged) {
                        points[j].x += moveX;
                        points[j].y += moveY;
                    }
                }
            }
        }
    }
}

bool Cloth::CheckPointProximity(const PointMass& p1, const PointMass& p2) const {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float distSquared = dx * dx + dy * dy;
    return distSquared < spacing * spacing;
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
    const float damping = 0.85f;  // Increased from 0.95 for more rigidity
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

COLORREF Cloth::GetFaceColor(const Face& face) const {
    // Calculate 2D edge vectors
    const PointMass& p1 = points[face.p1];
    const PointMass& p2 = points[face.p2];
    const PointMass& p3 = points[face.p3];
    
    float dx1 = p2.x - p1.x;
    float dy1 = p2.y - p1.y;
    float dx2 = p3.x - p1.x;
    float dy2 = p3.y - p1.y;
    
    // Calculate 2D cross product for orientation
    float crossProduct = dx1 * dy2 - dx2 * dy1;
    
    // Calculate area-based shading
    float area = std::abs(crossProduct) / 2.0f;
    float baseArea = spacing * spacing / 2.0f;
    float stretch = area / baseArea;
    
    // Map stretch to intensity
    float intensity = 0.3f + 0.7f * (1.0f / (1.0f + stretch * 0.5f));
    intensity = std::min(1.0f, std::max(0.3f, intensity));
    
    int color = (int)(intensity * 255);
    return RGB(color, color, color);
}

void Cloth::DrawFace(HDC hdc, const Face& face) {
    POINT points[3];
    points[0].x = (LONG)this->points[face.p1].x;
    points[0].y = (LONG)this->points[face.p1].y;
    points[1].x = (LONG)this->points[face.p2].x;
    points[1].y = (LONG)this->points[face.p2].y;
    points[2].x = (LONG)this->points[face.p3].x;
    points[2].y = (LONG)this->points[face.p3].y;
    
    HBRUSH hBrush = CreateSolidBrush(GetFaceColor(face));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    
    Polygon(hdc, points, 3);
    
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}

void Cloth::DrawSpring(HDC hdc, const Spring& spring, const PointMass& p1, const PointMass& p2) {
    if (spring.broken) return;

    // Calculate spring stretch for color
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float length = std::sqrt(dx * dx + dy * dy);
    float stretch = length / spring.restLength;

    // Enhanced color gradient
    float tension = (stretch - 1.0f) / (spring.maxStretch - 1.0f);
    tension = std::min(1.0f, std::max(0.0f, tension));

    int r = (int)(255 * tension);
    int g = (int)(255 * (1.0f - tension));
    int b = (int)(255 * (1.0f - tension * 0.5f));

    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(r, g, b));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, (int)p1.x, (int)p1.y, NULL);
    LineTo(hdc, (int)p2.x, (int)p2.y);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void Cloth::Draw(HDC hdc) {
    // Draw faces first
    for (const auto& face : faces) {
        DrawFace(hdc, face);
    }
    
    // Draw springs on top with reduced opacity
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

void Cloth::SetGravity(float g) {
    gravityForce = g * 1000.0f; // Scale for better slider control
}

void Cloth::SetStiffness(float s) {
    springStiffness = s * 10000.0f; // Scale for better slider control
    for (auto& spring : springs) {
        spring.stiffness = springStiffness;
    }
}

void Cloth::SetDamping(float d) {
    springDamping = d * 2.0f; // Scale for better slider control
    for (auto& spring : springs) {
        spring.damping = springDamping;
    }
}

void Cloth::Reset() {
    // Reset points to initial positions
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            points[i].x = x * spacing + 100.0f;
            points[i].y = y * spacing + 100.0f;
            points[i].vx = 0;
            points[i].vy = 0;
            points[i].fx = 0;
            points[i].fy = 0;
        }
    }
    // Reset springs
    for (auto& spring : springs) {
        spring.broken = false;
        spring.stressFrames = 0;
    }
}