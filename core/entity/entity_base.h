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
    s32  sprite = 0;

    struct Texture* tex = nullptr; // @Pending: This should be an asset handle.
};

fn serialize(Serializer* s, const Entity_Base& e) -> void;
fn deserialize(Deserializer* d, Entity_Base* e) -> void;

fn serialize_fields_base_entity(Serializer* s, const Entity_Base& e) -> void;
fn deserialize_fields_base_entity(Deserializer* d, Entity_Base* e) -> bool;

#endif

#ifdef ENTITY_IMPL

fn serialize(Serializer* s, const Entity_Base& e) -> void {
    serialize_block_init(s);
    serialize_fields_base_entity(s, e);
    serialize_block_done(s);
}

fn deserialize(Deserializer* d, Entity_Base* e) -> void {
    deserialize_block_init(d);
    while (!deserialize_peek_block_done(d)) {
        if (!deserialize_fields_base_entity(d, e)) {
            deserialize_skip_line(d);
        }
    }
    deserialize_block_done(d);
}

fn serialize_fields_base_entity(Serializer* s, const Entity_Base& e) -> void {
    serialize_field(s, "kind", to_string(e.kind));
    serialize_field(s, "enabled", e.enabled);
    serialize_field(s, "pos", e.pos);
    serialize_field(s, "rot", e.rot);
    serialize_field(s, "scl", e.scl);
    serialize_field(s, "tint", e.tint);
    serialize_field(s, "sprite", e.sprite);
}

fn deserialize_fields_base_entity(Deserializer* d, Entity_Base* e) -> bool {
    std::string_view key = deserialize_read_key(d);
    if (key == "enabled") {
        deserialize_value(d, e->enabled);
        return true;
    }
    // @Note: Enums are a bit anoying to deserialize.
    if (key == "kind") {
        std::string str_kind;
        deserialize_value(d, str_kind);
        from_string(str_kind, &e->kind);
        return true;
    }
    if (key == "pos") {
        deserialize_value(d, e->pos);
        return true;
    }
    if (key == "rot") {
        deserialize_value(d, e->rot);
        return true;
    }
    if (key == "scl") {
        deserialize_value(d, e->scl);
        return true;
    }
    if (key == "tint") {
        deserialize_value(d, e->tint);
        return true;
    }
    if (key == "sprite") {
        deserialize_value(d, e->sprite);
        return true;
    }
    return false;
}

#endif