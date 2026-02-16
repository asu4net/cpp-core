#ifndef ENTITY_H
#define ENTITY_H

#include "entity_game_defs.h"

// Kind enum.
enum Entity_Kind : u64 {
    Entity_Kind_None = 0,
    #define declare_enum_entry(_kind) \
        Entity_Kind_##_kind,

        for_entity_kinds(declare_enum_entry)
    
    #undef declare_enum_entry
    
    Entity_Kind_Count
};

// Forward declarations.
#define forward_declare(_kind) \
    struct _kind;
    
    for_entity_kinds(forward_declare)

#undef forward_declare

// Base entity type.
struct Entity_Base {
    Entity_Kind kind = Entity_Kind_None;
    bool enabled = true;
    Vec3 pos = F32.Zero;
    Vec3 rot = F32.Zero;
    Vec3 scl = F32.One;
    struct Texture* tex = nullptr;
    s32 cell = 0; 
};

// Entity callbacks.
using Entity_Fn_Serialize = void(*)(const Entity_Base*, std::string*);
using Entity_Fn_Deserialize = void(*)(std::string_view, Entity_Base*);
using Entity_Fn_Loop = void (*)(Entity_Base *);

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
fn entity_destroy(Entity_Handle handle) -> void;
fn entity_get(Entity_Handle handle) -> Entity_Base*;
fn entity_loop(Entity_Fn_Loop loop) -> void;

#endif

#ifdef GAME_ENTITY_IMPL

static World* world = nullptr;

fn world_init() -> void {

    if (world) {
        world_done();
    }
    
    world = new World();

    #define set_callbacks(_kind) \
        world->serialize[Entity_Kind_##_kind] = &_Serialize_##_kind; \
        world->deserialize[Entity_Kind_##_kind] = &_Deserialize_##_kind; \

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

fn entity_get(Entity_Handle handle) -> Entity_Base* {
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

fn entity_loop(Entity_Fn_Loop loop) -> void {
    #define do_loop(_kind)                                 \
        for(_kind& entity: world->_kind##_Storage) {       \
            if (entity.enabled) {                          \
                loop(&entity);                             \
            }                                              \
        }

    for_entity_kinds(do_loop)
}

#endif