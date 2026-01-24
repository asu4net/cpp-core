#include "os_core_win32.h"

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

auto os_set_working_dir_win32(const std::string& path) -> void
{
    if (path.empty())
    return;

    std::wostringstream conv;
    conv << path.c_str();
    std::wstring working_dir(conv.str());
/*
    wchar_t exe_path[MAX_PATH];
    GetModuleFileName(nullptr, exe_path, MAX_PATH);
    std::wstring exe_w_path(exe_path);
    size_t pos = exe_w_path.find_last_of(L"\\/");
    std::wstring exe_dir = exe_w_path.substr(0, pos);
    std::wstring final_path = exe_dir + working_dir;
    SetCurrentDirectory(final_path.c_str());
*/
    SetCurrentDirectory(working_dir.c_str());
}

auto os_get_time_win32() -> f64
{
    internal LARGE_INTEGER frequency;
    internal BOOL initialized = FALSE;

    if (!initialized)
    {
        QueryPerformanceFrequency(&frequency);
        initialized = TRUE;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    // @Note: Ticks divided by ticks-per-second.
    return (f64) counter.QuadPart / (f64) frequency.QuadPart;
}