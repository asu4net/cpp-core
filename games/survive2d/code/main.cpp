#include "app.h"
#include "draw.h"
#include "graphics.h"

fn main() -> s32 {
    
    App_Desc desc;
    desc.window.title = L"Survive 2D";
    app_init(desc);
    draw_init();
    
    Texture tex;
    Texture_Def def;
    def.kind = Texture_Kind::Tileset;
    def.tile_size = 192;
    def.filename = "sprites/Units/Blue Units/Monk/Run.png";
    texture_init(&tex, def);

    Draw_Sprite sprite;
    sprite.tex = &tex;
    sprite.scl = { 3.f, 3.f, 1.f };

    Draw_Sprite spriteA;
    spriteA.pos = Vec3(F32.Right) * 2.f;
    spriteA.tex = &tex;
    spriteA.scl = { 3.f, 3.f, 1.f };
    sprite.cell = 1;

    while(app_running()) {
        
        clear_back_buffer();
        draw_sprite(sprite);
        draw_sprite(spriteA);
        os_swap_buffers();
    }

    texture_done(&tex);
    draw_done();
    app_done();
}