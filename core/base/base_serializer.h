#pragma once

struct Serializer {
    static constexpr s32 indent_size = 4;
    std::string out;
    s32 indent_level = 0;
};

template<typename T>
fn serialize(Serializer*, const T&) -> void {
    checkf(false, "Unimplemented serialize specification!");
}

template<typename T>
fn deserialize(std::string_view s, T*) -> void {
    checkf(false, "Unimplemented deserialize specification!");
};

fn serialize_indent(Serializer* s) -> void;
fn serialize_block_init(Serializer* s) -> void;
fn serialize_block_done(Serializer* s) -> void;
fn serialize_field(Serializer* s, std::string_view field) -> void;
fn serialize_new_line(Serializer* s) -> void;

template<typename T>
fn serialize_field(Serializer* s, std::string_view field, const T& value) -> void {
    serialize_field(s, field);
    if constexpr (std::is_arithmetic_v<T>) {
        s->out.append(std::to_string(value));
    } else if constexpr (std::is_same_v<T, std::string>) {
        s->out.append(value);
    } else if constexpr (std::is_same_v<T, const char*>) {
        s->out.append(value);
    } else { // Here we asume that it is a complex type.
        serialize_new_line(s);
        serialize(s, value);
    }
    s->out += "\n";
}

template<>
fn serialize<Vec3>(Serializer* s, const Vec3& v) -> void;