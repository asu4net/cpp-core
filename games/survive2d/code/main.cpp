#include "app.h"
#include "draw.h"
#include "graphics.h"

#define GAME_ENTITY_IMPL
#include "entity.h"

static fn draw_sprite_entity(Entity* e) {
    draw_sprite(e->tex, e->cell, e->tint, Mat4::transform(e->pos, e->rot, e->scl));
}

fn main() -> s32 {
    
    App_Desc desc;
    desc.window.title = L"Survive 2D";
    app_init(desc);
    draw_init();
    world_init();
    
    Texture tex;
    Texture_Def def;
    def.kind = Texture_Kind::Tileset;
    def.tile_size = 192;
    def.filename = "sprites/Units/Blue Units/Monk/Run.png";
    texture_init(&tex, def);

    Entity_Handle hA = entity_create();
    Entity_Handle hB = entity_create();
    
    Entity* entityA = entity_get(hA);
    Entity* entityB = entity_get(hB);

    entityA->tex = &tex;
    entityA->scl = { 3.f, 3.f, 1.f };

    entityB->pos = Vec3(F32.Right) * 2.f;
    entityB->tex = &tex;
    entityB->scl = { 3.f, 3.f, 1.f };
    entityB->cell = 1;

    Serializer s;
    serialize(&s, *entityA);

    Deserializer d;
    d.src = s.out;
    Entity e;
    deserialize(&d, &e);

    while(app_running()) {
        
        clear_back_buffer();
        entity_pass(draw_sprite_entity);
        os_swap_buffers();
    }

    texture_done(&tex);
    world_done();
    draw_done();
    app_done();
}