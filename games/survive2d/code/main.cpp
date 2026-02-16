#include "app.h"
#include "draw.h"
#include "graphics.h"
#include "game_entity.h"

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
    
    Entity* entityA = (Entity*) entity_get(hA);
    Entity* entityB = (Entity*) entity_get(hB);

    entityA->tex = &tex;
    entityA->scl = { 3.f, 3.f, 1.f };
    entityA->enabled = false;

    entityB->pos = Vec3(F32.Right) * 2.f;
    entityB->tex = &tex;
    entityB->scl = { 3.f, 3.f, 1.f };
    entityB->cell = 1;

    while(app_running()) {
        
        clear_back_buffer();
        auto fnDrawEntity = [](Entity_Base* entity_base) {
            if (entity_base->kind != Entity_Kind_Entity) {
                return;
            }
            Entity* entity = (Entity*) entity_base;
            Draw_Sprite sprite;
            sprite.pos = entity->pos;
            sprite.rot = entity->rot;
            sprite.scl = entity->scl;
            sprite.tex = entity->tex;
            sprite.cell = entity->cell;
            draw_sprite(sprite);
        };
        entity_loop(fnDrawEntity);
        os_swap_buffers();
    }

    texture_done(&tex);
    world_done();
    draw_done();
    app_done();
}