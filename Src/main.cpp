/*
Copyright 2026 YunLuo
Licensed under the Apache License, Version 2.0
http://www.apache.org/licenses/LICENSE-2.0
*/
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <string>
#include <vector>

#pragma comment(lib, "comctl32.lib")

#include "Resource.h"
#include "DataStructures.h"
#include "TargetDialog.h"
#include "TimePickerDialog.h"

HostData gHosts[MAX_HOSTS];
CounterData gCounters[6];
int gHostCount = BASE_HOST_COUNT;
int gBaseY;
HWND gHwndMain;
HWND gBtnAddHost;
HWND gBtnCopy;
HWND gLastCopyTimeLabel;

int GetWindowTextInt(HWND hwnd) {
    wchar_t buf[32];
    GetWindowText(hwnd, buf, 32);
    return _wtoi(buf);
}

HWND CreateLabel(HWND hParent, int x, int y, const wchar_t* text) {
    return CreateWindow(L"static", text, WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
        x, y, 80, 25, hParent, NULL, NULL, NULL);
}

HWND CreateEdit(HWND hParent, int x, int y, int w, int h, int id, const wchar_t* text = L"") {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT;
    return CreateWindow(L"edit", text, style, x, y, w, h, hParent, (HMENU)(UINT_PTR)id, NULL, NULL);
}

HWND CreateNumberEdit(HWND hParent, int x, int y, int w, int h, int id, const wchar_t* text = L"") {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT;
    return CreateWindow(L"edit", text, style, x, y, w, h, hParent, (HMENU)(UINT_PTR)id, NULL, NULL);
}

HWND CreateButton(HWND hParent, int x, int y, int w, int h, int id, const wchar_t* text) {
    return CreateWindow(L"button", text, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x, y, w, h, hParent, (HMENU)(UINT_PTR)id, NULL, NULL);
}

void CreateCounterWithTarget(HWND hwnd, int x, int y, const wchar_t* label, int incBtnId, int setBtnId, int currEditId, int tgtEditId, CounterData* pCounter, bool showIncBtn = true) {
    CreateLabel(hwnd, x, y, label);
    pCounter->hCurrEdit = CreateNumberEdit(hwnd, x + 65, y, 50, 25, currEditId, L"0");
    if (showIncBtn) {
        CreateButton(hwnd, x + 120, y, 30, 25, incBtnId, L"+1");
        pCounter->hIncBtn = NULL;
    } else {
        pCounter->hIncBtn = NULL;
    }
    pCounter->hTgtEdit = CreateNumberEdit(hwnd, x + 155, y, 50, 25, tgtEditId, L"0");
    CreateButton(hwnd, x + 205, y, 60, 25, setBtnId, L"设定目标");
    pCounter->incBtnId = incBtnId;
    pCounter->setBtnId = setBtnId;
    pCounter->currValue = 0;
    pCounter->tgtValue = 0;
}

void UpdateSummaryCounters() {
    int totalLaxin = 0, totalErxiao = 0, totalJianlian = 0, totalSanguan = 0;
    
    for (int i = 0; i < gHostCount; i++) {
        totalLaxin += GetWindowTextInt(gHosts[i].hLaxinEdit);
        totalErxiao += GetWindowTextInt(gHosts[i].hErxiaoEdit);
        totalJianlian += GetWindowTextInt(gHosts[i].hJianlianEdit);
        totalSanguan += GetWindowTextInt(gHosts[i].hSanguanEdit);
    }
    
    SetWindowText(gCounters[2].hCurrEdit, std::to_wstring(totalLaxin).c_str());
    SetWindowText(gCounters[4].hCurrEdit, std::to_wstring(totalErxiao).c_str());
    SetWindowText(gCounters[3].hCurrEdit, std::to_wstring(totalJianlian).c_str());
    SetWindowText(gCounters[5].hCurrEdit, std::to_wstring(totalSanguan).c_str());
}

