// Voice Room Live Real-time Report Generator
// IMGUI Version - v0.9.2

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "settings.h"
#include <d3d11.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <ctime>
#include <cstdio>

// Forward declaration for WndProc handler (required by IMGUI)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ==============================
// DirectX11 Global Variables
// ==============================
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

// Forward declarations
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Font reload function - sets flag for delayed reload
extern float gPendingFontSize;

void ReloadFont(float fontSize) {
    gPendingFontSize = fontSize;
}

// ==============================
// Application Data Structures
// ==============================
struct CounterData {
    int currentValue = 0;
    int targetValue = 0;
    std::string name;
};

struct HostData {
    char name[64] = "";
    int laxin = 0;
    int erxiao = 0;
    int jianlian = 0;
    int sanguan = 0;
};

// ==============================
// Global Application State
// ==============================
const int BASE_HOST_COUNT = 3;
const int MAX_HOSTS = 10;

// Hall info
char gHallName[128] = "";
char gYimai[128] = "";
char gTimeStart[8] = "00:00";
char gTimeEnd[8] = "03:00";

// Counters (0=Hanhuo, 1=Xinfu, 2=Laxin, 3=Jianlian, 4=Erxiao, 5=Sanguan)
CounterData gCounters[6] = {
    {0, 0, "Hanhuo"},
    {0, 0, "Xinfu"},
    {0, 0, "Laxin"},
    {0, 0, "Jianlian"},
    {0, 0, "Erxiao"},
    {0, 0, "Sanguan"}
};

// Hosts
std::vector<HostData> gHosts(BASE_HOST_COUNT);

// Copy time
char gLastCopyTime[128] = "";

// Time picker state
bool gShowTimePickerStart = false;
bool gShowTimePickerEnd = false;
bool gShowHealthReminder = false;
int gSelectedHour = 0;
int gSelectedMinute = 0;

// Font reload state
float gPendingFontSize = 0.0f;

// Settings window state
bool gShowSettings = false;

// Target dialog state
bool gShowTargetDialog = false;
int gTargetCounterIndex = -1;
int gTargetInputValue = 0;

// ==============================
// Debug Logging
// ==============================
#ifdef _DEBUG
#define LOG_INFO(fmt, ...) printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_TEST(fmt, ...) printf("[TEST] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#define LOG_TEST(fmt, ...)
#define LOG_ERROR(fmt, ...)
#endif

// ==============================
// Helper Functions
// ==============================
void UpdateSummaryCounters() {
    int totalLaxin = 0, totalErxiao = 0, totalJianlian = 0, totalSanguan = 0;
    
    for (const auto& host : gHosts) {
        totalLaxin += host.laxin;
        totalErxiao += host.erxiao;
        totalJianlian += host.jianlian;
        totalSanguan += host.sanguan;
    }
    
    gCounters[2].currentValue = totalLaxin;  // Laxin
    gCounters[3].currentValue = totalJianlian; // Jianlian
    gCounters[4].currentValue = totalErxiao;  // Erxiao
    gCounters[5].currentValue = totalSanguan; // Sanguan
    
    LOG_TEST("Summary counters updated: Laxin=%d, Jianlian=%d, Erxiao=%d, Sanguan=%d", 
             totalLaxin, totalJianlian, totalErxiao, totalSanguan);
}

