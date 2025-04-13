# Cloth Simulation

A real-time 2D cloth physics simulation written in C++ using Windows GDI. Features interactive controls and performance optimization.

## Features

- Real-time cloth physics simulation
- Spring-mass system with structural and diagonal springs
- Gravity, wind, and drag forces
- Interactive mouse control (click and drag cloth points)
- Double-buffered rendering with position interpolation
- Quality presets (High/Medium/Low)
- Adjustable simulation parameters
- Self-collision detection
- Wire/solid rendering modes
- FPS display and performance monitoring

## Controls

- Left-click and drag: Move cloth points
- 'R' key: Reset simulation
- 'W' key: Toggle wire/solid mode
- Top sliders: Adjust gravity, stiffness, and damping
- Quality presets: Switch between different simulation settings
- Resolution slider: Change cloth mesh density

## Physics Parameters

The simulation uses the following physics parameters, all adjustable through the UI:
- Spring stiffness: 0.0-1.0 (normalized)
- Spring damping: 0.0-1.0 (normalized)  
- Gravity: 0.0-1.0 (normalized)
- Wind force: Gentle oscillating force
- Resolution: 15-30 points per side
- Fixed top corners for stability
- Collision detection with window boundaries

## Requirements

- Windows OS
- CMake (version 3.10 or higher)
- C++ compiler with Windows SDK
- GDI32 (included with Windows)

## Building and Running

1. Clone and navigate to the repository
2. Create and enter a build directory:
```bash
mkdir build && cd build
```
3. Configure and build:
```bash
cmake ..
cmake --build .
```
4. Run the executable:
```bash
.\ClothSimulation.exe
```

## Project Structure

- `main.cpp`: Application entry, window handling, and main loop
- `Cloth.h/cpp`: Core simulation logic
- `GuiControls.h/cpp`: UI controls and parameter management

## License

MIT License - Feel free to use and modify.