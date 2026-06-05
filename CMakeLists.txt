cmake_minimum_required(VERSION 3.10)
project(Voice-Room-Live-Real-time-Report-Generator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==============================
# Build Configuration
# Test build: Console mode (with CMD window, for debugging)
# Release build: WIN32 mode (no CMD window, for distribution)
# ==============================

# Test build configuration (uncomment for debug, comment for release)
add_executable(${PROJECT_NAME}
    Src/main.cpp
)
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)

# Release build configuration (uncomment for release, comment for debug)
# add_executable(${PROJECT_NAME} WIN32
#     Src/main.cpp
# )

# IMGUI source files
set(IMGUI_SOURCES
    Src/imgui/imgui.cpp
    Src/imgui/imgui_draw.cpp
    Src/imgui/imgui_tables.cpp
    Src/imgui/imgui_widgets.cpp
    Src/imgui/imgui_impl_win32.cpp
    Src/imgui/imgui_impl_dx11.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SOURCES})

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Out)

# Static linking
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/Src
    ${CMAKE_SOURCE_DIR}/Src/imgui
)

# DirectX11 libraries
target_link_libraries(${PROJECT_NAME} 
    d3d11
    dxgi
    d3dcompiler
    dwmapi
)

# Windows libraries
target_link_libraries(${PROJECT_NAME} 
    comctl32
    user32
    gdi32
    shell32
    ole32
    oleaut32
)

# Unicode support
add_definitions(-DUNICODE -D_UNICODE)

# IMGUI configuration
add_definitions(-DIMGUI_IMPL_WIN32_DISABLE_GAMEPAD)
