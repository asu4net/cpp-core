#pragma once

fn gpu_clear_back_buffer() -> void;
fn gpu_scene2d_init() -> void;
fn gpu_scene2d_done() -> void;
fn gpu_draw_quad(const Vec3& pos, const Vec3& rot = F32.Zero, const Vec3& scl = F32.One) -> void;
