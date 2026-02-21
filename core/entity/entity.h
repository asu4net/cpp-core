#ifndef ENTITY_STORAGE_H
#define ENTITY_STORAGE_H

#include "entity_base.h"

// Entity handle.
struct Entity_Handle {
    Entity_Kind kind = Entity_Kind_None;
    Array_Handle value;
};

// Entity_Storage: Entity manager.
struct Entity_Storage {
    #define declare_storage(_kind) \
        Fixed_Handle_Array<struct _kind> _kind##_Storage;
        
        for_entity_kinds(declare_storage)

    #undef declare_storage
};

fn entity_storage_init() -> void;
fn entity_storage_done() -> void;
fn entity_create(Entity_Kind kind = (Entity_Kind) 1u) -> Entity_Handle;
fn entity_destroy(Entity_Handle handle) -> void; // @Pending: Save the entities to a cleanup list and wait till the frame ends.
fn entity_get(Entity_Handle handle) -> Entity_Base*;
fn entity_pass(void (*update)(Entity_Base*)) -> void;

#endif

#ifdef ENTITY_IMPL

static Entity_Storage* world = nullptr;

fn entity_storage_init() -> void {

    if (world) {
        entity_storage_done();
    }
    
    world = new Entity_Storage();
}

fn entity_storage_done() -> void {
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

fn entity_pass(void (*update)(Entity_Base*)) -> void {
    #define do_pass(_kind)                                 \
        for(_kind& entity: world->_kind##_Storage) {       \
            if (entity.enabled) {                          \
                update(&entity);                           \
            }                                              \
        }

    for_entity_kinds(do_pass)
}

#endif