void CreateHostRow(HWND hwnd, int x, int y, int hostIndex, int totalWidth) {
    const wchar_t* hostLabels[] = { L"一麦主持：", L"二麦主持：", L"三麦主持：", L"四麦主持：", L"五麦主持：", L"六麦主持：", L"七麦主持：", L"八麦主持：" };
    const wchar_t* statLabels[] = { L"拉新", L"二消", L"建联", L"三关" };

    HostData host;
    host.hostIndex = hostIndex;

    int col1 = x;
    host.hLabel = CreateLabel(hwnd, col1, y, hostLabels[hostIndex]);

    int nameX = col1 + 85;
    int nameEditW = 70;

    int nameEditId = ID_HOST_EDIT_NAME_BASE + hostIndex;
    host.hNameEdit = CreateEdit(hwnd, nameX, y, nameEditW, 25, nameEditId);

    int statX = nameX + nameEditW + 5;

    int laxinEditId = ID_HOST_EDIT_LAXIN_BASE + hostIndex;
    int erxiaoEditId = ID_HOST_EDIT_ERXIAO_BASE + hostIndex;
    int jianlianEditId = ID_HOST_EDIT_JIANLIAN_BASE + hostIndex;
    int sanguanEditId = ID_HOST_EDIT_SANGUAN_BASE + hostIndex;

    int laxinIncBtnId = ID_HOST_BTN_LAXIN_INC_BASE + hostIndex;
    int erxiaoIncBtnId = ID_HOST_BTN_ERXIAO_INC_BASE + hostIndex;
    int jianlianIncBtnId = ID_HOST_BTN_JIANLIAN_INC_BASE + hostIndex;
    int sanguanIncBtnId = ID_HOST_BTN_SANGUAN_INC_BASE + hostIndex;

    host.hLaxinLabel = CreateWindow(L"static", statLabels[0], WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
        statX, y, 30, 25, hwnd, NULL, NULL, NULL);
    host.hLaxinEdit = CreateNumberEdit(hwnd, statX + 35, y, 35, 25, laxinEditId, L"0");
    host.hLaxinBtn = CreateButton(hwnd, statX + 75, y, 25, 25, laxinIncBtnId, L"+1");
    statX += 110;

    host.hErxiaoLabel = CreateWindow(L"static", statLabels[1], WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
        statX, y, 30, 25, hwnd, NULL, NULL, NULL);
    host.hErxiaoEdit = CreateNumberEdit(hwnd, statX + 35, y, 35, 25, erxiaoEditId, L"0");
    host.hErxiaoBtn = CreateButton(hwnd, statX + 75, y, 25, 25, erxiaoIncBtnId, L"+1");
    statX += 110;

    host.hJianlianLabel = CreateWindow(L"static", statLabels[2], WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
        statX, y, 30, 25, hwnd, NULL, NULL, NULL);
    host.hJianlianEdit = CreateNumberEdit(hwnd, statX + 35, y, 35, 25, jianlianEditId, L"0");
    host.hJianlianBtn = CreateButton(hwnd, statX + 75, y, 25, 25, jianlianIncBtnId, L"+1");
    statX += 110;

    host.hSanguanLabel = CreateWindow(L"static", statLabels[3], WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
        statX, y, 30, 25, hwnd, NULL, NULL, NULL);
    host.hSanguanEdit = CreateNumberEdit(hwnd, statX + 35, y, 35, 25, sanguanEditId, L"0");
    host.hSanguanBtn = CreateButton(hwnd, statX + 75, y, 25, 25, sanguanIncBtnId, L"+1");
    statX += 110;

    if (hostIndex >= 3) {
        int delBtnId = ID_BTN_DELHOST_BASE + hostIndex;
        host.hDelBtn = CreateButton(hwnd, statX + 10, y, 50, 25, delBtnId, L"删除");
    } else {
        host.hDelBtn = NULL;
    }

    gHosts[hostIndex] = host;
}

