#pragma once

fn gpu_scene2d_init_gl() -> void;
fn gpu_scene2d_done_gl() -> void;
fn gpu_draw_quad_gl(const Vec3& pos, const Vec3& rot = F32.Zero, const Vec3& scl = F32.One) -> void;
