#pragma once

fn time_reset() -> void;
fn time_step() -> void;
fn time_delta() -> f32;
fn time_fixed_ticks() -> u32;
fn time_fps() -> f32;
fn time_av_fps() -> f32;
fn time_set_scale(f32 scale) -> void;