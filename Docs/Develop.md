# Development Guide

## Architecture Overview

Voice-Room-Live-Real-time-Report-Generator is a Windows desktop application built using **Dear ImGui** with **DirectX11** as the rendering backend. It follows an immediate mode GUI architecture.

> **Note**: This project uses Dear ImGui as the official UI library. For information about integrating other C++ UI libraries, see the [UI Library Integration Guide](ImportTheUILibrary.md).

## Project Structure

```
Voice-Room-Live-Real-time-Report-Generator/
├── Src/
│   ├── main.cpp              # Main window and application logic
│   ├── DataStructures.h      # Data structures and global variables
│   ├── Resource.h            # Control IDs and constants
│   ├── Resource.rc           # Resource file (icons, fonts)
│   ├── imgui/                # Dear ImGui library
│   │   ├── imgui.h
│   │   ├── imgui.cpp
│   │   ├── imgui_impl_win32.h/cpp
│   │   ├── imgui_impl_dx11.h/cpp
│   │   └── ...
│   ├── fonts/                # Font files
│   ├── TargetDialog.h/cpp    # Target value setting dialog
│   └── TimePickerDialog.h/cpp # Time selection dialog
└── CMakeLists.txt            # Build configuration
```

## Core Components

### 1. Main Window (main.cpp)

The main application entry point and rendering loop:

```cpp
// Main loop
while (!done) {
    // Handle Windows messages
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    // Start new ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Render UI components
    RenderHallInfoSection();
    RenderCountersSection();
    RenderHostsSection();

    // Render
    ImGui::Render();
    // ... DirectX11 rendering
}
```

### 2. Data Structures (DataStructures.h)

#### CounterData
Stores information for each counter:
```cpp
struct CounterData {
    char name[32];       // Counter name
    int currentValue;    // Current value
    int targetValue;     // Target value
};
```

#### HostData
Stores information for each host row:
```cpp
struct HostData {
    char name[64];       // Host name
    int laxin;           // New user count
    int erxiao;          // Repeat purchase count
    int jianlian;        // Connection count
    int sanguan;         // Three-way conversion count
};
```

### 3. Resource Definitions (Resource.h)

Defines all control IDs and constants:
- `MAX_HOSTS` - Maximum number of hosts (10)
- `BASE_HOST_COUNT` - Initial host count (3)

### 4. UI Sections

#### Hall Info Section
```cpp
void RenderHallInfoSection() {
    ImGui::SeparatorText("厅信息");
    // Hall name input
    ImGui::InputText("##HallName", gHallName, IM_ARRAYSIZE(gHallName));
    // Yimai input
    ImGui::InputText("##Yimai", gYimai, IM_ARRAYSIZE(gYimai));
    // Time range inputs
    ImGui::InputText("##TimeStart", gTimeStart, IM_ARRAYSIZE(gTimeStart));
    ImGui::InputText("##TimeEnd", gTimeEnd, IM_ARRAYSIZE(gTimeEnd));
}
```

#### Counters Section
```cpp
void RenderCountersSection() {
    ImGui::SeparatorText("计数器");
    for (int i = 0; i < 6; i++) {
        ImGui::Text("%s", gCounters[i].name);
        ImGui::InputInt("##Current", &gCounters[i].currentValue);
        ImGui::InputInt("##Target", &gCounters[i].targetValue);
    }
}
```

#### Hosts Section
```cpp
void RenderHostsSection() {
    ImGui::SeparatorText("主持数据");
    for (size_t i = 0; i < gHosts.size(); i++) {
        ImGui::InputText("##HostName", gHosts[i].name, IM_ARRAYSIZE(gHosts[i].name));
        ImGui::InputInt("##Laxin", &gHosts[i].laxin);
        // ... other inputs
    }
}
```

## Adding New Features

### Adding a New Counter

1. Extend the `gCounters` array in `main.cpp`
2. Add rendering code in `RenderCountersSection()`
3. Update `CopyDataToClipboard()` to include the new counter

### Adding a New UI Section

1. Create a new render function:
```cpp
void RenderMyNewSection() {
    ImGui::SeparatorText("New Section");
    // Add your UI elements here
}
```

2. Call it in the main loop:
```cpp
// In the main rendering loop
RenderMyNewSection();
```

### Adding Input Validation

```cpp
// Non-negative validation
if (value < 0) value = 0;

// Time validation
int startHour, startMin, endHour, endMin;
sscanf_s(gTimeStart, "%d:%d", &startHour, &startMin);
sscanf_s(gTimeEnd, "%d:%d", &endHour, &endMin);
// Validate and adjust
```

## Style Guidelines

- Use ImGui API conventions
- Follow the existing code structure
- Keep functions focused and modular
- Use meaningful variable and function names
- Add comments for complex logic
- Use `ImGui::PushID()`/`ImGui::PopID()` for unique widget IDs

## Debugging Tips

- Use `LOG_TEST()` macro for debug output (only in test builds)
- Use `LOG_ERROR()` for error conditions
- Use `LOG_INFO()` for initialization events
- Check ImGui demo window: `ImGui::ShowDemoWindow()`

## Common Patterns

### Creating Input Fields

```cpp
// Text input
ImGui::InputText("##Label", buffer, bufferSize);

// Integer input (non-negative)
ImGui::InputInt("##Label", &value);
if (value < 0) value = 0;

// With unique ID
ImGui::PushID(uniqueId);
ImGui::InputInt("##Value", &value);
ImGui::PopID();
```

### Creating Buttons

```cpp
if (ImGui::Button("Button Label")) {
    // Handle button click
}
```

### Layout

```cpp
// Horizontal layout
ImGui::SameLine();

// Columns
ImGui::Columns(2, "columnName", false);
ImGui::SetColumnWidth(0, 120);
ImGui::Text("Label:");
ImGui::NextColumn();
ImGui::InputText("##Input", buffer, size);
ImGui::Columns(1);

// Spacing
ImGui::Spacing();
ImGui::Separator();
```

## Build Configuration

The project uses CMake for building. Add new source files to `CMakeLists.txt`:

```cmake
add_executable(${PROJECT_NAME}
    Src/main.cpp
    Src/Resource.rc
    Src/MyNewFile.cpp  # Add new files here
)
```
