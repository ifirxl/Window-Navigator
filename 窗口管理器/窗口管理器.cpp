#include <windows.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
using namespace std;

// 定义 WindowInfo 结构体，用于存储窗口信息
struct WindowInfo {
    HWND hwnd;              // 窗口句柄
    string title;      // 窗口标题
    RECT rect;              // 窗口的矩形区域
    int monitorIndex;       // 窗口所在的显示器编号
};

// 定义 MonitorInfoEx 结构体，用于存储显示器信息
struct MonitorInfoEx {
    HMONITOR hMonitor;      // 显示器句柄
    MONITORINFOEX monitorInfo; // 显示器的详细信息
    int index;              // 显示器编号
};

// 回调函数 EnumWindowsProc，用于枚举所有窗口并获取其信息
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    // 解释 lParam 参数为包含显示器和窗口信息的 pair
    pair<vector<MonitorInfoEx>*, vector<WindowInfo>*>* params =
        reinterpret_cast<pair<vector<MonitorInfoEx>*, vector<WindowInfo>*>*>(lParam);
    vector<MonitorInfoEx>* monitors = params->first;  // 获取显示器信息向量
    vector<WindowInfo>* windows = params->second;     // 获取窗口信息向量

    char windowTitle[256];
    GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle)); // 获取窗口标题

    if (IsWindowVisible(hwnd) && strlen(windowTitle) > 0) { // 检查窗口是否可见且有标题
        RECT rect;
        GetWindowRect(hwnd, &rect);  // 获取窗口的矩形区域

        HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST); // 获取窗口所在的显示器
        int monitorIndex = 0;
        for (size_t i = 0; i < monitors->size(); ++i) {
            if (monitors->at(i).hMonitor == hMonitor) { // 找到窗口所在的显示器编号
                monitorIndex = monitors->at(i).index;
                break;
            }
        }

        // 存储窗口信息
        WindowInfo info = { hwnd, windowTitle, rect, monitorIndex };
        windows->push_back(info);
    }

    return TRUE; // 继续枚举
}

// 回调函数 MonitorEnumProc，用于枚举所有显示器并获取其信息
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &monitorInfo); // 获取显示器的详细信息

    // 存储显示器信息
    MonitorInfoEx info = { hMonitor, monitorInfo, 0 };
    vector<MonitorInfoEx>* monitors = reinterpret_cast<vector<MonitorInfoEx>*>(dwData);
    monitors->push_back(info);

    return TRUE;
}

int main() {
    vector<MonitorInfoEx> monitors;
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors)); // 枚举所有显示器

    // 按显示器的左上角坐标排序，并分配索引
    sort(monitors.begin(), monitors.end(), [](const MonitorInfoEx& a, const MonitorInfoEx& b) {
        return a.monitorInfo.rcMonitor.left < b.monitorInfo.rcMonitor.left;
        });

    for (size_t i = 0; i < monitors.size(); ++i) {
        monitors[i].index = static_cast<int>(i) + 1; // 分配显示器编号
    }

    vector<WindowInfo> windows;
    pair<vector<MonitorInfoEx>*, vector<WindowInfo>*> params = { &monitors, &windows };
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&params)); // 枚举所有窗口

    // 输出所有显示器的坐标信息
    cout << setw(10) << left << "显示器"
        << setw(20) << left << "左上角坐标"
        << setw(20) << left << "右下角坐标" << endl;

    for (const auto& monitor : monitors) {
        string topLeft = "(" + to_string(monitor.monitorInfo.rcMonitor.left) + ", " + to_string(monitor.monitorInfo.rcMonitor.top) + ")";
        string bottomRight = "(" + to_string(monitor.monitorInfo.rcMonitor.right) + ", " + to_string(monitor.monitorInfo.rcMonitor.bottom) + ")";
        cout << setw(10) << left << monitor.index
            << setw(20) << left << topLeft
            << setw(20) << left << bottomRight << endl;
    }
    cout << endl;
    // 输出表头
    cout << setw(7) << left << "序号" << setw(65) << left << "应用"
        << setw(10) << left << "显示器" << setw(20) << left << "定位(左上)"
        << setw(20) << left << "尺寸" << endl;

    // 打印所有窗口信息
    for (size_t i = 0; i < windows.size(); ++i) {
        string Position = "(" + to_string(windows[i].rect.left) + ", " + to_string(windows[i].rect.top) + ")";
        string Size = "(" + to_string(windows[i].rect.right - windows[i].rect.left) + ", " + to_string(windows[i].rect.bottom - windows[i].rect.top) + ")";
        cout << setw(5) << left << i + 1 << ": " << setw(65) << left << windows[i].title;
        cout << setw(10) << left << windows[i].monitorIndex;
        cout << setw(20) << left << Position << setw(20) << left << Size << endl;
    }

    // 让用户选择一个窗口
    int choice;
    cout << "请选择一个窗口编号：";
    cin >> choice;
    --choice; // 转换为0-based索引

    if (choice >= 0 && choice < static_cast<int>(windows.size())) {
        int newX, newY;
        cout << "请输入新的X坐标：";
        cin >> newX;
        cout << "请输入新的Y坐标：";
        cin >> newY;

        // 移动窗口
        MoveWindow(windows[choice].hwnd, newX, newY,
            windows[choice].rect.right - windows[choice].rect.left,
            windows[choice].rect.bottom - windows[choice].rect.top,
            TRUE);

        cout << "窗口已移动到 (" << newX << ", " << newY << ")" << endl;
    }
    else {
        cout << "无效的选择！" << endl;
    }

    return 0;
}
