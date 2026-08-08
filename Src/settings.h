#pragma once

#include "imgui.h"

// Theme settings structure
struct ThemeSettings {
    // Window colors
    ImVec4 WindowBg;
    ImVec4 ChildBg;
    ImVec4 PopupBg;
    
    // Text colors
    ImVec4 Text;
    ImVec4 TextDisabled;
    
    // Button colors
    ImVec4 Button;
    ImVec4 ButtonHovered;
    ImVec4 ButtonActive;
    
    // Frame colors
    ImVec4 FrameBg;
    ImVec4 FrameBgHovered;
    ImVec4 FrameBgActive;
    
    // Input colors
    ImVec4 InputText;
    ImVec4 InputBorder;
    
    // Font settings
    float FontSize;
    const char* FontPath;
    
    // Rounding settings
    float WindowRounding;
    float FrameRounding;
};

// Global theme settings extern declaration
extern ThemeSettings gThemeSettings;

// Default theme settings
void SetDefaultTheme();

// Apply theme to ImGui
void ApplyTheme();

// Render settings window (pass showSettings flag from outside)
void RenderSettingsWindow(bool* showSettings);
