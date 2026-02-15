#pragma once
#include "graphics.h"

fn draw_init() -> void;

struct Draw_Sprite {
    const Texture* tex = nullptr;
    Vec3 pos = F32.Zero;
    Vec3 rot = F32.Zero;
    Vec3 scl = F32.One;
    s32 cell = 0;
};

fn draw_sprite(const Draw_Sprite& sprite = {}) -> void;
fn draw_done() -> void;