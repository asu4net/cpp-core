#pragma once

#if GAME_GL

fn gl_quad_buffer_init() -> void;
fn gl_quad_buffer_done() -> void;
fn gl_quad_draw(const Vec3& pos, const Vec3& rot = F32.Zero, const Vec3& scl = F32.One) -> void;

#endif
