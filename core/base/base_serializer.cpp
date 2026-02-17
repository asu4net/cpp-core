#include "base_serializer.h"

fn serialize_indent(Serializer* s) -> void {
    s32 indent = s->indent_level * s->indent_size;
    s->out.append(indent, ' ');
}

fn serialize_block_init(Serializer* s) -> void {
    serialize_indent(s);
    s->out += "{\n";
    s->indent_level++;
}

fn serialize_block_done(Serializer* s) -> void {
    s->indent_level--;
    serialize_indent(s);
    s->out += "}\n";
}

fn serialize_field(Serializer* s, std::string_view field) -> void {
    serialize_indent(s);
    s->out += field;
    s->out += ": ";
}

fn serialize_new_line(Serializer* s) -> void {
    s->out += "\n";
}

template<>
fn serialize<Vec3>(Serializer* s, const Vec3& v) -> void {
    serialize_block_init(s);
    serialize_field(s, "x", v.x);
    serialize_field(s, "y", v.y);
    serialize_field(s, "z", v.z);
    serialize_block_done(s);
}