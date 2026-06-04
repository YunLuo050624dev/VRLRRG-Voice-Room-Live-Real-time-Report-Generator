/*
Copyright 2026 YunLuo
Licensed under the Apache License, Version 2.0
http://www.apache.org/licenses/LICENSE-2.0
*/
#pragma once

#include <windows.h>
#include <vector>

struct CounterData {
    HWND hCurrEdit;
    HWND hTgtEdit;
    HWND hIncBtn;
    int incBtnId;
    int setBtnId;
    int currValue;
    int tgtValue;
};

struct HostData {
    HWND hLabel;
    HWND hNameEdit;
    HWND hLaxinLabel;
    HWND hLaxinEdit;
    HWND hLaxinBtn;
    HWND hErxiaoLabel;
    HWND hErxiaoEdit;
    HWND hErxiaoBtn;
    HWND hJianlianLabel;
    HWND hJianlianEdit;
    HWND hJianlianBtn;
    HWND hSanguanLabel;
    HWND hSanguanEdit;
    HWND hSanguanBtn;
    HWND hDelBtn;
    int hostIndex;
};

#define MAX_HOSTS 8

extern HostData gHosts[MAX_HOSTS];
extern CounterData gCounters[6];
extern int gHostCount;
extern int gBaseY;
extern HWND gHwndMain;
extern HWND gBtnAddHost;
extern HWND gBtnCopy;
extern HWND gLastCopyTimeLabel;