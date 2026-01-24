
#include "m_imgui.h"
#include "os_window.h"

#if defined(GAME_WIN) && defined(GAME_GL)
#   include "imgui_win32_gl.h"
#endif

internal bool g_imgui_is_init = false;

fn imgui_init(const IWindow& window) -> void {

#if defined(GAME_WIN) && defined(GAME_GL)
        HGLRC hglrc = wglGetCurrentContext();
        HDC hdc = wglGetCurrentDC();
        HWND hwnd = reinterpret_cast<HWND>(window.handle());
        imgui_init_win32_gl(hwnd, hdc, hglrc);
#endif
    g_imgui_is_init = true;
}

fn imgui_done() -> void {

    if (!g_imgui_is_init) {
        return;
    }

#if defined(GAME_WIN) && defined(GAME_GL)
    imgui_done_win32_gl();
#endif

    g_imgui_is_init = false;
}

fn imgui_frame_init() -> void {

    if (!g_imgui_is_init) {
        return;
    }

#if defined(GAME_WIN) && defined(GAME_GL)
    imgui_frame_init_win32_gl();
#endif
}

fn imgui_frame_done() -> void {

    if (!g_imgui_is_init) {
        return;
    }

#if defined(GAME_WIN) && defined(GAME_GL)
    imgui_frame_done_win32_gl();
#endif
}

fn imgui_is_init() -> bool {
    return g_imgui_is_init;
}

fn imgui_draw_frame(Draw_ImGui_Fn draw) -> void {

    if (!g_imgui_is_init) {
        return;
    }

    imgui_frame_init();
    if (draw) {
        draw();
    }
    imgui_frame_done();
}

fn imgui_force_save_ini() -> void {

    if (!g_imgui_is_init) {
        return;
    }

    ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename); // @Note: Manually save the file.
    ImGui::GetIO().IniFilename = nullptr; // @Note: Prevent ImGui to save the file when it destroys the context.
}
