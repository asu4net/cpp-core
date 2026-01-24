#pragma once

using Audio_Handle = Slot_Handle;

fn audio_init() -> bool;
fn audio_done() -> void;
fn audio_load(const std::string& filename) -> Audio_Handle;
fn audio_free(const Audio_Handle& handle) -> void;
fn audio_play(const Audio_Handle& handle) -> void;
fn audio_stop(const Audio_Handle& handle) -> void;
fn audio_set_volume(const Audio_Handle& handle, f32 volume) -> void;