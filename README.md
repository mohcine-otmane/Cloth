# Cloth Simulation

A real-time 2D cloth simulation using GDI32 in C++. This project demonstrates a physics-based cloth simulation with interactive features.

## Features

- Real-time cloth physics simulation
- Spring-mass system with structural and diagonal springs
- Gravity and wind forces
- Mouse interaction (click and drag points)
- Collision detection with window boundaries
- Double-buffered rendering for smooth animation
- Color-coded visualization of spring tension

## Requirements

- Windows OS
- CMake (version 3.10 or higher)
- MinGW or Visual Studio compiler
- GDI32 (included with Windows)

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/yourusername/Cloth.git
cd Cloth
```

2. Create a build directory and run CMake:
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
```

3. Build the project:
```bash
cmake --build .
```

## Running the Simulation

After building, run the executable:
```bash
.\ClothSimulation.exe
```

## Controls

- Left-click and drag: Move cloth points
- The top corners are fixed in place
- The cloth responds to gravity and wind forces
- Points bounce off window boundaries

## Physics Parameters

The simulation uses the following physics parameters:
- Spring stiffness: 2000.0 (structural), 1000.0 (diagonal)
- Spring damping: 0.5
- Gravity: 500.0
- Wind force: 5.0 * sin(time)
- Collision restitution: 0.3
- Velocity damping: 0.95

## Project Structure

- `main.cpp`: Main application and window handling
- `Cloth.h`: Cloth simulation class definition
- `Cloth.cpp`: Cloth simulation implementation
- `CMakeLists.txt`: Build configuration

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. 