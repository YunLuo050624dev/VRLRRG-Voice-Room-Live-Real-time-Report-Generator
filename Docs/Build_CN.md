# 编译指南

## 前置要求

- MinGW-w64（包含 gcc, g++, mingw32-make）
- CMake 3.10 或更高版本

## 编译步骤

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

## 编译配置

### 测试版（带控制台窗口）

测试版包含控制台窗口用于调试输出，适用于开发阶段。

**编译方法：**
1. 打开 `CMakeLists.txt`
2. 确保测试版配置未被注释：
   ```cmake
   add_executable(${PROJECT_NAME}
       Src/main.cpp
       Src/Resource.rc
   )
   target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
   ```
3. 确保正式版配置被注释：
   ```cmake
   # add_executable(${PROJECT_NAME} WIN32
   #     Src/main.cpp
   #     Src/Resource.rc
   # )
   ```

**特性：**
- 控制台窗口显示调试输出
- 便于追踪程序执行流程
- 适用于开发和调试

### 正式版（无控制台窗口）

正式版是纯 GUI 应用，没有控制台窗口，适用于生产环境。

**编译方法：**
1. 打开 `CMakeLists.txt`
2. 确保正式版配置未被注释：
   ```cmake
   add_executable(${PROJECT_NAME} WIN32
       Src/main.cpp
       Src/Resource.rc
   )
   ```
3. 确保测试版配置被注释：
   ```cmake
   # add_executable(${PROJECT_NAME}
   #     Src/main.cpp
   #     Src/Resource.rc
   # )
   # target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
   ```

**特性：**
- 干净的纯 GUI 应用
- 无控制台窗口
- 适用于最终用户

## 字体文件部署

应用程序需要字体文件才能正确显示中文字符。

**目录结构：**
```
Out/
├── Voice-Room-Live-Real-time-Report-Generator.exe
└── data/
    └── font/
        └── Main.ttf
```

**设置步骤：**
1. 创建字体目录：`Out/data/font/`
2. 将字体文件 `Src/fonts/Main.ttf` 复制到 `Out/data/font/Main.ttf`

## 清理编译产物

```cmd
rd /s /q build
rd /s /q Out
```

## 输出

编译成功后，可执行文件位于 `Out/Voice-Room-Live-Real-time-Report-Generator.exe`

## 依赖项

本项目使用以下库：
- **Dear ImGui** - 即时模式 GUI 库（包含在 `Src/imgui/` 中）
- **DirectX11** - 用于渲染的图形 API（系统库）

## 可移植性

项目完全可移植。您可以将整个项目目录移动到任何位置，无需修改任何配置文件。所有路径都使用 CMake 相对路径配置。
