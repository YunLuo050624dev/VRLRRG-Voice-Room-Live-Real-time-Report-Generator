cmake_minimum_required(VERSION 3.15)
project(Voice-Room-Live-Real-time-Report-Generator)

add_definitions(-DUNICODE -D_UNICODE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Out)

# ===== 测试版与正式版配置 =====
# 测试版：使用控制台模式（有CMD窗口，用于调试）
# 正式版：使用WIN32模式（无CMD窗口，用于发布）
# ==============================

# 测试版配置（取消注释以下行，注释正式版配置）
# add_executable(${PROJECT_NAME} 
#     Src/main.cpp
#     Src/TargetDialog.cpp
#     Src/TimePickerDialog.cpp
# )

# 正式版配置（取消注释以下行，注释测试版配置）
add_executable(${PROJECT_NAME} WIN32 
    Src/main.cpp
    Src/TargetDialog.cpp
    Src/TimePickerDialog.cpp
)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
target_link_libraries(${PROJECT_NAME} comctl32.lib)
