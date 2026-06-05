# 接入C++ UI库

## 当前UI框架

**本项目使用 Dear ImGui 作为官方 UI 库，以 DirectX11 作为渲染后端。**

Dear ImGui 是一个无臃肿的 C++ 图形用户界面库。它快速、可移植、与渲染器无关且自包含。

### Dear ImGui 的优势
- 即时模式 GUI - 简单直观的 API
- 无需外部依赖
- 可执行文件体积小
- 卓越的性能
- 易于集成和定制
- 非常适合工具和调试界面

## 当前实现

项目将 Dear ImGui 与 DirectX11 集成：

```cpp
// 初始化 DirectX11
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;

// 初始化 ImGui
ImGui::CreateContext();
ImGui_ImplWin32_Init(hwnd);
ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

// 主循环
while (!done) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    // 在此渲染 UI
    RenderUI();
    
    ImGui::Render();
    // 呈现
}
```

## 其他 C++ UI 库

### 1. Qt

Qt 是一个用于开发图形用户界面的跨平台应用程序框架。

#### 集成步骤
1. 安装 Qt（从 [qt.io](https://www.qt.io/) 下载）
2. 修改 `CMakeLists.txt`：
```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
```

3. 使用 Qt 控件重写应用程序
4. 更新 `main.cpp` 以使用 `QApplication`

### 2. wxWidgets

wxWidgets 是一个 C++ 库，允许开发者为 Windows、macOS、Linux 和其他平台创建应用程序。

#### 集成步骤
1. 安装 wxWidgets
2. 修改 `CMakeLists.txt`：
```cmake
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
```

### 3. MFC（微软基础类）

MFC 是微软用于在 C++ 中开发 Windows 应用程序的库。

#### 集成步骤
1. 在 Visual Studio 项目设置中启用 MFC
2. 使用 `CWinApp` 和 `CDialog` 类

### 4. Win32 API（原生）

原始实现使用原生 Win32 API。这种方法提供：
- 直接访问 Windows 功能
- 最小的可执行文件
- 完全控制 UI

## 迁移考虑事项

### 何时考虑切换 UI 库
- 需要跨平台支持
- 希望使用现代 UI 组件
- 复杂的布局需求
- 需要内置的主题/样式功能

### 迁移检查清单
1. **备份代码** - 在进行重大更改之前始终创建备份
2. **规划迁移** - 将现有功能映射到新的 UI 框架
3. **增量测试** - 一次迁移一个功能并进行彻底测试
4. **更新文档** - 保持 README 和开发指南为最新

## 建议

- **对于本项目**：继续使用 Dear ImGui，因其简单性和性能
- **对于跨平台需求**：考虑使用 Qt 或 wxWidgets
- **对于企业级 Windows 应用**：考虑使用 MFC

## 其他资源

- [Dear ImGui GitHub](https://github.com/ocornut/imgui)
- [Dear ImGui Wiki](https://github.com/ocornut/imgui/wiki)
- [Qt 文档](https://doc.qt.io/)
- [wxWidgets 文档](https://docs.wxwidgets.org/)
- [Win32 API 文档](https://learn.microsoft.com/zh-cn/windows/win32/)