void CopyDataToClipboard() {
    LOG_TEST("Copy data button clicked");
    
    std::wstring output;
    
    // Helper function to convert UTF-8 to UTF-16
    auto utf8ToUtf16 = [](const char* utf8) -> std::wstring {
        if (!utf8 || *utf8 == '\0') return L"";
        int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
        if (len <= 1) return L""; // Only null terminator or error
        std::wstring result(len - 1, L'\0'); // Exclude null terminator
        MultiByteToWideChar(CP_UTF8, 0, utf8, -1, &result[0], len);
        return result;
    };
    
    // Hall info
    output += L"厅名称：" + utf8ToUtf16(gHallName) + L"\r\n";
    output += L"一麦：" + utf8ToUtf16(gYimai) + L"\r\n";
    output += L"档时：" + utf8ToUtf16(gTimeStart) + L"-" + utf8ToUtf16(gTimeEnd) + L"\r\n";
    
    // Counters
    output += L"喊活：" + std::to_wstring(gCounters[0].currentValue) + L"/" + std::to_wstring(gCounters[0].targetValue) + L"\r\n";
    output += L"新付：" + std::to_wstring(gCounters[1].currentValue) + L"/" + std::to_wstring(gCounters[1].targetValue) + L"\r\n";
    output += L"拉新：" + std::to_wstring(gCounters[2].currentValue) + L"/" + std::to_wstring(gCounters[2].targetValue) + L"\r\n";
    output += L"建联：" + std::to_wstring(gCounters[3].currentValue) + L"/" + std::to_wstring(gCounters[3].targetValue) + L"\r\n";
    output += L"二消：" + std::to_wstring(gCounters[4].currentValue) + L"/" + std::to_wstring(gCounters[4].targetValue) + L"\r\n";
    output += L"三关：" + std::to_wstring(gCounters[5].currentValue) + L"/" + std::to_wstring(gCounters[5].targetValue) + L"\r\n";
    
    // Host data
    for (size_t i = 0; i < gHosts.size(); i++) {
        std::wstring hostName = utf8ToUtf16(gHosts[i].name);
        if (hostName.empty()) {
            hostName = L"NULL";
        }
        output += hostName + L"：拉新:" + std::to_wstring(gHosts[i].laxin) + L" 二消:" + std::to_wstring(gHosts[i].erxiao) + L" 建联:" + std::to_wstring(gHosts[i].jianlian) + L" 三关:" + std::to_wstring(gHosts[i].sanguan) + L"\r\n";
    }
    
    // Copy to clipboard (Unicode)
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        size_t size = (output.size() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
        if (hMem) {
            wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
            memcpy(pMem, output.c_str(), size);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
        CloseClipboard();
        
        // Update copy time
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        sprintf_s(gLastCopyTime, "已于%04d年%02d月%02d日%02d时%02d分%02d秒复制数据",
                  t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                  t->tm_hour, t->tm_min, t->tm_sec);
        
        LOG_TEST("Data copied to clipboard");
    }
}

// ==============================
// UI Rendering Functions
// ==============================
void RenderHallInfoSection() {
    // Hall info section with settings button
    float availableWidth = ImGui::GetContentRegionAvail().x;
    
    // Title with settings button
    ImGui::BeginGroup();
    ImGui::Text("厅信息");
    ImGui::SameLine(availableWidth - 60);
    if (ImGui::Button("设置")) {
        gShowSettings = !gShowSettings;
    }
    ImGui::EndGroup();
    
    ImGui::Separator();
    
    // Row 1: Hall name
    ImGui::Columns(2, "hall_info_row1", false);
    ImGui::SetColumnWidth(0, 120);
    ImGui::Text("厅名称:");
    ImGui::NextColumn();
    ImGui::SetColumnWidth(1, 350);
    ImGui::InputText("##HallName", gHallName, IM_ARRAYSIZE(gHallName));
    ImGui::Columns(1);
    
    ImGui::Spacing();
    
    // Row 2: Yimai
    ImGui::Columns(2, "hall_info_row2", false);
    ImGui::SetColumnWidth(0, 120);
    ImGui::Text("一麦:");
    ImGui::NextColumn();
    ImGui::SetColumnWidth(1, 350);
    ImGui::InputText("##Yimai", gYimai, IM_ARRAYSIZE(gYimai));
    ImGui::Columns(1);
    
    ImGui::Spacing();
    
    // Row 3: Time range
    ImGui::Columns(2, "hall_info_row3", false);
    ImGui::SetColumnWidth(0, 120);
    ImGui::Text("档时:");
    ImGui::NextColumn();
    ImGui::SetColumnWidth(1, 400);
    
    // Time inputs with buttons
    ImGui::PushItemWidth(100);
    ImGui::InputText("##TimeStart", gTimeStart, IM_ARRAYSIZE(gTimeStart));
    ImGui::SameLine();
    if (ImGui::Button("选择##TimeStartBtn", ImVec2(60, 0))) {
        gShowTimePickerStart = true;
        sscanf_s(gTimeStart, "%d:%d", &gSelectedHour, &gSelectedMinute);
        LOG_TEST("Time start picker button clicked");
    }
    ImGui::SameLine();
    ImGui::Text("-");
    ImGui::SameLine();
    ImGui::InputText("##TimeEnd", gTimeEnd, IM_ARRAYSIZE(gTimeEnd));
    ImGui::SameLine();
    if (ImGui::Button("选择##TimeEndBtn", ImVec2(60, 0))) {
        gShowTimePickerEnd = true;
        sscanf_s(gTimeEnd, "%d:%d", &gSelectedHour, &gSelectedMinute);
        LOG_TEST("Time end picker button clicked");
    }
    ImGui::PopItemWidth();
    
    ImGui::Columns(1);
}

void RenderCountersSection() {
    ImGui::SeparatorText("计数器");
    
    // 更新汇总计数器
    UpdateSummaryCounters();
    
    const char* counterNames[] = {"喊活", "新付", "拉新", "建联", "二消", "三关"};
    
    for (int i = 0; i < 6; i++) {
        ImGui::PushID(i);
        
        ImGui::Text("%s", counterNames[i]);
        ImGui::SameLine();
        
        ImGui::PushItemWidth(70);
        ImGui::InputInt(("##curr" + std::to_string(i)).c_str(), &gCounters[i].currentValue, 0, 0);
        if (gCounters[i].currentValue < 0) gCounters[i].currentValue = 0;
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        ImGui::Text("/");
        ImGui::SameLine();
        
        ImGui::PushItemWidth(70);
        ImGui::InputInt(("##tgt" + std::to_string(i)).c_str(), &gCounters[i].targetValue, 0, 0);
        if (gCounters[i].targetValue < 0) gCounters[i].targetValue = 0;
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        // 只有喊活(0)和新付(1)有+按钮
        if (i == 0 || i == 1) {
            if (ImGui::Button(("+##" + std::to_string(i)).c_str(), ImVec2(40, 0))) {
                gCounters[i].currentValue++;
                LOG_TEST("Counter %s +1 clicked", counterNames[i]);
            }
            ImGui::SameLine();
        }
        
        if (ImGui::Button(("设定##" + std::to_string(i)).c_str(), ImVec2(70, 0))) {
            gShowTargetDialog = true;
            gTargetCounterIndex = i;
            gTargetInputValue = gCounters[i].targetValue;
        }
        
        ImGui::PopID();
        ImGui::NewLine();
    }
}

void RenderHostsSection() {
    ImGui::SeparatorText("主持数据");
    
    // Host rows
    for (size_t i = 0; i < gHosts.size(); i++) {
        ImGui::PushID((int)i);
        
        ImGui::Columns(6, ("host_" + std::to_string(i)).c_str(), false);
        
        // Column 0: Name
        ImGui::SetColumnWidth(0, 220);
        char label[32];
        sprintf_s(label, "%zu麦:", i + 1);
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::PushItemWidth(120);
        ImGui::InputText("##Name", gHosts[i].name, IM_ARRAYSIZE(gHosts[i].name));
        ImGui::PopItemWidth();
        ImGui::NextColumn();
        
        // Column 1: Laxin
        ImGui::SetColumnWidth(1, 160);
        ImGui::Text("拉新:");
        ImGui::SameLine();
        ImGui::PushItemWidth(60);
        ImGui::InputInt(("##Laxin" + std::to_string(i)).c_str(), &gHosts[i].laxin, 0, 0);
        if (gHosts[i].laxin < 0) gHosts[i].laxin = 0;
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button(("+##Laxin" + std::to_string(i)).c_str(), ImVec2(40, 0))) {
            gHosts[i].laxin++;
            UpdateSummaryCounters();
            LOG_TEST("Host #%zu Laxin +1 clicked", i + 1);
        }
        ImGui::NextColumn();
        
        // Column 2: Jianlian
        ImGui::SetColumnWidth(2, 160);
        ImGui::Text("建联:");
        ImGui::SameLine();
        ImGui::PushItemWidth(60);
        ImGui::InputInt(("##Jianlian" + std::to_string(i)).c_str(), &gHosts[i].jianlian, 0, 0);
        if (gHosts[i].jianlian < 0) gHosts[i].jianlian = 0;
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button(("+##Jianlian" + std::to_string(i)).c_str(), ImVec2(40, 0))) {
            gHosts[i].jianlian++;
            UpdateSummaryCounters();
            LOG_TEST("Host #%zu Jianlian +1 clicked", i + 1);
        }
        ImGui::NextColumn();
        
        // Column 3: Erxiao
        ImGui::SetColumnWidth(3, 160);
        ImGui::Text("二消:");
        ImGui::SameLine();
        ImGui::PushItemWidth(60);
        ImGui::InputInt(("##Erxiao" + std::to_string(i)).c_str(), &gHosts[i].erxiao, 0, 0);
        if (gHosts[i].erxiao < 0) gHosts[i].erxiao = 0;
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button(("+##Erxiao" + std::to_string(i)).c_str(), ImVec2(40, 0))) {
            gHosts[i].erxiao++;
            UpdateSummaryCounters();
            LOG_TEST("Host #%zu Erxiao +1 clicked", i + 1);
        }
        ImGui::NextColumn();
        
        // Column 4: Sanguan
        ImGui::SetColumnWidth(4, 160);
        ImGui::Text("三关:");
        ImGui::SameLine();
        ImGui::PushItemWidth(60);
        ImGui::InputInt(("##Sanguan" + std::to_string(i)).c_str(), &gHosts[i].sanguan, 0, 0);
        if (gHosts[i].sanguan < 0) gHosts[i].sanguan = 0;
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button(("+##Sanguan" + std::to_string(i)).c_str(), ImVec2(40, 0))) {
            gHosts[i].sanguan++;
            UpdateSummaryCounters();
            LOG_TEST("Host #%zu Sanguan +1 clicked", i + 1);
        }
        ImGui::NextColumn();
        
        // Column 5: Delete button (only for hosts > 3)
        ImGui::SetColumnWidth(5, 100);
        if (i >= BASE_HOST_COUNT) {
            if (ImGui::Button("删除", ImVec2(80, 0))) {
                ImGui::OpenPopup("ConfirmDelete");
            }
            
            if (ImGui::BeginPopupModal("ConfirmDelete", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("确定删除第 %zu 个主持?", i + 1);
                ImGui::Text("数据将被清除。");
                if (ImGui::Button("确定", ImVec2(120, 0))) {
                    gHosts.erase(gHosts.begin() + i);
                    UpdateSummaryCounters();
                    LOG_TEST("Remove host slot #%zu, count: %zu -> %zu", i + 1, gHosts.size() + 1, gHosts.size());
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("取消", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::NextColumn();
        
        ImGui::Columns(1);
        ImGui::PopID();
        
        ImGui::Spacing();
    }
    
    // Add host button
    if (gHosts.size() < MAX_HOSTS) {
        if (ImGui::Button("添加主持", ImVec2(150, 0))) {
            gHosts.push_back(HostData());
            LOG_TEST("Add host slot, count: %zu -> %zu", gHosts.size() - 1, gHosts.size());
        }
    }
}

void RenderActionButtons() {
    ImGui::Separator();
    
    // Copy button
    if (ImGui::Button("复制数据", ImVec2(180, 40))) {
        CopyDataToClipboard();
    }
    
    // Last copy time
    if (strlen(gLastCopyTime) > 0) {
        ImGui::SameLine();
        ImGui::Text("%s", gLastCopyTime);
    }
    
    // Beta version notice
    ImGui::Spacing();
    ImGui::TextDisabled("测试版，不代表最终结果");
}

void RenderTimePickerDialog() {
    // Don't render time picker if health reminder is shown
    if (gShowHealthReminder) {
        return;
    }
    
    if (gShowTimePickerStart || gShowTimePickerEnd) {
        ImGui::SetNextWindowSize(ImVec2(300, 220), ImGuiCond_Always);
        
        const char* title = gShowTimePickerStart ? "开始时间" : "结束时间";
        if (ImGui::Begin(title, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            ImGui::Text("小时:");
            ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::SliderInt("##Hour", &gSelectedHour, 0, 23);
            ImGui::PopItemWidth();
            
            ImGui::Text("分钟:");
            ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::SliderInt("##Minute", &gSelectedMinute, 0, 59);
            ImGui::PopItemWidth();
            
            if (ImGui::Button("确定", ImVec2(120, 0))) {
                char* target = gShowTimePickerStart ? gTimeStart : gTimeEnd;
                sprintf_s(target, 8, "%02d:%02d", gSelectedHour, gSelectedMinute);
                
                // Validate: if start > end, set end = start
                if (gShowTimePickerStart) {
                    int startH, startM, endH, endM;
                    sscanf_s(gTimeStart, "%d:%d", &startH, &startM);
                    sscanf_s(gTimeEnd, "%d:%d", &endH, &endM);
                    if (startH * 60 + startM > endH * 60 + endM) {
                        sprintf_s(gTimeEnd, 8, "%02d:%02d", startH, startM);
                    }
                }
                
                // Check if duration exceeds 6 hours (health reminder)
                int startH, startM, endH, endM;
                sscanf_s(gTimeStart, "%d:%d", &startH, &startM);
                sscanf_s(gTimeEnd, "%d:%d", &endH, &endM);
                int durationMinutes = (endH * 60 + endM) - (startH * 60 + startM);
                if (durationMinutes < 0) {
                    durationMinutes += 24 * 60; // Handle overnight
                }
                
                if (durationMinutes > 6 * 60) {
                    gShowHealthReminder = true;
                } else {
                    gShowTimePickerStart = false;
                    gShowTimePickerEnd = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("取消", ImVec2(120, 0))) {
                gShowTimePickerStart = false;
                gShowTimePickerEnd = false;
            }
        }
        ImGui::End();
    }
}

void RenderHealthReminderDialog() {
    if (gShowHealthReminder) {
        ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_Always);
        if (ImGui::Begin("健康提示", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            ImGui::Text("连续档时较长，你确定吗？");
            
            if (ImGui::Button("确定", ImVec2(120, 0))) {
                gShowHealthReminder = false;
                gShowTimePickerStart = false;
                gShowTimePickerEnd = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("取消", ImVec2(120, 0))) {
                gShowHealthReminder = false;
                // Keep time picker open for user to adjust
            }
        }
        ImGui::End();
    }
}

void RenderTargetDialog() {
    if (gShowTargetDialog && gTargetCounterIndex >= 0) {
        ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Always);
        
        if (ImGui::Begin("设定目标", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            ImGui::Text("目标值:");
            ImGui::PushItemWidth(150);
            ImGui::InputInt("##TargetValue", &gTargetInputValue, 0, 0);
            ImGui::PopItemWidth();
            
            // 确保目标值不为负数
            if (gTargetInputValue < 0) {
                gTargetInputValue = 0;
            }
            
            if (ImGui::Button("确定", ImVec2(120, 0))) {
                gCounters[gTargetCounterIndex].targetValue = gTargetInputValue;
                gShowTargetDialog = false;
                gTargetCounterIndex = -1;
            }
            ImGui::SameLine();
            if (ImGui::Button("取消", ImVec2(120, 0))) {
                gShowTargetDialog = false;
                gTargetCounterIndex = -1;
            }
        }
        ImGui::End();
    }
}

// ==============================
// Main Entry Point
// ==============================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    LOG_INFO("Application starting...");
    
    // Enable DPI awareness
    ImGui_ImplWin32_EnableDpiAwareness();
    
    // Create window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), 
                       nullptr, nullptr, nullptr, nullptr, L"VoiceRoomReportGenerator", nullptr };
    ::RegisterClassExW(&wc);
    
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"语音厅群内报表生成器 Version:0.9.2", 
                                 WS_OVERLAPPEDWINDOW, 100, 100, 1200, 900, 
                                 nullptr, nullptr, wc.hInstance, nullptr);
    
    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        LOG_ERROR("Failed to create DirectX11 device");
        return 1;
    }
    
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    
    LOG_INFO("Window created successfully");
    
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Load custom font with Chinese support
    // Initialize theme settings
    SetDefaultTheme();
    
    // Load font
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wchar_t* lastBackslash = wcsrchr(exePath, L'\\');
    if (lastBackslash != nullptr) {
        *lastBackslash = L'\0';
    }
    std::wstring fontPathW = std::wstring(exePath) + L"\\data\\font\\Main.ttf";
    
    // Convert wide string to multi-byte string
    int bufferSize = WideCharToMultiByte(CP_ACP, 0, fontPathW.c_str(), -1, NULL, 0, NULL, NULL);
    std::string fontPath(bufferSize, 0);
    WideCharToMultiByte(CP_ACP, 0, fontPathW.c_str(), -1, &fontPath[0], bufferSize, NULL, NULL);
    
    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), gThemeSettings.FontSize, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    if (font == nullptr) {
        LOG_ERROR("Failed to load font from data/font/Main.ttf, falling back to default font");
    } else {
        LOG_INFO("Custom font loaded successfully");
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    
    // Apply default theme
    ApplyTheme();
    
    LOG_INFO("ImGui initialized");
    
    // Main loop
    bool done = false;
    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;
        
        // Handle window resize
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }
        
        // Handle delayed font reload (disabled for now due to reload issues)
        // if (gPendingFontSize > 0.0f) {
        //     ImGuiIO& io = ImGui::GetIO();
        //     
        //     // Clear all fonts
        //     io.Fonts->Clear();
        //     
        //     // Get executable path
        //     wchar_t exePath[MAX_PATH];
        //     GetModuleFileNameW(NULL, exePath, MAX_PATH);
        //     wchar_t* lastBackslash = wcsrchr(exePath, L'\\');
        //     if (lastBackslash != nullptr) {
        //         *lastBackslash = L'\0';
        //     }
        //     std::wstring fontPathW = std::wstring(exePath) + L"\\data\\font\\Main.ttf";
        //     
        //     // Convert wide string to UTF-8
        //     int bufferSize = WideCharToMultiByte(CP_UTF8, 0, fontPathW.c_str(), -1, NULL, 0, NULL, NULL);
        //     std::string fontPath(bufferSize, 0);
        //     WideCharToMultiByte(CP_UTF8, 0, fontPathW.c_str(), -1, &fontPath[0], bufferSize, NULL, NULL);
        //     
        //     // Log for debugging
        //     printf("[DEBUG] Font path: %s\n", fontPath.c_str());
        //     printf("[DEBUG] Font size: %.1f\n", gPendingFontSize);
        //     
        //     // Add font with new size
        //     ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), gPendingFontSize, nullptr, io.Fonts->GetGlyphRangesChineseFull());
        //     
        //     if (font == nullptr) {
        //         printf("[DEBUG] Font load failed, using default font\n");
        //         // Fallback: try with default font
        //         font = io.Fonts->AddFontDefault();
        //     } else {
        //         printf("[DEBUG] Font loaded successfully\n");
        //     }
        //     
        //     // Set as default font
        //     io.FontDefault = font;
        //     printf("[DEBUG] FontDefault set\n");
        //     
        //     // Rebuild font atlas
        //     ImGui_ImplDX11_InvalidateDeviceObjects();
        //     printf("[DEBUG] InvalidateDeviceObjects done\n");
        //     io.Fonts->Build();
        //     printf("[DEBUG] Fonts->Build() done\n");
        //     ImGui_ImplDX11_CreateDeviceObjects();
        //     printf("[DEBUG] CreateDeviceObjects done\n");
        //     
        //     gPendingFontSize = 0.0f;
        //     printf("[DEBUG] Font reload completed\n");
        // }
        
        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        // Main window
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 20, io.DisplaySize.y - 20));
        ImGui::Begin("Voice Room Report Generator", nullptr, 
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        
        // Render settings window
        RenderSettingsWindow(&gShowSettings);
        
        // Render UI sections
        RenderHallInfoSection();
        RenderCountersSection();
        RenderHostsSection();
        RenderActionButtons();
        
        ImGui::End();
        
        // Render dialogs
        RenderTimePickerDialog();
        RenderHealthReminderDialog();
        RenderTargetDialog();
        
        // Rendering
        ImGui::Render();
        const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        
        g_SwapChainOccluded = false;
        HRESULT hr = g_pSwapChain->Present(1, 0);
        if (hr == DXGI_STATUS_OCCLUDED) {
            g_SwapChainOccluded = true;
        }
    }
    
    // Cleanup
    LOG_INFO("Application exiting");
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    
    return 0;
}

// ==============================
// DirectX11 Helper Functions
// ==============================
bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 
                                                 createDeviceFlags, featureLevelArray, 2, 
                                                 D3D11_SDK_VERSION, &sd, &g_pSwapChain, 
                                                 &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) {
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 
                                             createDeviceFlags, featureLevelArray, 2, 
                                             D3D11_SDK_VERSION, &sd, &g_pSwapChain, 
                                             &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    }
    if (res != S_OK)
        return false;
    
    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    
    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
