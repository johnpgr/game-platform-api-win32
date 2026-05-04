#define PLATFORM_IMPLEMENTATION
#include "platform.h"

#include <stdio.h>

static f32 clamp01(f32 x) {
    if(x < 0.0f)
        return 0.0f;
    if(x > 1.0f)
        return 1.0f;
    return x;
}

PlatformApi api;

int main(int argc, char** argv) {
    (void)argv;

    f32 player_x    = 0.5f;
    f32 player_y    = 0.5f;
    f32 zoom        = 0.5f;
    i32 interactive = argc > 1;
    i32 frame       = 0;

    api.window.size.x        = 960;
    api.window.size.y        = 540;
    api.window.title         = "platform.h testbed";
    api.opengl.major         = 3;
    api.opengl.minor         = 3;
    api.opengl.debug_context = 0;
    api.opengl.compatibility = 0;

    if(!platform_init(&api)) {
        fprintf(stderr, "platform_init failed: %s\n", platform_error());
        return 1;
    }

    printf("platform.h testbed\n");
    printf(
        "OpenGL 3.3 request, window %dx%d\n",
        api.window.size.x,
        api.window.size.y
    );
    printf(
        "WASD/arrows move the color cursor, mouse changes tint, wheel changes "
        "brightness, Escape quits.\n"
    );
    if(!interactive)
        printf(
            "Running smoke mode for 240 frames. Pass any argument for "
            "interactive mode.\n"
        );

    while(!api.quit) {
        f32 dt;
        f32 speed;
        f32 input_x = 0.0f;
        f32 input_y = 0.0f;
        f32 mouse_r;
        f32 mouse_g;
        f32 r, g, b;

        platform_update();

        if(api.keys[PLATFORM_KEY_ESCAPE].went_down)
            platform_request_quit();

        dt    = api.time.now.seconds_delta;
        speed = api.input.shift ? 0.85f : 0.45f;

        input_x += (f32)api.keys['d'].is_down;
        input_x -= (f32)api.keys['a'].is_down;
        input_x += (f32)api.keys[PLATFORM_KEY_RIGHT].is_down;
        input_x -= (f32)api.keys[PLATFORM_KEY_LEFT].is_down;
        input_y += (f32)api.keys['s'].is_down;
        input_y -= (f32)api.keys['w'].is_down;
        input_y += (f32)api.keys[PLATFORM_KEY_DOWN].is_down;
        input_y -= (f32)api.keys[PLATFORM_KEY_UP].is_down;

        if(api.gamepads[0].connected) {
            input_x += api.gamepads[0].left_stick.value.x;
            input_y -= api.gamepads[0].left_stick.value.y;
        }

        player_x = clamp01(player_x + input_x * speed * dt);
        player_y = clamp01(player_y + input_y * speed * dt);
        zoom     = clamp01(zoom + api.mouse.wheel_delta * 0.08f);

        mouse_r = api.window.size.x > 0
                      ? clamp01((f32)api.mouse.x / (f32)api.window.size.x)
                      : 0.0f;
        mouse_g = api.window.size.y > 0
                      ? clamp01((f32)api.mouse.y / (f32)api.window.size.y)
                      : 0.0f;

        r = clamp01(0.10f + player_x * 0.65f + mouse_r * 0.20f);
        g = clamp01(0.08f + player_y * 0.65f + mouse_g * 0.20f);
        b = clamp01(0.18f + zoom * 0.62f);

        if(glViewport)
            glViewport(0, 0, api.window.size.x, api.window.size.y);
        if(glClearColor)
            glClearColor(r, g, b, 1.0f);
        if(glClear)
            glClear(GL_COLOR_BUFFER_BIT);

        platform_swap_buffers();

        if((frame % 60) == 0) {
            printf(
                "frame=%d dt=%.4f window=%dx%d mouse=%d,%d keys[d]=%d "
                "gamepad0=%d\n",
                frame,
                api.time.now.seconds_delta,
                api.window.size.x,
                api.window.size.y,
                api.mouse.x,
                api.mouse.y,
                api.keys['d'].is_down,
                api.gamepads[0].connected
            );
        }

        ++frame;
        if(!interactive && frame >= 240)
            platform_request_quit();
    }

    platform_shutdown();
    printf("testbed finished after %d frames\n", frame);
    return 0;
}
