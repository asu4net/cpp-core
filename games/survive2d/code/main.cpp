#include "app.h"
#include "draw.h"
#include "graphics.h"

#include "entity.h"
struct Player {
    Entity base;
};

#define ENTITY_IMPL
#include "entity.h"

fn serialize(Serializer* s, const Player& e) -> void {
    serialize_block_init(s);
    serialize_fields_base_entity(s, e.base);
    serialize_block_done(s);
}

fn deserialize(Deserializer* d, Player* e) -> void {
    deserialize_block_init(d);
    while(!deserialize_peek_block_done(d)) {
        deserialize_fields_base_entity(d, &e->base);
        //std::string_view key = deserialize_read_key(d);   
    }
    deserialize_block_done(d);
}

fn draw_sprite_entity(Entity* e) {
    if (e->visible) {
        draw_sprite(e->tex, e->sprite, e->tint, Mat4::transform(e->pos, e->rot, e->scl));
    }
}

fn main() -> s32 {
    
    App_Desc desc;
    desc.window.title = L"Survive 2D";
    app_init(desc);
    draw_init();
    entity_storage_init();
    
    Texture tex;
    Texture_Def def;
    def.kind = Texture_Kind::Tileset;
    def.tile_size = 192;
    def.filename = "sprites/Units/Blue Units/Monk/Run.png";
    texture_init(&tex, def);

    Entity_Handle hA = entity_create(Entity_Kind_Player);
    Entity_Handle hB = entity_create(Entity_Kind_Player);
    
    Entity* entityA = entity_get(hA);
    Entity* entityB = entity_get(hB);

    entityA->scl = { 3.f, 3.f, 1.f };
    entityA->tex = &tex;
    entityA->sprite = 1;

    entityB->pos = Vec3(F32.Right) * 2.f;
    entityB->scl = { 3.f, 3.f, 1.f };
    entityB->tex = &tex;
    entityB->sprite = 1;

    Serializer s;
    serialize(&s, *entityA);

    Deserializer d;
    d.src = s.out;
    Player e;
    deserialize(&d, &e);

    while(app_running()) {
        
        clear_back_buffer();
        entity_pass(draw_sprite_entity);
        os_swap_buffers();
    }

    texture_done(&tex);
    entity_storage_done();
    draw_done();
    app_done();
}