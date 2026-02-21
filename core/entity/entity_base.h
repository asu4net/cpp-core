#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include "entity_kind.h"

// Base entity type.
struct Entity_Base {
    Entity_Kind kind = Entity_Kind_None;
    bool enabled = true;
    Vec3 pos = F32.Zero;
    Vec3 rot = F32.Zero;
    Vec3 scl = F32.One;
    Vec4 tint = Color.White;
    struct Texture* tex = nullptr; // @Pending: This should be an asset handle.
    s32 cell = 0; 
};

fn serialize_entity_init(Serializer* s, const Entity_Base& e) -> void;
fn serialize_entity_done(Serializer* s) -> void;
fn serialize(Serializer* s, const Entity_Base& e) -> void;

#endif

#ifdef ENTITY_IMPL

fn serialize_entity_init(Serializer* s, const Entity_Base& e) -> void {
    serialize_block_init(s);
    serialize_field(s, "kind", to_string(e.kind));
    serialize_field(s, "enabled", e.enabled);
    serialize_field(s, "pos", e.pos);
    serialize_field(s, "rot", e.rot);
    serialize_field(s, "scl", e.scl);
}

fn serialize_entity_done(Serializer* s) -> void {
    serialize_block_done(s);
}

fn deserialize_entity_init(Deserializer* d, Entity_Base* e) -> void {
    s32 cursor = d->cursor;
    deserialize_block_init(d);
    while (!deserialize_peek_block_done(d)) {
        std::string_view key = deserialize_read_key(d);
        if (key == "enabled") deserialize_value(d, e->enabled);
        // @Note: Enums are a bit anoying to deserialize.
        else if (key == "kind") {
            std::string str_kind;
            deserialize_value(d, str_kind);
            from_string(str_kind, &e->kind);
        }
        else if (key == "pos") deserialize_value(d, e->pos);
        else if (key == "rot") deserialize_value(d, e->rot);
        else if (key == "scl") deserialize_value(d, e->scl);
        else deserialize_skip_line(d); // Unknown field, skip it
    }
    deserialize_block_done(d);
    d->cursor = cursor; // Restore the cursor to make another pass.
}

inline fn deserialize_entity_done(Deserializer*) -> void {
    // Dummy. We keep it for coherence.
}

fn serialize(Serializer* s, const Entity_Base& e) -> void {
    serialize_entity_init(s, e);
    serialize_entity_done(s);
}

fn deserialize(Deserializer* d, Entity_Base* e) -> void {
    deserialize_entity_init(d, e);
    deserialize_entity_done(d);
}

#endif