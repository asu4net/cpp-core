#include "app.h"
#include "draw.h"

struct Ball {
    AABB box;
    Vec2 vel;
    f32 speed = 0.0f;
};

struct Player {
    AABB box;
    f32 speed = 0.0f;
};

struct Game_State {
    bool started = false;
    Audio_Handle blip;
    Audio_Handle blip2;
    Player pR;
    Player pL;
    Ball ball;
    s32 rPoints = 0;
    s32 lPoints = 0;
};

constexpr Vec2 Half_World = { 3.f * (16.f / 9.f), 3.f};

struct {
    f32 Left    = -Half_World.x;
    f32 Right   = +Half_World.x;
    f32 Bottom  = -Half_World.y;
    f32 Top     = +Half_World.y;
} constexpr Walls;

fn player_l_start_x(const Game_State& gs) -> f32 {
    return Walls.Left + (gs.pL.box.half_w + 0.5f);
}

fn player_r_start_x(const Game_State& gs) -> f32 {
    return Walls.Right - (gs.pL.box.half_w + 0.5f);
}

fn reset_players(Game_State& gs) {

    auto& pL  = gs.pL;
    auto& pR  = gs.pR;

    pL.speed = 5.f;
    pL.box.half_h = 0.5f;
    pL.box.half_w = 0.1f;
    pL.box.x = player_l_start_x(gs);
    pL.box.y = 0.0f;

    pR.speed = 5.f;
    pR.box.half_h = 0.5f;
    pR.box.half_w = 0.1f;
    pR.box.x = player_r_start_x(gs);
    pR.box.y = 0.0f;
}

fn reset_ball(Game_State& gs) {
    auto& ball = gs.ball;

    ball.speed = 7.0f;
    ball.vel = Vec2{ 0.8f, 0.2f } * ball.speed;
    ball.box.half_h = 0.15f;
    ball.box.half_w = 0.15f;
    ball.box.x = 0.0f;
    ball.box.y = 0.7f;
}

fn reset_game_state(Game_State& gs) {
    gs.lPoints = 0;
    gs.rPoints = 0;
    gs.started = false;
    reset_ball(gs);
    reset_players(gs);
}

fn tick_player(Player& p, bool up, bool down, f32 dt) {
    auto& box = p.box;

    if (up) {
        box.y += p.speed * dt;
    }
    if (down) {
        box.y -= p.speed * dt;
    }

    f32 top = box.y + box.half_h;
    f32 bottom = box.y - box.half_h;

    if (top > Walls.Top) {
        box.y = Walls.Top - box.half_h;
    }
    if (bottom < Walls.Bottom) {
        box.y = Walls.Bottom + box.half_h;
    }
}

fn tick_ball(Ball& b, f32 dt) {
    b.box.x += b.vel.x * dt;
    b.box.y += b.vel.y * dt;
}

fn test_ball_walls(Game_State& gs) {
    auto& box = gs.ball.box;
    auto& vel = gs.ball.vel;
    
    f32 top = box.y + box.half_h;
    f32 bottom = box.y - box.half_h;
    f32 left = box.x - box.half_w;
    f32 right = box.x + box.half_w;

    if (top >= Walls.Top) {
        box.y = Walls.Top - box.half_h;
        vel.y *= -1;
        io_audio_play(gs.blip);
    } else if (bottom <= Walls.Bottom) {
        box.y = Walls.Bottom + box.half_h;
        vel.y *= -1;
        io_audio_play(gs.blip);
    } else if (right >= Walls.Right) {
        box.x = Walls.Right - box.half_w;
        vel.x *= -1;
        io_audio_play(gs.blip);
    } else if (left <= Walls.Left) {
        box.x = Walls.Left + box.half_w;
        vel.x *= -1;
        io_audio_play(gs.blip);
    }
}

