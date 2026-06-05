# Voice-Room-Live-Real-time-Report-Generator

## Overview

Voice-Room-Live-Real-time-Report-Generator is a real-time report generation tool developed by YunLuo, designed for Douyin (TikTok) Qianguang labor unions and other non-8-person microphone voice chat live broadcast work groups.

This project uses **Dear ImGui** as the official UI library with **DirectX11** as the rendering backend.

## Features

- Hall information management (hall name, host information)
- Real-time counter management (calling, new payment, new user acquisition, connection, repeat purchase, three-way conversion)
- Host dynamic table management (add/delete hosts)
- Time selection for shift management
- One-click data copying to clipboard
- Target value setting
- Health reminder for long shifts (over 6 hours)

## Documentation

- [Build Guide](Docs/Build.md) - How to build the project
- [Development Guide](Docs/Develop.md) - How to develop and extend the project
- [UI Library Integration Guide](Docs/ImportTheUILibrary.md) - How to integrate other C++ UI libraries

## Project Structure

```
Voice-Room-Live-Real-time-Report-Generator/
├── Build/              # Build intermediate files
├── Out/                # Output executable
│   └── data/font/      # Font files directory
├── Src/                # Source code
│   ├── main.cpp
│   ├── DataStructures.h
│   ├── Resource.h
│   ├── Resource.rc
│   ├── imgui/          # Dear ImGui library
│   ├── fonts/          # Font files
│   ├── TargetDialog.h/cpp
│   └── TimePickerDialog.h/cpp
├── Docs/               # Documentation
│   ├── Build.md
│   ├── Build_CN.md
│   ├── Develop.md
│   ├── Develop_CN.md
│   ├── ImportTheUILibrary.md
│   └── ImportTheUILibrary_CN.md
├── CMakeLists.txt
├── README.md
├── README_CN.md
└── LICENSE             # Apache License 2.0
```

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## Copyright

Copyright 2026 YunLuo
