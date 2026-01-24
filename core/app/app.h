
#pragma once
#include "os_window.h"
#include "os_input.h"
#include "m_time.h"
#include "m_imgui.h"
#include "audio.h"

// @Note: This file is just a C-style easy wrapper for the common stuff. You don't need to use it.

struct App_Desc
{
    std::string working_dir = ".\\assets";
    Window_Desc window;
    Input_Desc input;

#ifdef GAME_DEBUG
    bool init_imgui = true;
#else 
    bool init_imgui = false;
#endif
};

// *** Setup ***

// @Note: Just creates IWindow and IInput instances.
fn app_init(App_Desc ds = {}) -> bool;
fn app_done() -> void;
fn app_running() -> bool;