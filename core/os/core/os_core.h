#pragma once

fn os_read_entire_file(std::string_view filename) -> std::string;
fn os_trim(std::string text) -> std::string;

// @Note: This is relative to the exe path. Ex: "\\..\\..\\assets" would set the wdir two folders up the exe, inside the assets dir.
fn os_set_working_dir(const std::string& path) -> void;
fn os_get_time() -> f64;