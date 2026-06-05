# Build Guide

## Prerequisites

- MinGW-w64 (includes gcc, g++, mingw32-make)
- CMake 3.10 or higher

## Build Steps

### Windows (MinGW)

```cmd
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

### Windows (Visual Studio)

```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Build Configuration

### Test Version (with Console Window)

The test version includes a console window for debugging output. This is useful during development.

**How to build:**
1. Open `CMakeLists.txt`
2. Ensure the test version configuration is uncommented:
   ```cmake
   add_executable(${PROJECT_NAME}
       Src/main.cpp
       Src/Resource.rc
   )
   target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
   ```
3. Ensure the formal version configuration is commented out:
   ```cmake
   # add_executable(${PROJECT_NAME} WIN32
   #     Src/main.cpp
   #     Src/Resource.rc
   # )
   ```

**Features:**
- Console window shows debug output
- Easy to trace program execution flow
- Useful for development and debugging

### Release Version (without Console Window)

The release version is a pure GUI application without any console window. This is suitable for production use.

**How to build:**
1. Open `CMakeLists.txt`
2. Ensure the formal version configuration is uncommented:
   ```cmake
   add_executable(${PROJECT_NAME} WIN32
       Src/main.cpp
       Src/Resource.rc
   )
   ```
3. Ensure the test version configuration is commented out:
   ```cmake
   # add_executable(${PROJECT_NAME}
   #     Src/main.cpp
   #     Src/Resource.rc
   # )
   # target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
   ```

**Features:**
- Clean GUI-only application
- No console window
- Suitable for end-users

## Font Files Deployment

The application requires font files to display Chinese characters correctly.

**Directory structure:**
```
Out/
├── Voice-Room-Live-Real-time-Report-Generator.exe
└── data/
    └── font/
        └── Main.ttf
```

**Setup:**
1. Create the font directory: `Out/data/font/`
2. Copy the font file `Src/fonts/Main.ttf` to `Out/data/font/Main.ttf`

## Clean Build Artifacts

```cmd
rd /s /q build
rd /s /q Out
```

## Output

After successful build, the executable is located at `Out/Voice-Room-Live-Real-time-Report-Generator.exe`

## Dependencies

This project uses the following libraries:
- **Dear ImGui** - Immediate mode GUI library (included in `Src/imgui/`)
- **DirectX11** - Graphics API for rendering (system library)

## Portability

The project is fully portable. You can move the entire project directory to any location without modifying any configuration files. All paths are configured using CMake relative paths.
