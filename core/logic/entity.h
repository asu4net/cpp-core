#ifndef ENTITY_H
#define ENTITY_H

#include "entity_kinds.h"

// Kind enum.
enum Entity_Kind : u64 {
    Entity_Kind_None = 0,
    #define declare_enum_entry(_kind) \
        Entity_Kind_##_kind,

        for_entity_kinds(declare_enum_entry)
    
    #undef declare_enum_entry
    
    Entity_Kind_Count
};

inline fn to_string(Entity_Kind kind) -> const char* {
    switch (kind)
    {
        #define stringify(_x) #_x
        #define declare_switch_case(_kind) \
            case Entity_Kind_##_kind: return stringify(Entity_Kind_##_kind);

            for_entity_kinds(declare_switch_case)
        #undef declare_switch_case
        #undef stringify
        
        default: return "";
    }
}

// Forward declarations.
#define forward_declare(_kind) \
    struct _kind;
    
    for_entity_kinds(forward_declare)

#undef forward_declare

// Base entity type.
struct Entity {
    Entity_Kind kind = Entity_Kind_None;
    bool enabled = true;
    Vec3 pos = F32.Zero;
    Vec3 rot = F32.Zero;
    Vec3 scl = F32.One;
    Vec4 tint = Color.White;
    struct Texture* tex = nullptr; // @Pending: This should be an asset handle.
    s32 cell = 0; 
};

fn serialize_entity_init(Serializer* s, const Entity& e) -> void;
fn serialize_entity_done(Serializer* s) -> void;

template<>
fn serialize<Entity>(Serializer* s, const Entity& e) -> void;

// @Pending: _Draw_Imgui callback.

// Entity callbacks. WE WONT NEED THIS ? maybe on deserialization.
using Entity_Fn_Serialize = void(*)(Serializer*, const Entity*);
using Entity_Fn_Deserialize = void(*)(Deserializer*, Entity*);

using Entity_Fn_Update = void (*)(Entity *);

// Entity handle.
struct Entity_Handle {
    Entity_Kind kind = Entity_Kind_None;
    Array_Handle value;
};

// World: Entity manager.
struct World {
    #define declare_storage(_kind) \
        Fixed_Handle_Array<struct _kind> _kind##_Storage;
        
        for_entity_kinds(declare_storage)

    #undef declare_storage

    Entity_Fn_Serialize serialize[Entity_Kind_Count];
    Entity_Fn_Deserialize deserialize[Entity_Kind_Count];
};

fn world_init() -> void;
fn world_done() -> void;

fn entity_create(Entity_Kind kind = (Entity_Kind) 1u) -> Entity_Handle;
fn entity_destroy(Entity_Handle handle) -> void; // @Pending: Save the entities to a cleanup list and wait till the frame ends.
fn entity_get(Entity_Handle handle) -> Entity*;
fn entity_pass(Entity_Fn_Update update) -> void;

#endif

#ifdef GAME_ENTITY_IMPL

fn serialize_entity_init(Serializer* s, const Entity& e) -> void {
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

fn deserialize_entity_init(Deserializer* d, Entity* e) -> void {
    s32 cursor = d->cursor;
    deserialize_block_init(d);
    while (!deserialize_peek_block_done(d)) {
        std::string_view key = deserialize_read_key(d);
        if (key == "enabled") deserialize_value(d, e->enabled);
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

template<>
fn serialize<Entity>(Serializer* s, const Entity& e) -> void {
    serialize_entity_init(s, e);
    serialize_entity_done(s);
}

template<>
fn deserialize<Entity>(Deserializer* d, Entity* e) -> void {
    deserialize_entity_init(d, e);
    deserialize_entity_done(d);
}

static World* world = nullptr;

fn world_init() -> void {

    if (world) {
        world_done();
    }
    
    world = new World();

    #define set_callbacks(_kind) \
        world->serialize[Entity_Kind_##_kind] = [](Serializer* s, const Entity* e) { \
            const _kind* entity = static_cast<const Entity*>(e); \
            serialize<_kind>(s, *entity); \
        }; \
        world->deserialize[Entity_Kind_##_kind] = [](Deserializer* d, Entity* e) { \
            _kind* entity = static_cast<Entity*>(e); \
            deserialize<_kind>(d, entity); \
        };

        for_entity_kinds(set_callbacks)
}

fn world_done() -> void {
    if (!world) {
        return;
    }
    #define free_storage(_kind) \
        reset(&world->_kind##_Storage);
    
        for_entity_kinds(free_storage)
    #undef free_storage

    delete world;
    world = nullptr;
}

fn entity_create(Entity_Kind kind) -> Entity_Handle {
    switch (kind) {
        #define choose_storage(_kind)                      \
            case Entity_Kind_##_kind:                      \
            {                                              \
                auto h = append(&world->_kind##_Storage);  \
                auto* e = get(world->_kind##_Storage, h);  \
                e->kind = Entity_Kind_##_kind;             \
                return { e->kind, h };                     \
            }

            for_entity_kinds(choose_storage)

        #undef choose_storage

        case Entity_Kind_None:
        case Entity_Kind_Count:
        default:
            return {};
    }
}

fn entity_destroy(Entity_Handle handle) -> void {
    switch (handle.kind) {
        #define choose_storage(_kind)                              \
            case Entity_Kind_##_kind:                              \
                    remove(&world->_kind##_Storage, handle.value); \

            for_entity_kinds(choose_storage)
        #undef choose_storage
        case Entity_Kind_None:
        case Entity_Kind_Count:
        default:
            return;
    }
}

fn entity_get(Entity_Handle handle) -> Entity* {
    switch (handle.kind) {
        #define choose_storage(_kind)                                  \
            case Entity_Kind_##_kind:                                  \
                    return get(world->_kind##_Storage, handle.value);  \
            case Entity_Kind_None:                                     \
            case Entity_Kind_Count:                                    \
            default:                                                   \
                return get_default(world->_kind##_Storage);           

        for_entity_kinds(choose_storage)
        #undef choose_storage
    }
}

fn entity_pass(Entity_Fn_Update update) -> void {
    #define do_pass(_kind)                                 \
        for(_kind& entity: world->_kind##_Storage) {       \
            if (entity.enabled) {                          \
                update(&entity);                           \
            }                                              \
        }

    for_entity_kinds(do_pass)
}

#endif