# 编译指南

## 前置要求

- MinGW-w64（包含 gcc, g++, mingw32-make）
- CMake 3.15 或更高版本

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
       Src/TargetDialog.cpp
       Src/TimePickerDialog.cpp
   )
   ```
3. 确保正式版配置被注释：
   ```cmake
   # add_executable(${PROJECT_NAME} WIN32 
   #     Src/main.cpp
   #     Src/TargetDialog.cpp
   #     Src/TimePickerDialog.cpp
   # )
   ```
4. 确保 `Src/main.cpp` 中的 `printf` 调试语句未被注释

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
       Src/TargetDialog.cpp
       Src/TimePickerDialog.cpp
   )
   ```
3. 确保测试版配置被注释：
   ```cmake
   # add_executable(${PROJECT_NAME} 
   #     Src/main.cpp
   #     Src/TargetDialog.cpp
   #     Src/TimePickerDialog.cpp
   # )
   ```
4. 注释掉 `Src/main.cpp` 中所有标记为 "正式版请注释" 的 `printf` 调试语句

**特性：**
- 干净的纯 GUI 应用
- 无控制台窗口
- 适用于最终用户

## 清理编译产物

```cmd
rd /s /q build
rd /s /q Out
```

## 输出

编译成功后，可执行文件位于 `Out/Voice-Room-Live-Real-time-Report-Generator.exe`

## 可移植性

项目完全可移植。您可以将整个项目目录移动到任何位置，无需修改任何配置文件。所有路径都使用 CMake 相对路径配置。
