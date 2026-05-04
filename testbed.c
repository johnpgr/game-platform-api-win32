#define PLATFORM_IMPLEMENTATION
#include "platform.h"

#include <stdio.h>

typedef struct GLContext {
    GLuint program;
    GLuint vao;
    GLuint vbo;
    GLint offset_location;
    GLint pulse_location;
} GLContext;

PlatformApi api;

static f32 clampf(f32 x, f32 lo, f32 hi) {
    if(x < lo)
        return lo;
    if(x > hi)
        return hi;
    return x;
}

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    GLint ok      = 0;

    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

    if(!ok) {
        char log[1024];
        GLsizei written = 0;
        glGetShaderInfoLog(shader, (GLsizei)sizeof(log), &written, log);
        fprintf(stderr, "shader compile failed: %.*s\n", (int)written, log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static GLuint create_program(void) {
    static const char* vertex_shader =
        "#version 330 core\n"
        "layout(location = 0) in vec2 in_position;\n"
        "layout(location = 1) in vec3 in_color;\n"
        "uniform vec2 u_offset;\n"
        "uniform float u_pulse;\n"
        "out vec3 v_color;\n"
        "void main(void) {\n"
        "    vec2 p = in_position + u_offset;\n"
        "    gl_Position = vec4(p, 0.0, 1.0);\n"
        "    v_color = in_color * (0.72 + u_pulse * 0.28);\n"
        "}\n";
    static const char* fragment_shader =
        "#version 330 core\n"
        "in vec3 v_color;\n"
        "out vec4 out_color;\n"
        "void main(void) {\n"
        "    out_color = vec4(v_color, 1.0);\n"
        "}\n";

    GLuint vs      = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    GLuint fs      = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);
    GLuint program = 0;
    GLint ok       = 0;

    if(!vs || !fs) {
        if(vs)
            glDeleteShader(vs);
        if(fs)
            glDeleteShader(fs);
        return 0;
    }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &ok);

    glDeleteShader(vs);
    glDeleteShader(fs);

    if(!ok) {
        char log[1024];
        GLsizei written = 0;
        glGetProgramInfoLog(program, (GLsizei)sizeof(log), &written, log);
        fprintf(stderr, "program link failed: %.*s\n", (int)written, log);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

static int create_modern_gl(GLContext* gl) {
    static const GLfloat vertices[] = {
        -0.50f, -0.42f, 1.00f, 0.20f, 0.10f,
         0.50f, -0.42f, 0.10f, 0.80f, 1.00f,
         0.00f,  0.50f, 0.95f, 0.85f, 0.18f,
    };

    gl->program = create_program();
    if(!gl->program)
        return 0;

    glGenVertexArrays(1, &gl->vao);
    glBindVertexArray(gl->vao);

    glGenBuffers(1, &gl->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gl->vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        (GLsizeiptr)sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * (GLsizei)sizeof(GLfloat),
        (const void*)0
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * (GLsizei)sizeof(GLfloat),
        (const void*)(2 * sizeof(GLfloat))
    );
    glEnableVertexAttribArray(1);

    gl->offset_location = glGetUniformLocation(gl->program, "u_offset");
    gl->pulse_location  = glGetUniformLocation(gl->program, "u_pulse");
    return gl->offset_location >= 0 && gl->pulse_location >= 0;
}

static void destroy_modern_gl(GLContext* gl) {
    if(gl->vbo)
        glDeleteBuffers(1, &gl->vbo);
    if(gl->vao)
        glDeleteVertexArrays(1, &gl->vao);
    if(gl->program)
        glDeleteProgram(gl->program);
}

int main(int argc, char** argv) {
    GLContext gl    = { 0 };
    f32 offset_x   = 0.0f;
    f32 offset_y   = 0.0f;
    f32 pulse      = 0.5f;
    i32 interactive = argc > 1;
    i32 frame       = 0;

    (void)argv;

    api.window.size.x        = 960;
    api.window.size.y        = 540;
    api.window.title         = "platform.h modern OpenGL shader testbed";
    api.opengl.major         = 3;
    api.opengl.minor         = 3;
    api.opengl.compatibility = 0;
    api.opengl.debug_context = 0;

    if(!platform_init(&api)) {
        fprintf(stderr, "platform_init failed: %s\n", platform_error());
        return 1;
    }

    if(!create_modern_gl(&gl)) {
        fprintf(stderr, "modern OpenGL setup failed\n");
        destroy_modern_gl(&gl);
        platform_shutdown();
        return 1;
    }

    printf("platform.h modern OpenGL shader testbed\n");
    printf(
        "OpenGL 3.3 core request, window %dx%d\n",
        api.window.size.x,
        api.window.size.y
    );
    printf("Uses VAO/VBO, GLSL shaders, uniforms, and glDrawArrays.\n");
    printf("WASD/arrows move the triangle, wheel changes pulse, Escape quits.\n");
    if(!interactive)
        printf(
            "Running smoke mode for 240 frames. Pass any argument for "
            "interactive mode.\n"
        );

    while(!api.quit) {
        f32 dt;
        f32 speed;

        platform_update();

        if(api.keys[PLATFORM_KEY_ESCAPE].went_down)
            platform_request_quit();

        dt    = api.time.now.seconds_delta;
        speed = api.input.shift ? 1.65f : 0.85f;

        if(api.keys['a'].is_down || api.keys[PLATFORM_KEY_LEFT].is_down)
            offset_x -= speed * dt;
        if(api.keys['d'].is_down || api.keys[PLATFORM_KEY_RIGHT].is_down)
            offset_x += speed * dt;
        if(api.keys['w'].is_down || api.keys[PLATFORM_KEY_UP].is_down)
            offset_y += speed * dt;
        if(api.keys['s'].is_down || api.keys[PLATFORM_KEY_DOWN].is_down)
            offset_y -= speed * dt;

        if(api.gamepads[0].connected) {
            offset_x += api.gamepads[0].left_stick.value.x * speed * dt;
            offset_y += api.gamepads[0].left_stick.value.y * speed * dt;
        }

        offset_x = clampf(offset_x, -0.45f, 0.45f);
        offset_y = clampf(offset_y, -0.35f, 0.35f);
        pulse    = clampf(pulse + api.mouse.wheel_delta * 0.05f, 0.0f, 1.0f);

        glViewport(0, 0, api.window.size.x, api.window.size.y);
        glClearColor(0.06f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(gl.program);
        glUniform2f(gl.offset_location, offset_x, offset_y);
        glUniform1f(gl.pulse_location, pulse);
        glBindVertexArray(gl.vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        platform_swap_buffers();

        if((frame % 60) == 0) {
            printf(
                "frame=%d dt=%.4f offset=(%.2f,%.2f) pulse=%.2f "
                "mouse=%d,%d gamepad0=%d\n",
                frame,
                api.time.now.seconds_delta,
                offset_x,
                offset_y,
                pulse,
                api.mouse.x,
                api.mouse.y,
                api.gamepads[0].connected
            );
        }

        ++frame;
        if(!interactive && frame >= 240)
            platform_request_quit();
    }

    destroy_modern_gl(&gl);
    platform_shutdown();
    printf("modern testbed finished after %d frames\n", frame);
    return 0;
}
