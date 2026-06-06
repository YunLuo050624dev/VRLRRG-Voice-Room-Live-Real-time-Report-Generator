# 开发指南

## 架构概述

Voice-Room-Live-Real-time-Report-Generator 是使用 **Dear ImGui** 构建的 Windows 桌面应用程序，以 **DirectX11** 作为渲染后端。它采用即时模式 GUI 架构。

> **注意**：本项目使用 Dear ImGui 作为官方 UI 库。关于接入其他 C++ UI 库的信息，请参阅 [UI库接入指南](ImportTheUILibrary_CN.md)。

## 项目结构

```
Voice-Room-Live-Real-time-Report-Generator/
├── Src/
│   ├── main.cpp              # 主窗口和应用程序逻辑
│   ├── DataStructures.h      # 数据结构和全局变量
│   ├── Resource.h            # 控件 ID 和常量
│   ├── Resource.rc           # 资源文件（图标、字体）
│   ├── imgui/                # Dear ImGui 库
│   │   ├── imgui.h
│   │   ├── imgui.cpp
│   │   ├── imgui_impl_win32.h/cpp
│   │   ├── imgui_impl_dx11.h/cpp
│   │   └── ...
│   ├── fonts/                # 字体文件
│   ├── TargetDialog.h/cpp    # 目标值设置对话框
│   └── TimePickerDialog.h/cpp # 时间选择对话框
└── CMakeLists.txt            # 构建配置
```

## 核心组件

### 1. 主窗口 (main.cpp)

主应用程序入口和渲染循环：

```cpp
// 主循环
while (!done) {
    // 处理 Windows 消息
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    // 开始新的 ImGui 帧
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 渲染 UI 组件
    RenderHallInfoSection();
    RenderCountersSection();
    RenderHostsSection();

    // 渲染
    ImGui::Render();
    // ... DirectX11 渲染
}
```

### 2. 数据结构 (DataStructures.h)

#### CounterData
存储每个计数器的信息：
```cpp
struct CounterData {
    char name[32];       // 计数器名称
    int currentValue;    // 当前值
    int targetValue;     // 目标值
};
```

#### HostData
存储每个主持行的信息：
```cpp
struct HostData {
    char name[64];       // 主持名称
    int laxin;           // 拉新数
    int erxiao;          // 二消数
    int jianlian;        // 建联数
    int sanguan;         // 三关数
};
```

### 3. 资源定义 (Resource.h)

定义所有控件 ID 和常量：
- `MAX_HOSTS` - 最大主持数 (10)
- `BASE_HOST_COUNT` - 初始主持数 (3)

### 4. UI 区域

#### 厅信息区域
```cpp
void RenderHallInfoSection() {
    ImGui::SeparatorText("厅信息");
    // 厅名称输入
    ImGui::InputText("##HallName", gHallName, IM_ARRAYSIZE(gHallName));
    // 一麦输入
    ImGui::InputText("##Yimai", gYimai, IM_ARRAYSIZE(gYimai));
    // 档时输入
    ImGui::InputText("##TimeStart", gTimeStart, IM_ARRAYSIZE(gTimeStart));
    ImGui::InputText("##TimeEnd", gTimeEnd, IM_ARRAYSIZE(gTimeEnd));
}
```

#### 计数器区域
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

#### 主持数据区域
```cpp
void RenderHostsSection() {
    ImGui::SeparatorText("主持数据");
    for (size_t i = 0; i < gHosts.size(); i++) {
        ImGui::InputText("##HostName", gHosts[i].name, IM_ARRAYSIZE(gHosts[i].name));
        ImGui::InputInt("##Laxin", &gHosts[i].laxin);
        // ... 其他输入
    }
}
```

## 添加新功能

### 添加新计数器

1. 在 `main.cpp` 中扩展 `gCounters` 数组
2. 在 `RenderCountersSection()` 中添加渲染代码
3. 更新 `CopyDataToClipboard()` 以包含新计数器

### 添加新 UI 区域

1. 创建新的渲染函数：
```cpp
void RenderMyNewSection() {
    ImGui::SeparatorText("新区域");
    // 在此添加您的 UI 元素
}
```

2. 在主循环中调用它：
```cpp
// 在主渲染循环中
RenderMyNewSection();
```

### 添加输入验证

```cpp
// 非负验证
if (value < 0) value = 0;

// 时间验证
int startHour, startMin, endHour, endMin;
sscanf_s(gTimeStart, "%d:%d", &startHour, &startMin);
sscanf_s(gTimeEnd, "%d:%d", &endHour, &endMin);
// 验证并调整
```

### 处理中文编码

ImGui 使用 UTF-8 编码存储字符串。当需要将字符串复制到剪贴板或其他 Windows API 时，需要进行编码转换：

```cpp
// UTF-8 转 UTF-16 辅助函数
auto utf8ToUtf16 = [](const char* utf8) -> std::wstring {
    if (!utf8 || *utf8 == '\0') return L"";
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (len <= 1) return L"";
    std::wstring result(len - 1, L'\0'); // 排除终止符
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, &result[0], len);
    return result;
};

// 使用示例
std::wstring wstr = utf8ToUtf16(gHallName);
```

**注意：** `MultiByteToWideChar` 返回的长度包含终止符，创建 `std::wstring` 时需要使用 `len - 1` 排除它，否则会导致字符串截断。

## 风格指南

- 使用 ImGui API 约定
- 遵循现有代码结构
- 保持函数专注和模块化
- 使用有意义的变量和函数名
- 为复杂逻辑添加注释
- 使用 `ImGui::PushID()`/`ImGui::PopID()` 确保控件 ID 唯一

## 调试技巧

- 使用 `LOG_TEST()` 宏输出调试信息（仅测试版）
- 使用 `LOG_ERROR()` 记录错误情况
- 使用 `LOG_INFO()` 记录初始化事件
- 查看 ImGui 演示窗口：`ImGui::ShowDemoWindow()`

## 常见模式

### 创建输入字段

```cpp
// 文本输入
ImGui::InputText("##Label", buffer, bufferSize);

// 整数输入（非负）
ImGui::InputInt("##Label", &value);
if (value < 0) value = 0;

// 使用唯一 ID
ImGui::PushID(uniqueId);
ImGui::InputInt("##Value", &value);
ImGui::PopID();
```

### 创建按钮

```cpp
if (ImGui::Button("按钮标签")) {
    // 处理按钮点击
}
```

### 布局

```cpp
// 水平布局
ImGui::SameLine();

// 列布局
ImGui::Columns(2, "columnName", false);
ImGui::SetColumnWidth(0, 120);
ImGui::Text("标签:");
ImGui::NextColumn();
ImGui::InputText("##Input", buffer, size);
ImGui::Columns(1);

// 间距
ImGui::Spacing();
ImGui::Separator();
```

## 构建配置

项目使用 CMake 进行构建。将新源文件添加到 `CMakeLists.txt`：

```cmake
add_executable(${PROJECT_NAME}
    Src/main.cpp
    Src/Resource.rc
    Src/MyNewFile.cpp  # 在此添加新文件
)
```