fn test_ball_point(Game_State& gs) {
    auto& box = gs.ball.box;
    auto& vel = gs.ball.vel;
    
    f32 left = box.x - box.half_w;
    f32 right = box.x + box.half_w;

    s32 points = 0;

    if (right >= player_r_start_x(gs)) {
        gs.lPoints += 1;
        points = gs.lPoints;
    }
    if (left <= player_l_start_x(gs)) {
        gs.rPoints += 1;
        points = gs.rPoints;
    }
    
    if (points > 10) {
        reset_game_state(gs);
    } else if (points > 0) {
        io_audio_play(gs.blip2);
        reset_players(gs);
        reset_ball(gs);
    }
}

fn test_ball_player(Game_State& gs, Player& p) {
    auto& b = gs.ball;

    if (!AABB::overlap(b.box, p.box)) {
        return;
    }

    if (b.vel.x > 0) {
        b.box.x = p.box.x - p.box.half_w - b.box.half_w;
    } else {
        b.box.x = p.box.x + p.box.half_w + b.box.half_w;
    }

    float offset = b.box.y - p.box.y / p.box.half_h;

    Vec2 dir;
    dir.x = (b.vel.x > 0) ? -1.0f : 1.0f;
    dir.y = offset;

    dir = dir.normalized();
    b.vel = dir * b.speed;
    io_audio_play(gs.blip);
} 

fn draw_box(const AABB& box) {
    Draw_Sprite sprite;
    sprite.pos = { box.x, box.y, 0.0f };
    sprite.scl = { box.half_w * 2.0f, box.half_h * 2.0f, 1.0f };
    draw_sprite(sprite);
}

fn main() -> s32 {

    App_Desc ds;
    ds.init_imgui = true;
    ds.window.title = L"Pong";
    app_init(ds);

    draw_init();

    Game_State gs;
    gs.blip = io_audio_load("pong_blip.wav");
    gs.blip2 = io_audio_load("pong_blip_2.wav");
    reset_game_state(gs);
    
    while (app_running())
    {
        const f32 dt = os_delta_time();

        // Logic.
        if (os_key_down('R')) {
            reset_game_state(gs);
        }
        if (os_key_down('M')) {
            io_audio_set_volume(gs.blip, 0.0f);
            io_audio_set_volume(gs.blip2, 0.0f);
        }
        if (os_key_down(Key_Code::Shift) && os_key_down('M')) {
            io_audio_set_volume(gs.blip, 1.0f);
            io_audio_set_volume(gs.blip2, 1.0f);
        }

        auto& pL = gs.pL;
        auto& pR = gs.pR;
        auto& ball = gs.ball;
        
        if (gs.started) {
            tick_player(pL, os_key_down('W'), os_key_down('S'), dt);
            tick_player(pR, os_key_down(Key_Code::Arrow_Up), os_key_down(Key_Code::Arrow_Down), dt);
        }
        
        tick_ball(ball, dt);
        
        // Collisions.
        bool collision = false;

        if (gs.started) {
            test_ball_player(gs, pL);
            test_ball_player(gs, pR);
            test_ball_point(gs);
        }

        test_ball_walls(gs);

        // Clear the back buffer.
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw.
        if (gs.started) {
            draw_box(pL.box);
            draw_box(pR.box);

            for (s32 i = 0; i < 8; ++i) {
                draw_box({ 0.0f, (f32)i / 2.7f, 0.05f, 0.1f });
                draw_box({ 0.0f, - (f32)i / 2.7f, 0.05f, 0.1f });
            }
        }

        draw_box(ball.box);

        // Draw the gui
        imgui_frame_init();
        {
            if (!gs.started) {
                imgui_draw_text({ 1270.0f / 2.85f, 720.0f / 2.6f }, 64.0f, Color.White, "Press Space");
                if (os_key_down(Key_Code::Space)) {
                    gs.started = true;
                }
            }
            else
            {
                imgui_draw_text({ 1270.0f - 350.0f, 50.0f }, 120.0f, Color.White, "%i", gs.rPoints);
                imgui_draw_text({ 300.0f, 50.0f }, 120.0f, Color.White, "%i", gs.lPoints);
            }
        }
        imgui_frame_done();
        
        // Swap the front and back buffers.
        os_swap_buffers();
    }

    io_audio_free(gs.blip);
    io_audio_free(gs.blip2);
    draw_done();
    app_done();
}