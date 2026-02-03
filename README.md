# Black Hole Simulation
A project using OpenGL to graphically simulate a black hole (more specifically the gravitational effects of a black hole) using real physics.

## Build & Run
This project uses CMake to generate build files and `compile_commands.json` for clangd.

Configure (first time or after CMake changes):
```bash
CC=clang cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Build:
```bash
cmake --build build
```

Run (custom target):
```bash
cmake --build build --target run
```

## Tooling Notes
- `compile_commands.json` is generated in `build/` by CMake and is symlinked at the project root so clangd picks it up automatically.
- GLAD is compiled as a separate source (`glad/glad.c`) and included via headers (`<glad/glad.h>`).
- This setup assumes GLFW is installed and discoverable by CMake (`find_package(glfw3 REQUIRED)`).

## Project Structure
- `src/` contains the main application sources.
- `glad/` contains the GLAD source and headers.
- `build/` contains generated build files and `compile_commands.json`.
