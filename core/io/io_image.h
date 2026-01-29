#pragma once

struct IO_Image {
    u8* data = nullptr;
    i32 width = 0;
    i32 height = 0;
    i32 channels = 0;
    bool is_owner = false;
};

fn io_image_white() -> const IO_Image*;
fn io_image_load(std::string_view filename, IO_Image* image) -> bool;
fn io_image_free(IO_Image* image) -> void;
fn io_image_valid(const IO_Image& image) -> bool;