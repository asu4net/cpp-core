#include "gpu_scene2d.h"

#ifdef GAME_GL
    #include "os_gl.h"
    #include "gpu_scene2d_gl.h"
#endif

fn gpu_clear_back_buffer() -> void {
#ifdef GAME_GL
    glClear(GL_COLOR_BUFFER_BIT);
#endif
}

fn gpu_scene2d_init() -> void {
#ifdef GAME_GL
    gpu_scene2d_init_gl();
#endif
}

fn gpu_scene2d_done() -> void {
#ifdef GAME_GL
    gpu_scene2d_done_gl();
#endif
}

fn gpu_draw_quad(const Vec3& pos, const Vec3& rot, const Vec3& scl) -> void {
#ifdef GAME_GL
    gpu_draw_quad_gl(pos, rot, scl);
#endif
}
