#define PLATFORM_IMPLEMENTATION
#include "platform.h"

#include <stdio.h>

static f32 clampf(f32 x, f32 lo, f32 hi) {
    if(x < lo)
        return lo;
    if(x > hi)
        return hi;
    return x;
}

typedef struct Player {
    f32 x;
    f32 y;
    f32 size;
} Player;

PlatformApi api;

int main(int argc, char** argv) {
    Player player  = { 500.0f, 375.0f, 32.0f };
    i32 interactive = argc > 1;
    i32 frame       = 0;

    (void)argv;

    api.window.size.x        = 1000;
    api.window.size.y        = 750;
    api.window.title         = "platform.h OpenGL compatibility testbed";
    api.opengl.major         = 2;
    api.opengl.minor         = 1;
    api.opengl.compatibility = 1;
    api.opengl.debug_context = 0;

    if(!platform_init(&api)) {
        fprintf(stderr, "platform_init failed: %s\n", platform_error());
        return 1;
    }

    printf("platform.h OpenGL compatibility testbed\n");
    printf(
        "OpenGL 2.1 compatibility request, window %dx%d\n",
        api.window.size.x,
        api.window.size.y
    );
    printf(
        "Uses fixed-function matrix stack and immediate mode: glMatrixMode, "
        "glOrtho, glBegin, glVertex2i.\n"
    );
    printf("WASD/arrows move the square, wheel scales it, Escape quits.\n");
    if(!interactive)
        printf(
            "Running smoke mode for 240 frames. Pass any argument for "
            "interactive mode.\n"
        );

    while(!api.quit) {
        f32 dt;
        f32 speed;
        f32 left;
        f32 right;
        f32 top;
        f32 bottom;

        platform_update();

        if(api.keys[PLATFORM_KEY_ESCAPE].went_down)
            platform_request_quit();

        dt    = api.time.now.seconds_delta;
        speed = api.input.shift ? 520.0f : 280.0f;

        if(api.keys['a'].is_down || api.keys[PLATFORM_KEY_LEFT].is_down)
            player.x -= speed * dt;
        if(api.keys['d'].is_down || api.keys[PLATFORM_KEY_RIGHT].is_down)
            player.x += speed * dt;
        if(api.keys['w'].is_down || api.keys[PLATFORM_KEY_UP].is_down)
            player.y -= speed * dt;
        if(api.keys['s'].is_down || api.keys[PLATFORM_KEY_DOWN].is_down)
            player.y += speed * dt;

        if(api.gamepads[0].connected) {
            player.x += api.gamepads[0].left_stick.value.x * speed * dt;
            player.y -= api.gamepads[0].left_stick.value.y * speed * dt;
        }

        player.size =
            clampf(player.size + api.mouse.wheel_delta * 4.0f, 8.0f, 96.0f);
        player.x = clampf(player.x, player.size, 1000.0f - player.size);
        player.y = clampf(player.y, player.size, 750.0f - player.size);

        left   = -player.size;
        right  = player.size;
        top    = -player.size;
        bottom = player.size;

        glViewport(0, 0, api.window.size.x, api.window.size.y);
        glClearColor(0.25f, 0.25f, 0.25f, 0.25f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 1000.0, 750.0, 0.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glPushMatrix();
        glTranslatef(player.x, player.y, 0.0f);

        glColor3f(1.0f, 0.15f, 0.10f);
        glBegin(GL_POLYGON);
        glVertex2i((GLint)left, (GLint)top);
        glVertex2i((GLint)right, (GLint)top);
        glVertex2i((GLint)right, (GLint)bottom);
        glVertex2i((GLint)left, (GLint)bottom);
        glEnd();

        glPopMatrix();

        glColor3f(0.10f, 0.12f, 0.14f);
        glBegin(GL_LINES);
        glVertex2i(0, 375);
        glVertex2i(1000, 375);
        glVertex2i(500, 0);
        glVertex2i(500, 750);
        glEnd();

        platform_swap_buffers();

        if((frame % 60) == 0) {
            printf(
                "frame=%d dt=%.4f player=(%.1f,%.1f) size=%.1f "
                "mouse=%d,%d gamepad0=%d\n",
                frame,
                api.time.now.seconds_delta,
                player.x,
                player.y,
                player.size,
                api.mouse.x,
                api.mouse.y,
                api.gamepads[0].connected
            );
        }

        ++frame;
        if(!interactive && frame >= 240)
            platform_request_quit();
    }

    platform_shutdown();
    printf("compat testbed finished after %d frames\n", frame);
    return 0;
}