void UpdateHostRows(HWND hwnd) {
    for (int i = 0; i < MAX_HOSTS; i++) {
        bool show = (i < gHostCount);
        bool showDel = (i >= 3 && i < gHostCount);

        if (i < MAX_HOSTS) {
            int rowY = gBaseY + i * 30;
            
            // 计算每个控件的X位置
            int x = 5;
            int nameX = x + 85;
            int statX = nameX + 70 + 5;

            ShowWindow(gHosts[i].hLabel, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hNameEdit, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hLaxinLabel, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hLaxinEdit, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hLaxinBtn, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hErxiaoLabel, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hErxiaoEdit, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hErxiaoBtn, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hJianlianLabel, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hJianlianEdit, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hJianlianBtn, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hSanguanLabel, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hSanguanEdit, show ? SW_SHOW : SW_HIDE);
            ShowWindow(gHosts[i].hSanguanBtn, show ? SW_SHOW : SW_HIDE);
            
            if (gHosts[i].hDelBtn) {
                ShowWindow(gHosts[i].hDelBtn, showDel ? SW_SHOW : SW_HIDE);
            }

            if (show) {
                // 更新所有控件的位置
                SetWindowPos(gHosts[i].hLabel, NULL, x, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(gHosts[i].hNameEdit, NULL, nameX, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                
                int currStatX = statX;
                SetWindowPos(gHosts[i].hLaxinLabel, NULL, currStatX, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(gHosts[i].hLaxinEdit, NULL, currStatX + 35, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(gHosts[i].hLaxinBtn, NULL, currStatX + 75, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                currStatX += 110;
                
                SetWindowPos(gHosts[i].hErxiaoLabel, NULL, currStatX, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(gHosts[i].hErxiaoEdit, NULL, currStatX + 35, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(gHosts[i].hErxiaoBtn, NULL, currStatX + 75, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                currStatX += 110;
                
                SetWindowPos(gHosts[i].hJianlianLabel, NULL, currStatX, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(gHosts[i].hJianlianEdit, NULL, currStatX + 35, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(gHosts[i].hJianlianBtn, NULL, currStatX + 75, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                currStatX += 110;
                
                SetWindowPos(gHosts[i].hSanguanLabel, NULL, currStatX, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(gHosts[i].hSanguanEdit, NULL, currStatX + 35, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(gHosts[i].hSanguanBtn, NULL, currStatX + 75, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                currStatX += 110;
                
                if (gHosts[i].hDelBtn) {
                    SetWindowPos(gHosts[i].hDelBtn, NULL, currStatX + 10, rowY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                }
            }
        }
    }

    if (gBtnAddHost && gBtnCopy) {
        int buttonY = gBaseY + gHostCount * 30;
        SetWindowPos(gBtnAddHost, NULL, 20, buttonY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        SetWindowPos(gBtnCopy, NULL, 130, buttonY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        if (gLastCopyTimeLabel) {
            SetWindowPos(gLastCopyTimeLabel, NULL, 260, buttonY + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }
}

void ShowDeleteConfirmDialog(HWND hwnd, int hostIndex) {
    wchar_t msg[256];
    swprintf_s(msg, L"是否确认删除%d号主持位？\r\n（已统计的该主持数据将被清空）", hostIndex + 1);

    if (MessageBox(hwnd, msg, L"确认删除", MB_OKCANCEL | MB_ICONWARNING) == IDOK) {
            // [TEST] Remove host slot (uncomment for debug build, comment for release)
            // printf("[TEST] Remove host slot #%d, count: %d -> %d\n", hostIndex + 1, gHostCount, gHostCount - 1);
        for (int i = hostIndex; i < gHostCount - 1; i++) {
            wchar_t buf1[64], buf2[64], buf3[64], buf4[64];

            GetWindowText(gHosts[i + 1].hNameEdit, buf1, 64);
            GetWindowText(gHosts[i + 1].hLaxinEdit, buf2, 64);
            GetWindowText(gHosts[i + 1].hErxiaoEdit, buf3, 64);
            GetWindowText(gHosts[i + 1].hJianlianEdit, buf4, 64);

            SetWindowText(gHosts[i].hNameEdit, buf1);
            SetWindowText(gHosts[i].hLaxinEdit, buf2);
            SetWindowText(gHosts[i].hErxiaoEdit, buf3);
            SetWindowText(gHosts[i].hJianlianEdit, buf4);

            GetWindowText(gHosts[i + 1].hSanguanEdit, buf1, 64);
            SetWindowText(gHosts[i].hSanguanEdit, buf1);
        }

        int lastIdx = gHostCount - 1;
        SetWindowText(gHosts[lastIdx].hNameEdit, L"");
        SetWindowText(gHosts[lastIdx].hLaxinEdit, L"0");
        SetWindowText(gHosts[lastIdx].hErxiaoEdit, L"0");
        SetWindowText(gHosts[lastIdx].hJianlianEdit, L"0");
        SetWindowText(gHosts[lastIdx].hSanguanEdit, L"0");

        gHostCount--;
        UpdateHostRows(hwnd);
        UpdateSummaryCounters();
        RECT rc;
        GetWindowRect(hwnd, &rc);
        SetWindowPos(hwnd, NULL, rc.left, rc.top, rc.right - rc.left, 425 + gHostCount * 30 + 30, SWP_NOZORDER);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
    }
}

void CopyDataToClipboard(HWND hwnd) {
    std::wstring output;

    wchar_t hallName[128], host1Name[128], timeStart[32], timeEnd[32];
    GetWindowText(GetDlgItem(hwnd, ID_EDIT_HALLNAME), hallName, 128);
    GetWindowText(GetDlgItem(hwnd, ID_EDIT_HOST1NAME), host1Name, 128);
    GetWindowText(GetDlgItem(hwnd, ID_EDIT_TIMESTART), timeStart, 32);
    GetWindowText(GetDlgItem(hwnd, ID_EDIT_TIMEEND), timeEnd, 32);

    if (wcslen(hallName) == 0) wcscpy_s(hallName, L"Null");
    if (wcslen(host1Name) == 0) wcscpy_s(host1Name, L"Null");
    if (wcslen(timeStart) == 0) wcscpy_s(timeStart, L"Null");
    if (wcslen(timeEnd) == 0) wcscpy_s(timeEnd, L"Null");

    output += L"厅名：" + std::wstring(hallName) + L"\r\n";
    output += L"一麦：" + std::wstring(host1Name) + L"\r\n";
    output += L"档时：" + std::wstring(timeStart) + L"-" + std::wstring(timeEnd) + L"\r\n";

    wchar_t buf[32];
    GetWindowText(gCounters[0].hCurrEdit, buf, 32);
    int hanhuo = _wtoi(buf);
    GetWindowText(gCounters[0].hTgtEdit, buf, 32);
    int hanhuoTgt = _wtoi(buf);

    GetWindowText(gCounters[1].hCurrEdit, buf, 32);
    int xinfu = _wtoi(buf);
    GetWindowText(gCounters[1].hTgtEdit, buf, 32);
    int xinfuTgt = _wtoi(buf);

    GetWindowText(gCounters[2].hCurrEdit, buf, 32);
    int laxin = _wtoi(buf);
    GetWindowText(gCounters[2].hTgtEdit, buf, 32);
    int laxinTgt = _wtoi(buf);

    GetWindowText(gCounters[3].hCurrEdit, buf, 32);
    int jianlian = _wtoi(buf);
    GetWindowText(gCounters[3].hTgtEdit, buf, 32);
    int jianlianTgt = _wtoi(buf);

    GetWindowText(gCounters[4].hCurrEdit, buf, 32);
    int erxiao = _wtoi(buf);
    GetWindowText(gCounters[4].hTgtEdit, buf, 32);
    int erxiaoTgt = _wtoi(buf);

    GetWindowText(gCounters[5].hCurrEdit, buf, 32);
    int sanguan = _wtoi(buf);
    GetWindowText(gCounters[5].hTgtEdit, buf, 32);
    int sanguanTgt = _wtoi(buf);

    output += L"喊活：" + std::to_wstring(hanhuo) + L"/" + std::to_wstring(hanhuoTgt) + L"\r\n";
    output += L"新付：" + std::to_wstring(xinfu) + L"/" + std::to_wstring(xinfuTgt) + L"\r\n";
    output += L"拉新：" + std::to_wstring(laxin) + L"/" + std::to_wstring(laxinTgt) + L"\r\n";
    output += L"建联：" + std::to_wstring(jianlian) + L"/" + std::to_wstring(jianlianTgt) + L"\r\n";
    output += L"二消：" + std::to_wstring(erxiao) + L"/" + std::to_wstring(erxiaoTgt) + L"\r\n";
    output += L"三关：" + std::to_wstring(sanguan) + L"/" + std::to_wstring(sanguanTgt) + L"\r\n";

    for (int i = 0; i < gHostCount; i++) {
        wchar_t name[64];
        GetWindowText(gHosts[i].hNameEdit, name, 64);
        if (wcslen(name) == 0) wcscpy_s(name, L"Null");

        int lx = GetWindowTextInt(gHosts[i].hLaxinEdit);
        int ex = GetWindowTextInt(gHosts[i].hErxiaoEdit);
        int jl = GetWindowTextInt(gHosts[i].hJianlianEdit);
        int sg = GetWindowTextInt(gHosts[i].hSanguanEdit);

        output += std::wstring(name) + L"：拉新:" + std::to_wstring(lx) + L" 二消:" + std::to_wstring(ex) +
            L" 建联:" + std::to_wstring(jl) + L" 三关:" + std::to_wstring(sg) + L"\r\n";
    }

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (output.size() + 1) * sizeof(wchar_t));
    if (hMem) {
        wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
        wcscpy_s(pMem, output.size() + 1, output.c_str());
        GlobalUnlock(hMem);

        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            SetClipboardData(CF_UNICODETEXT, hMem);
            CloseClipboard();

            SYSTEMTIME st;
            GetLocalTime(&st);
            wchar_t timeStr[128];
            swprintf_s(timeStr, L"已于%04d年%02d月%02d日%02d时%02d分%02d秒复制数据",
                st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
            SetWindowText(gLastCopyTimeLabel, timeStr);
            
            // [TEST] Copy data (uncomment for debug build, comment for release)
            // printf("[TEST] Data copied to clipboard\n");
        } else {
            GlobalFree(hMem);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HFONT hFont;

    switch (uMsg) {
    case WM_CREATE: {
        hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"微软雅黑");
        if (!hFont) {
            hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_SWISS, L"SimHei");
        }
        if (!hFont) {
            hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        }

        int y = 20;
        // 厅名称
        CreateLabel(hwnd, 5, y, L"厅名：");
        CreateEdit(hwnd, 70, y, 120, 25, ID_EDIT_HALLNAME);

        y += 35;
        // 一麦主持
        CreateLabel(hwnd, 5, y, L"一麦：");
        CreateEdit(hwnd, 70, y, 120, 25, ID_EDIT_HOST1NAME);

        y += 35;
        // 档时
        CreateLabel(hwnd, 5, y, L"档时：");
        CreateEdit(hwnd, 70, y, 60, 25, ID_EDIT_TIMESTART);
        CreateButton(hwnd, 135, y, 50, 25, ID_BTN_TIME_START, L"选择");
        CreateLabel(hwnd, 190, y, L"-");
        CreateEdit(hwnd, 210, y, 60, 25, ID_EDIT_TIMEEND);
        CreateButton(hwnd, 275, y, 50, 25, ID_BTN_TIME_END, L"选择");

        y += 35;
        // 喊活
        CreateCounterWithTarget(hwnd, 5, y, L"喊活：", ID_BTN_HANHUO_INC, ID_BTN_HANHUO_SET, ID_EDIT_HANHUO_CURR, ID_EDIT_HANHUO_TGT, &gCounters[0]);
        y += 35;
        // 新付
        CreateCounterWithTarget(hwnd, 5, y, L"新付：", ID_BTN_XINFU_INC, ID_BTN_XINFU_SET, ID_EDIT_XINFU_CURR, ID_EDIT_XINFU_TGT, &gCounters[1]);
        y += 35;
        // 拉新（自动汇总，无+1按钮）
        CreateCounterWithTarget(hwnd, 5, y, L"拉新：", ID_BTN_LAXIN_INC, ID_BTN_LAXIN_SET, ID_EDIT_LAXIN_CURR, ID_EDIT_LAXIN_TGT, &gCounters[2], false);
        y += 35;
        // 建联（自动汇总，无+1按钮）
        CreateCounterWithTarget(hwnd, 5, y, L"建联：", ID_BTN_JIANLIAN_INC, ID_BTN_JIANLIAN_SET, ID_EDIT_JIANLIAN_CURR, ID_EDIT_JIANLIAN_TGT, &gCounters[3], false);
        y += 35;
        // 二消（自动汇总，无+1按钮）
        CreateCounterWithTarget(hwnd, 5, y, L"二消：", ID_BTN_ERXIAO_INC, ID_BTN_ERXIAO_SET, ID_EDIT_ERXIAO_CURR, ID_EDIT_ERXIAO_TGT, &gCounters[4], false);
        y += 35;
        // 三关（自动汇总，无+1按钮）
        CreateCounterWithTarget(hwnd, 5, y, L"三关：", ID_BTN_SANGUAN_INC, ID_BTN_SANGUAN_SET, ID_EDIT_SANGUAN_CURR, ID_EDIT_SANGUAN_TGT, &gCounters[5], false);
        y += 45;
        gBaseY = y;

        for (int i = 0; i < MAX_HOSTS; i++) {
            CreateHostRow(hwnd, 5, y, i, 760);
            y += 30;
        }

        gBtnAddHost = CreateButton(hwnd, 5, y, 100, 25, ID_BTN_ADDHOST, L"添加主持");
        gBtnCopy = CreateButton(hwnd, 115, y, 100, 25, ID_BTN_COPY, L"复制数据");
        gLastCopyTimeLabel = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | SS_NOPREFIX,
            225, y + 5, 300, 20, hwnd, NULL, NULL, NULL);

        UpdateHostRows(hwnd);

        SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
        break;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);

        for (int i = 0; i < 6; i++) {
            if (id == gCounters[i].incBtnId) {
                // [TEST] Log counter increment (uncomment for debug build)
                // printf("[TEST] Counter +1 clicked, index: %d\n", i);
                int value = GetWindowTextInt(gCounters[i].hCurrEdit);
                SetWindowText(gCounters[i].hCurrEdit, std::to_wstring(value + 1).c_str());
                return 0;
            }
            if (id == gCounters[i].setBtnId) {
                // [TEST] Log target setting (uncomment for debug build)
                // printf("[TEST] Target setting clicked, index: %d\n", i);
                ShowTargetDialog(hwnd, &gCounters[i]);
                return 0;
            }
        }

        if (id == ID_BTN_ADDHOST && gHostCount < MAX_HOSTS) {
            // [TEST] Add host slot (uncomment for debug build, comment for release)
            // printf("[TEST] Add host slot, count: %d -> %d\n", gHostCount, gHostCount + 1);
            gHostCount++;
            UpdateHostRows(hwnd);
            RECT rc;
            GetWindowRect(hwnd, &rc);
            SetWindowPos(hwnd, NULL, rc.left, rc.top, rc.right - rc.left, 425 + gHostCount * 30 + 30, SWP_NOZORDER);
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            return 0;
        }

        if (id >= ID_BTN_DELHOST_BASE && id < ID_BTN_DELHOST_BASE + MAX_HOSTS) {
            int hostIndex = id - ID_BTN_DELHOST_BASE;
            // [TEST] Log delete host button click (uncomment for debug build)
            // printf("[TEST] Delete host button clicked, index: %d\n", hostIndex);
            if (hostIndex >= 3 && hostIndex < gHostCount) {
                ShowDeleteConfirmDialog(hwnd, hostIndex);
            }
            return 0;
        }

        for (int i = 0; i < MAX_HOSTS; i++) {
            if (id == ID_HOST_BTN_LAXIN_INC_BASE + i) {
                // [TEST] Log host laxin +1 (uncomment for debug build)
                // printf("[TEST] Host #%d Laxin +1 clicked\n", i + 1);
                int value = GetWindowTextInt(gHosts[i].hLaxinEdit);
                SetWindowText(gHosts[i].hLaxinEdit, std::to_wstring(value + 1).c_str());
                UpdateSummaryCounters();
                return 0;
            }
            if (id == ID_HOST_BTN_ERXIAO_INC_BASE + i) {
                // [TEST] Log host erxiao +1 (uncomment for debug build)
                // printf("[TEST] Host #%d Erxiao +1 clicked\n", i + 1);
                int value = GetWindowTextInt(gHosts[i].hErxiaoEdit);
                SetWindowText(gHosts[i].hErxiaoEdit, std::to_wstring(value + 1).c_str());
                UpdateSummaryCounters();
                return 0;
            }
            if (id == ID_HOST_BTN_JIANLIAN_INC_BASE + i) {
                // [TEST] Log host jianlian +1 (uncomment for debug build)
                // printf("[TEST] Host #%d Jianlian +1 clicked\n", i + 1);
                int value = GetWindowTextInt(gHosts[i].hJianlianEdit);
                SetWindowText(gHosts[i].hJianlianEdit, std::to_wstring(value + 1).c_str());
                UpdateSummaryCounters();
                return 0;
            }
            if (id == ID_HOST_BTN_SANGUAN_INC_BASE + i) {
                // [TEST] Log host sanguan +1 (uncomment for debug build)
                // printf("[TEST] Host #%d Sanguan +1 clicked\n", i + 1);
                int value = GetWindowTextInt(gHosts[i].hSanguanEdit);
                SetWindowText(gHosts[i].hSanguanEdit, std::to_wstring(value + 1).c_str());
                UpdateSummaryCounters();
                return 0;
            }
        }

        if (id == ID_BTN_COPY) {
            // [TEST] Log copy button click (uncomment for debug build)
            // printf("[TEST] Copy data button clicked\n");
            CopyDataToClipboard(hwnd);
            return 0;
        }

        if (id == ID_BTN_TIME_START) {
            // [TEST] Log time start button click (uncomment for debug build)
            // printf("[TEST] Time start picker button clicked\n");
            ShowTimePickerDialog(hwnd, ID_EDIT_TIMESTART);
            return 0;
        }

        if (id == ID_BTN_TIME_END) {
            // [TEST] Log time end button click (uncomment for debug build)
            // printf("[TEST] Time end picker button clicked\n");
            ShowTimePickerDialog(hwnd, ID_EDIT_TIMEEND);
            return 0;
        }

        if (HIWORD(wParam) == EN_CHANGE) {
            if ((id >= ID_HOST_EDIT_LAXIN_BASE && id < ID_HOST_EDIT_LAXIN_BASE + MAX_HOSTS) ||
                (id >= ID_HOST_EDIT_ERXIAO_BASE && id < ID_HOST_EDIT_ERXIAO_BASE + MAX_HOSTS) ||
                (id >= ID_HOST_EDIT_JIANLIAN_BASE && id < ID_HOST_EDIT_JIANLIAN_BASE + MAX_HOSTS) ||
                (id >= ID_HOST_EDIT_SANGUAN_BASE && id < ID_HOST_EDIT_SANGUAN_BASE + MAX_HOSTS)) {
                // [TEST] Log host data change (uncomment for debug build)
                // printf("[TEST] Host data edited, updating summary counters\n");
                UpdateSummaryCounters();
                return 0;
            }
        }

        break;
    }
    case WM_GETMINMAXINFO: {
        MINMAXINFO* pmmi = (MINMAXINFO*)lParam;
        pmmi->ptMinTrackSize.x = 750;
        pmmi->ptMinTrackSize.y = 425 + gHostCount * 30 + 30;
        break;
    }
    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    // ===== 测试版调试输出（正式版请注释掉以下printf行）=====
    // 测试版：记录初始化和关键操作日志
    // 正式版：注释掉所有printf行
    
    // [TEST] Initialize logs (uncomment for debug build, comment for release)
    // printf("[INFO] Application starting...\n");
    OutputDebugString(L"WinMain started\n");
    
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    const wchar_t CLASS_NAME[] = L"YunLuoChatLiveToolClass";

    WNDCLASSEXW wcex = {0};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = CLASS_NAME;

    ATOM atom = RegisterClassExW(&wcex);
    if (!atom) {
        // [TEST] Log error (uncomment for debug build, comment for release)
        // printf("[ERROR] Failed to register window class\n");
        MessageBoxW(NULL, L"注册窗口类失败", L"错误", MB_ICONERROR);
        return 1;
    }
    // [TEST] Log success (uncomment for debug build, comment for release)
    // printf("[INFO] Window class registered successfully\n");
    OutputDebugString(L"Window class registered\n");

    int initialHeight = 425 + BASE_HOST_COUNT * 30 + 30;
    HWND hwnd = CreateWindowW(CLASS_NAME, L"语音厅群内报表生成器 Version:0.8.0", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 750, initialHeight, NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        // [TEST] Log error (uncomment for debug build, comment for release)
        // printf("[ERROR] Failed to create window\n");
        MessageBoxW(NULL, L"创建窗口失败", L"错误", MB_ICONERROR);
        return 1;
    }
    // [TEST] Log success (uncomment for debug build, comment for release)
    // printf("[INFO] Window created successfully\n");
    OutputDebugString(L"Window created\n");

    gHwndMain = hwnd;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    // [TEST] Log success (uncomment for debug build, comment for release)
    // printf("[INFO] Window shown\n");
    OutputDebugString(L"Window shown\n");

    MSG msg;
    // [TEST] Log info (uncomment for debug build, comment for release)
    // printf("[INFO] Entering message loop\n");
    OutputDebugString(L"Entering message loop\n");
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // [TEST] Log info (uncomment for debug build, comment for release)
    // printf("[INFO] Application exiting\n");
    OutputDebugString(L"Exiting\n");
    return (int)msg.wParam;
}