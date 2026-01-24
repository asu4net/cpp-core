#pragma once

#ifdef GAME_WIN

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef GAME_GL
#include "os_win32_gl_context.h"
#endif

#include "os_window.h"

class Win32_Window final : public IWindow
{
public:
    Win32_Window(const Window_Desc& ds = {});
    ~Win32_Window();

    Win32_Window(const Win32_Window&) = delete;
    Win32_Window(Win32_Window&&) = delete;
    
    auto operator=(const Win32_Window&) -> Win32_Window & = delete;
    auto operator=(Win32_Window&&) -> Win32_Window & = delete;
    
    auto handle() const -> void* override;
    auto show() const -> void override;
    auto present(bool vsync) const -> void override;
    
private:
    HWND m_handle = nullptr;
    
    #ifdef GAME_GL
    std::unique_ptr<Win32_GL_Context> m_gl_context = nullptr;
    #else
    struct Win32_No_Context {};
    std::unique_ptr<Win32_No_Context> m_no_context = nullptr; // @Note: To keep the class always of same size.
    #endif
};

#endif // GAME_WIN 