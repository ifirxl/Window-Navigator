#include <windows.h>
#include <iostream>
#include <shellscalingapi.h>
#define _WIN32_WINNT 0x0603 // Windows 8.1
#define NTDDI_VERSION NTDDI_WINBLUE // ∂‘”¶ Windows 8.1

#pragma comment(lib, "Shcore.lib")


BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    if (GetMonitorInfo(hMonitor, &monitorInfo)) {
        UINT dpiX, dpiY;
        if (GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) == S_OK) {
            std::wcout << L"Monitor: " << monitorInfo.szDevice << std::endl;
            std::wcout << L"Position: (" << monitorInfo.rcMonitor.left << L"," << monitorInfo.rcMonitor.top << L") to ("
                << monitorInfo.rcMonitor.right << L"," << monitorInfo.rcMonitor.bottom << L")" << std::endl;
            std::wcout << L"DPI: " << dpiX << L" x " << dpiY << std::endl;
            std::wcout << L"=====================" << std::endl;
        }
    }
    return TRUE;
}

int main() {
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
    return 0;
}
