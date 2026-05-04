#ifndef PLATFORM_H
#define PLATFORM_H

/*
   platform.h - small Windows/OpenGL game platform layer

   Usage:
    #define PLATFORM_IMPLEMENTATION
    #include "platform.h"

    PlatformApi platform;

    int main(void) {
        PlatformApi api = {0};
        api.window.size.x = 1280;
        api.window.size.y = 720;
        api.window.title = "game";
        api.opengl.major = 3;
        api.opengl.minor = 3;

        if (!platform_init(&api)) return 1;
        while (!api.quit) {
            platform_update();
            glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            platform_swap_buffers();
        }
        platform_shutdown();
        return 0;
    }
*/

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PLATFORM_API
#define PLATFORM_API extern
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;
typedef intptr_t isize;
typedef uint32_t b32;
typedef float f32;
typedef double f64;

typedef struct Point {
    int x, y;
} Point;

typedef struct Pointf {
    f32 x, y;
} Pointf;

typedef struct TimeInfo {
    f32 seconds_delta; /* OUT */
    f64 seconds;      /* OUT */
    u64 ns;              /* OUT */
    u64 ns_delta;        /* OUT */
    u64 ms;              /* OUT */
    u64 ms_delta;        /* OUT */
} TimeInfo;

typedef struct Button {
    b32 is_down;   /* OUT */
    b32 went_down; /* OUT */
    b32 went_up;   /* OUT */
} Button;

enum {
    PLATFORM_KEY_BACKSPACE = 8,
    PLATFORM_KEY_TAB       = 9,
    PLATFORM_KEY_ENTER     = 13,
    PLATFORM_KEY_SHIFT     = 16,
    PLATFORM_KEY_CONTROL   = 17,
    PLATFORM_KEY_ALT       = 18,
    PLATFORM_KEY_ESCAPE    = 27,
    PLATFORM_KEY_SPACE     = 32,
    PLATFORM_KEY_LEFT      = 256,
    PLATFORM_KEY_UP,
    PLATFORM_KEY_RIGHT,
    PLATFORM_KEY_DOWN,
    PLATFORM_KEY_INSERT,
    PLATFORM_KEY_DELETE,
    PLATFORM_KEY_HOME,
    PLATFORM_KEY_END,
    PLATFORM_KEY_PAGE_UP,
    PLATFORM_KEY_PAGE_DOWN,
    PLATFORM_KEY_F1,
    PLATFORM_KEY_F2,
    PLATFORM_KEY_F3,
    PLATFORM_KEY_F4,
    PLATFORM_KEY_F5,
    PLATFORM_KEY_F6,
    PLATFORM_KEY_F7,
    PLATFORM_KEY_F8,
    PLATFORM_KEY_F9,
    PLATFORM_KEY_F10,
    PLATFORM_KEY_F11,
    PLATFORM_KEY_F12,
    PLATFORM_KEY_COUNT = 512
};

enum {
    PLATFORM_MOUSE_LEFT,
    PLATFORM_MOUSE_MIDDLE,
    PLATFORM_MOUSE_RIGHT,
    PLATFORM_MOUSE_COUNT
};

enum {
    PLATFORM_GAMEPAD_A,
    PLATFORM_GAMEPAD_B,
    PLATFORM_GAMEPAD_X,
    PLATFORM_GAMEPAD_Y,
    PLATFORM_GAMEPAD_START,
    PLATFORM_GAMEPAD_BACK,
    PLATFORM_GAMEPAD_LEFT_SHOULDER,
    PLATFORM_GAMEPAD_RIGHT_SHOULDER,
    PLATFORM_GAMEPAD_DPAD_UP,
    PLATFORM_GAMEPAD_DPAD_DOWN,
    PLATFORM_GAMEPAD_DPAD_LEFT,
    PLATFORM_GAMEPAD_DPAD_RIGHT,
    PLATFORM_GAMEPAD_COUNT
};

#define PLATFORM_MAX_CHARS_PER_FRAME 32

typedef struct MouseState {
    Point position;       /* OUT client/canvas coordinates */
    Point delta_position; /* OUT */
    Point raw_delta;      /* OUT reserved for raw input */
    Button buttons[PLATFORM_MOUSE_COUNT];
    Button left, middle, right;
    f32 wheel_delta; /* OUT */
    int x, y;          /* OUT convenience copy of position */
} MouseState;

typedef struct GamepadStick {
    Pointf raw;           /* OUT -1..1 */
    Pointf value;         /* OUT processed -1..1 */
    f32 magnitude;      /* OUT 0..1 */
    f32 deadzone_inner; /* IN */
    f32 deadzone_outer; /* IN */
    f32 bias;           /* IN reserved */
} GamepadStick;

typedef struct GamepadState {
    b32 connected; /* OUT */
    GamepadStick left_stick;
    GamepadStick right_stick;
    Button buttons[PLATFORM_GAMEPAD_COUNT];
    f32 left_trigger;  /* OUT 0..1 */
    f32 right_trigger; /* OUT 0..1 */
} GamepadState;

typedef struct PlatformApi {
    b32 quit; /* OUT */

    struct {
        Point size;        /* INOUT client size; 0,0 uses default */
        b32 fullscreen;    /* INIT reserved */
        const char* title; /* INIT */
        Point position;    /* OUT screen coordinates */
        b32 forbid_resize; /* INIT */
        b32 focused;       /* OUT */
    } window;

    struct {
        Point size;    /* INOUT canvas size; 0,0 follows window */
        b32 pixelate;  /* IN reserved */
        b32 lock_size; /* IN */
        int x, y;      /* OUT convenience copy of size */
    } draw;

    struct {
        TimeInfo now;                    /* OUT */
        f32 clamped_max_seconds_delta; /* IN */
        TimeInfo clamped;                /* OUT */
        b32 paused;                      /* IN */
        TimeInfo pausable;               /* OUT */
    } time;

    MouseState mouse;                   /* OUT */
    Button keys[PLATFORM_KEY_COUNT];              /* OUT */
    GamepadState gamepads[4];           /* OUT */
    int typing[PLATFORM_MAX_CHARS_PER_FRAME + 1]; /* OUT Unicode codepoints */

    struct {
        int one_event_per_frame;                        /* IN reserved */
        int preserve_all_events;                        /* IN reserved */
        unsigned char key_for_char[PLATFORM_KEY_COUNT]; /* OUT */
        char keynames[PLATFORM_KEY_COUNT][30];          /* OUT reserved */
        b32 shift, control, alt;                        /* OUT */
    } input;

    struct {
        int disable;            /* INIT reserved */
        int sample_rate;        /* INOUT reserved */
        int channels;           /* OUT reserved */
        f32* buffer;          /* OUT reserved */
        int buffer_num_aframes; /* OUT reserved */
    } sound;

    struct {
        int major, minor;          /* INIT reserved */
        b32 compatibility;         /* INIT reserved */
        b32 debug_context;         /* INIT reserved */
        b32 disable_vsync;         /* IN */
        unsigned int renderbuffer; /* OUT default framebuffer, always 0 */
    } opengl;

    struct {
        struct {
            Pointf mouselook_degrees; /* OUT */
            f32 mouselook_scale;    /* IN */
            Pointf wasd_movement;     /* OUT */
            Pointf wasd_smoothed;     /* OUT reserved */
            f32 smooth_start_time;  /* IN reserved */
            f32 smooth_stop_time;   /* IN reserved */
        } fps_input;
        void* memory_buffer;            /* IN reserved */
        size_t memory_size;             /* IN reserved */
        GamepadState gamepad; /* OUT most recently active */
    } extra;

    struct {
        f32 startup_seconds; /* OUT */
        f32 update_seconds;  /* OUT */
    } system;
} PlatformApi;

PLATFORM_API int platform_init(PlatformApi* api);
PLATFORM_API void platform_update(void);
PLATFORM_API void platform_swap_buffers(void);
PLATFORM_API void platform_shutdown(void);
PLATFORM_API void platform_request_quit(void);
PLATFORM_API void platform_set_vsync(int enabled);
PLATFORM_API void* platform_gl_proc(const char* name);
PLATFORM_API const char* platform_error(void);

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef f32 GLfloat;
typedef f32 GLclampf;
typedef f64 GLdouble;
typedef u64 GLuint64;
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

#define GL_FALSE 0
#define GL_TRUE 1
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_TRIANGLES 0x0004
#define GL_LINES 0x0001
#define GL_FLOAT 0x1406
#define GL_UNSIGNED_BYTE 0x1401
#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_INT 0x1405
#define GL_TEXTURE_2D 0x0DE1
#define GL_RGBA 0x1908
#define GL_RGB 0x1907
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84

#ifndef PLATFORM_GLAPI
#ifdef _WIN32
#define PLATFORM_GLAPI __stdcall
#else
#define PLATFORM_GLAPI
#endif
#endif

typedef void(PLATFORM_GLAPI* platform_glClearColor_proc)(
    GLfloat,
    GLfloat,
    GLfloat,
    GLfloat
);
typedef void(PLATFORM_GLAPI* platform_glClear_proc)(GLbitfield);
typedef void(PLATFORM_GLAPI*
                 platform_glViewport_proc)(GLint, GLint, GLsizei, GLsizei);
typedef void(PLATFORM_GLAPI* platform_glGenBuffers_proc)(GLsizei, GLuint*);
typedef void(PLATFORM_GLAPI* platform_glBindBuffer_proc)(GLenum, GLuint);
typedef void(PLATFORM_GLAPI* platform_glBufferData_proc)(
    GLenum,
    GLsizeiptr,
    const void*,
    GLenum
);
typedef void(PLATFORM_GLAPI*
                 platform_glDeleteBuffers_proc)(GLsizei, const GLuint*);
typedef GLuint(PLATFORM_GLAPI* platform_glCreateShader_proc)(GLenum);
typedef void(PLATFORM_GLAPI* platform_glShaderSource_proc)(
    GLuint,
    GLsizei,
    const GLchar* const*,
    const GLint*
);
typedef void(PLATFORM_GLAPI* platform_glCompileShader_proc)(GLuint);
typedef void(PLATFORM_GLAPI*
                 platform_glGetShaderiv_proc)(GLuint, GLenum, GLint*);
typedef void(PLATFORM_GLAPI* platform_glGetShaderInfoLog_proc)(
    GLuint,
    GLsizei,
    GLsizei*,
    GLchar*
);
typedef void(PLATFORM_GLAPI* platform_glDeleteShader_proc)(GLuint);
typedef GLuint(PLATFORM_GLAPI* platform_glCreateProgram_proc)(void);
typedef void(PLATFORM_GLAPI* platform_glAttachShader_proc)(GLuint, GLuint);
typedef void(PLATFORM_GLAPI* platform_glLinkProgram_proc)(GLuint);
typedef void(PLATFORM_GLAPI* platform_glUseProgram_proc)(GLuint);
typedef void(PLATFORM_GLAPI*
                 platform_glGetProgramiv_proc)(GLuint, GLenum, GLint*);
typedef void(PLATFORM_GLAPI* platform_glGetProgramInfoLog_proc)(
    GLuint,
    GLsizei,
    GLsizei*,
    GLchar*
);
typedef void(PLATFORM_GLAPI* platform_glDeleteProgram_proc)(GLuint);
typedef void(PLATFORM_GLAPI* platform_glGenVertexArrays_proc)(GLsizei, GLuint*);
typedef void(PLATFORM_GLAPI* platform_glBindVertexArray_proc)(GLuint);
typedef void(PLATFORM_GLAPI*
                 platform_glDeleteVertexArrays_proc)(GLsizei, const GLuint*);
typedef void(PLATFORM_GLAPI* platform_glEnableVertexAttribArray_proc)(GLuint);
typedef void(PLATFORM_GLAPI* platform_glVertexAttribPointer_proc)(
    GLuint,
    GLint,
    GLenum,
    GLboolean,
    GLsizei,
    const void*
);
typedef void(PLATFORM_GLAPI* platform_glGenTextures_proc)(GLsizei, GLuint*);
typedef void(PLATFORM_GLAPI* platform_glBindTexture_proc)(GLenum, GLuint);
typedef void(PLATFORM_GLAPI*
                 platform_glTexParameteri_proc)(GLenum, GLenum, GLint);
typedef void(PLATFORM_GLAPI* platform_glTexImage2D_proc)(
    GLenum,
    GLint,
    GLint,
    GLsizei,
    GLsizei,
    GLint,
    GLenum,
    GLenum,
    const void*
);
typedef void(PLATFORM_GLAPI*
                 platform_glDeleteTextures_proc)(GLsizei, const GLuint*);

PLATFORM_API platform_glClearColor_proc glClearColor;
PLATFORM_API platform_glClear_proc glClear;
PLATFORM_API platform_glViewport_proc glViewport;
PLATFORM_API platform_glGenBuffers_proc glGenBuffers;
PLATFORM_API platform_glBindBuffer_proc glBindBuffer;
PLATFORM_API platform_glBufferData_proc glBufferData;
PLATFORM_API platform_glDeleteBuffers_proc glDeleteBuffers;
PLATFORM_API platform_glCreateShader_proc glCreateShader;
PLATFORM_API platform_glShaderSource_proc glShaderSource;
PLATFORM_API platform_glCompileShader_proc glCompileShader;
PLATFORM_API platform_glGetShaderiv_proc glGetShaderiv;
PLATFORM_API platform_glGetShaderInfoLog_proc glGetShaderInfoLog;
PLATFORM_API platform_glDeleteShader_proc glDeleteShader;
PLATFORM_API platform_glCreateProgram_proc glCreateProgram;
PLATFORM_API platform_glAttachShader_proc glAttachShader;
PLATFORM_API platform_glLinkProgram_proc glLinkProgram;
PLATFORM_API platform_glUseProgram_proc glUseProgram;
PLATFORM_API platform_glGetProgramiv_proc glGetProgramiv;
PLATFORM_API platform_glGetProgramInfoLog_proc glGetProgramInfoLog;
PLATFORM_API platform_glDeleteProgram_proc glDeleteProgram;
PLATFORM_API platform_glGenVertexArrays_proc glGenVertexArrays;
PLATFORM_API platform_glBindVertexArray_proc glBindVertexArray;
PLATFORM_API platform_glDeleteVertexArrays_proc glDeleteVertexArrays;
PLATFORM_API platform_glEnableVertexAttribArray_proc glEnableVertexAttribArray;
PLATFORM_API platform_glVertexAttribPointer_proc glVertexAttribPointer;
PLATFORM_API platform_glGenTextures_proc glGenTextures;
PLATFORM_API platform_glBindTexture_proc glBindTexture;
PLATFORM_API platform_glTexParameteri_proc glTexParameteri;
PLATFORM_API platform_glTexImage2D_proc glTexImage2D;
PLATFORM_API platform_glDeleteTextures_proc glDeleteTextures;

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_H */

#ifdef PLATFORM_IMPLEMENTATION

#ifdef PLATFORM_API
#undef PLATFORM_API
#endif
#define PLATFORM_API

#include <math.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static void* platform_get_gl_address(const char* name);

static void PLATFORM_GLAPI platform_on_demand_glClearColor(
    GLfloat red,
    GLfloat green,
    GLfloat blue,
    GLfloat alpha
);
static void PLATFORM_GLAPI platform_on_demand_glClear(GLbitfield mask);
static void PLATFORM_GLAPI
platform_on_demand_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
static void PLATFORM_GLAPI
platform_on_demand_glGenBuffers(GLsizei n, GLuint* buffers);
static void PLATFORM_GLAPI
platform_on_demand_glBindBuffer(GLenum target, GLuint buffer);
static void PLATFORM_GLAPI platform_on_demand_glBufferData(
    GLenum target,
    GLsizeiptr size,
    const void* data,
    GLenum usage
);
static void PLATFORM_GLAPI
platform_on_demand_glDeleteBuffers(GLsizei n, const GLuint* buffers);
static GLuint PLATFORM_GLAPI platform_on_demand_glCreateShader(GLenum type);
static void PLATFORM_GLAPI platform_on_demand_glShaderSource(
    GLuint shader,
    GLsizei count,
    const GLchar* const* string,
    const GLint* length
);
static void PLATFORM_GLAPI platform_on_demand_glCompileShader(GLuint shader);
static void PLATFORM_GLAPI
platform_on_demand_glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
static void PLATFORM_GLAPI platform_on_demand_glGetShaderInfoLog(
    GLuint shader,
    GLsizei bufSize,
    GLsizei* length,
    GLchar* infoLog
);
static void PLATFORM_GLAPI platform_on_demand_glDeleteShader(GLuint shader);
static GLuint PLATFORM_GLAPI platform_on_demand_glCreateProgram(void);
static void PLATFORM_GLAPI
platform_on_demand_glAttachShader(GLuint program, GLuint shader);
static void PLATFORM_GLAPI platform_on_demand_glLinkProgram(GLuint program);
static void PLATFORM_GLAPI platform_on_demand_glUseProgram(GLuint program);
static void PLATFORM_GLAPI
platform_on_demand_glGetProgramiv(GLuint program, GLenum pname, GLint* params);
static void PLATFORM_GLAPI platform_on_demand_glGetProgramInfoLog(
    GLuint program,
    GLsizei bufSize,
    GLsizei* length,
    GLchar* infoLog
);
static void PLATFORM_GLAPI platform_on_demand_glDeleteProgram(GLuint program);
static void PLATFORM_GLAPI
platform_on_demand_glGenVertexArrays(GLsizei n, GLuint* arrays);
static void PLATFORM_GLAPI
platform_on_demand_glBindVertexArray(GLuint array);
static void PLATFORM_GLAPI
platform_on_demand_glDeleteVertexArrays(GLsizei n, const GLuint* arrays);
static void PLATFORM_GLAPI
platform_on_demand_glEnableVertexAttribArray(GLuint index);
static void PLATFORM_GLAPI platform_on_demand_glVertexAttribPointer(
    GLuint index,
    GLint size,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    const void* pointer
);
static void PLATFORM_GLAPI
platform_on_demand_glGenTextures(GLsizei n, GLuint* textures);
static void PLATFORM_GLAPI
platform_on_demand_glBindTexture(GLenum target, GLuint texture);
static void PLATFORM_GLAPI
platform_on_demand_glTexParameteri(GLenum target, GLenum pname, GLint param);
static void PLATFORM_GLAPI platform_on_demand_glTexImage2D(
    GLenum target,
    GLint level,
    GLint internalformat,
    GLsizei width,
    GLsizei height,
    GLint border,
    GLenum format,
    GLenum type,
    const void* pixels
);
static void PLATFORM_GLAPI
platform_on_demand_glDeleteTextures(GLsizei n, const GLuint* textures);

platform_glClearColor_proc glClearColor = platform_on_demand_glClearColor;
platform_glClear_proc glClear = platform_on_demand_glClear;
platform_glViewport_proc glViewport = platform_on_demand_glViewport;
platform_glGenBuffers_proc glGenBuffers = platform_on_demand_glGenBuffers;
platform_glBindBuffer_proc glBindBuffer = platform_on_demand_glBindBuffer;
platform_glBufferData_proc glBufferData = platform_on_demand_glBufferData;
platform_glDeleteBuffers_proc glDeleteBuffers = platform_on_demand_glDeleteBuffers;
platform_glCreateShader_proc glCreateShader = platform_on_demand_glCreateShader;
platform_glShaderSource_proc glShaderSource = platform_on_demand_glShaderSource;
platform_glCompileShader_proc glCompileShader = platform_on_demand_glCompileShader;
platform_glGetShaderiv_proc glGetShaderiv = platform_on_demand_glGetShaderiv;
platform_glGetShaderInfoLog_proc glGetShaderInfoLog =
    platform_on_demand_glGetShaderInfoLog;
platform_glDeleteShader_proc glDeleteShader = platform_on_demand_glDeleteShader;
platform_glCreateProgram_proc glCreateProgram =
    platform_on_demand_glCreateProgram;
platform_glAttachShader_proc glAttachShader = platform_on_demand_glAttachShader;
platform_glLinkProgram_proc glLinkProgram = platform_on_demand_glLinkProgram;
platform_glUseProgram_proc glUseProgram = platform_on_demand_glUseProgram;
platform_glGetProgramiv_proc glGetProgramiv =
    platform_on_demand_glGetProgramiv;
platform_glGetProgramInfoLog_proc glGetProgramInfoLog =
    platform_on_demand_glGetProgramInfoLog;
platform_glDeleteProgram_proc glDeleteProgram =
    platform_on_demand_glDeleteProgram;
platform_glGenVertexArrays_proc glGenVertexArrays =
    platform_on_demand_glGenVertexArrays;
platform_glBindVertexArray_proc glBindVertexArray =
    platform_on_demand_glBindVertexArray;
platform_glDeleteVertexArrays_proc glDeleteVertexArrays =
    platform_on_demand_glDeleteVertexArrays;
platform_glEnableVertexAttribArray_proc glEnableVertexAttribArray =
    platform_on_demand_glEnableVertexAttribArray;
platform_glVertexAttribPointer_proc glVertexAttribPointer =
    platform_on_demand_glVertexAttribPointer;
platform_glGenTextures_proc glGenTextures = platform_on_demand_glGenTextures;
platform_glBindTexture_proc glBindTexture = platform_on_demand_glBindTexture;
platform_glTexParameteri_proc glTexParameteri =
    platform_on_demand_glTexParameteri;
platform_glTexImage2D_proc glTexImage2D = platform_on_demand_glTexImage2D;
platform_glDeleteTextures_proc glDeleteTextures =
    platform_on_demand_glDeleteTextures;

#if !defined(_WIN32)
#error platform.h implementation currently supports Windows only.
#endif

#define PLATFORM_STDCALL __stdcall
#define PLATFORM_CALLBACK __stdcall

typedef void* platform_handle;
typedef void* platform_hwnd;
typedef void* platform_hdc;
typedef void* platform_hglrc;
typedef void* platform_hinstance;
typedef intptr_t PlatformLResult;
typedef uintptr_t platform_wparam;
typedef intptr_t platform_lparam;
typedef unsigned long platform_dword;
typedef unsigned int platform_uint;
typedef unsigned short platform_word;
typedef unsigned char platform_byte;
typedef int platform_long;
typedef uintptr_t platform_uintptr;

typedef struct PlatformWinPoint {
    platform_long x, y;
} PlatformWinPoint;

typedef struct PlatformWinRect {
    platform_long left, top, right, bottom;
} PlatformWinRect;

typedef struct PlatformMsg {
    platform_hwnd hwnd;
    platform_uint message;
    platform_wparam wParam;
    platform_lparam lParam;
    platform_dword time;
    PlatformWinPoint pt;
} PlatformMsg;

typedef PlatformLResult(PLATFORM_CALLBACK* platform_wndproc_proc)(
    platform_hwnd,
    platform_uint,
    platform_wparam,
    platform_lparam
);

typedef struct PlatformWndClassEx {
    platform_uint cbSize;
    platform_uint style;
    platform_wndproc_proc lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    platform_hinstance hInstance;
    platform_handle hIcon;
    platform_handle hCursor;
    platform_handle hbrBackground;
    const char* lpszMenuName;
    const char* lpszClassName;
    platform_handle hIconSm;
} PlatformWndClassEx;

typedef struct PlatformPixelFormat {
    platform_word nSize;
    platform_word nVersion;
    platform_dword dwFlags;
    platform_byte iPixelType;
    platform_byte cColorBits;
    platform_byte cRedBits, cRedShift, cGreenBits, cGreenShift, cBlueBits,
        cBlueShift;
    platform_byte cAlphaBits, cAlphaShift;
    platform_byte cAccumBits, cAccumRedBits, cAccumGreenBits, cAccumBlueBits,
        cAccumAlphaBits;
    platform_byte cDepthBits;
    platform_byte cStencilBits;
    platform_byte cAuxBuffers;
    platform_byte iLayerType;
    platform_byte bReserved;
    platform_dword dwLayerMask, dwVisibleMask, dwDamageMask;
} PlatformPixelFormat;

typedef struct PlatformQPCLargeInteger {
    int64_t QuadPart;
} PlatformQPCLargeInteger;

typedef struct PlatformXInputGamepad {
    platform_word wButtons;
    platform_byte bLeftTrigger;
    platform_byte bRightTrigger;
    short sThumbLX;
    short sThumbLY;
    short sThumbRX;
    short sThumbRY;
} PlatformXInputGamepad;

typedef struct PlatformXInputState {
    platform_dword dwPacketNumber;
    PlatformXInputGamepad Gamepad;
} PlatformXInputState;

__declspec(dllimport) platform_handle PLATFORM_STDCALL
LoadLibraryA(const char*);
__declspec(dllimport) int PLATFORM_STDCALL FreeLibrary(platform_handle);
__declspec(dllimport) void* PLATFORM_STDCALL
GetProcAddress(platform_handle, const char*);
__declspec(dllimport) platform_hinstance PLATFORM_STDCALL
GetModuleHandleA(const char*);
__declspec(dllimport) int PLATFORM_STDCALL
QueryPerformanceCounter(PlatformQPCLargeInteger*);
__declspec(dllimport) int PLATFORM_STDCALL
QueryPerformanceFrequency(PlatformQPCLargeInteger*);

#define PLATFORM_WM_DESTROY 0x0002
#define PLATFORM_WM_CLOSE 0x0010
#define PLATFORM_WM_QUIT 0x0012
#define PLATFORM_WM_SIZE 0x0005
#define PLATFORM_WM_SETFOCUS 0x0007
#define PLATFORM_WM_KILLFOCUS 0x0008
#define PLATFORM_WM_MOUSEMOVE 0x0200
#define PLATFORM_WM_LBUTTONDOWN 0x0201
#define PLATFORM_WM_LBUTTONUP 0x0202
#define PLATFORM_WM_RBUTTONDOWN 0x0204
#define PLATFORM_WM_RBUTTONUP 0x0205
#define PLATFORM_WM_MBUTTONDOWN 0x0207
#define PLATFORM_WM_MBUTTONUP 0x0208
#define PLATFORM_WM_MOUSEWHEEL 0x020A
#define PLATFORM_WM_KEYDOWN 0x0100
#define PLATFORM_WM_KEYUP 0x0101
#define PLATFORM_WM_SYSKEYDOWN 0x0104
#define PLATFORM_WM_SYSKEYUP 0x0105
#define PLATFORM_WM_CHAR 0x0102

#define PLATFORM_PM_REMOVE 0x0001
#define PLATFORM_SW_SHOW 5
#define PLATFORM_CS_OWNDC 0x0020
#define PLATFORM_CS_HREDRAW 0x0002
#define PLATFORM_CS_VREDRAW 0x0001
#define PLATFORM_WS_OVERLAPPEDWINDOW 0x00CF0000L
#define PLATFORM_WS_CAPTION 0x00C00000L
#define PLATFORM_WS_SYSMENU 0x00080000L
#define PLATFORM_WS_THICKFRAME 0x00040000L
#define PLATFORM_WS_MINIMIZEBOX 0x00020000L
#define PLATFORM_WS_MAXIMIZEBOX 0x00010000L
#define PLATFORM_WS_VISIBLE 0x10000000L
#define PLATFORM_CW_USEDEFAULT ((int)0x80000000)
#define PLATFORM_PFD_DRAW_TO_WINDOW 0x00000004
#define PLATFORM_PFD_SUPPORT_OPENGL 0x00000020
#define PLATFORM_PFD_u64BUFFER 0x00000001
#define PLATFORM_PFD_TYPE_RGBA 0
#define PLATFORM_PFD_MAIN_PLANE 0
#define PlatformXInputGamepad_DPAD_UP 0x0001
#define PlatformXInputGamepad_DPAD_DOWN 0x0002
#define PlatformXInputGamepad_DPAD_LEFT 0x0004
#define PlatformXInputGamepad_DPAD_RIGHT 0x0008
#define PlatformXInputGamepad_START 0x0010
#define PlatformXInputGamepad_BACK 0x0020
#define PlatformXInputGamepad_LEFT_SHOULDER 0x0100
#define PlatformXInputGamepad_RIGHT_SHOULDER 0x0200
#define PlatformXInputGamepad_A 0x1000
#define PlatformXInputGamepad_B 0x2000
#define PlatformXInputGamepad_X 0x4000
#define PlatformXInputGamepad_Y 0x8000
#define PLATFORM_ERROR_SUCCESS 0L

typedef unsigned short platform_atom;
typedef platform_atom(PLATFORM_STDCALL* platform_RegisterClassExA_proc)(
    const PlatformWndClassEx*
);
typedef platform_hwnd(PLATFORM_STDCALL* platform_CreateWindowExA_proc)(
    platform_dword,
    const char*,
    const char*,
    platform_dword,
    int,
    int,
    int,
    int,
    platform_hwnd,
    platform_handle,
    platform_hinstance,
    void*
);
typedef PlatformLResult(PLATFORM_STDCALL* platform_DefWindowProcA_proc)(
    platform_hwnd,
    platform_uint,
    platform_wparam,
    platform_lparam
);
typedef int(PLATFORM_STDCALL* platform_ShowWindow_proc)(platform_hwnd, int);
typedef int(PLATFORM_STDCALL* platform_UpdateWindow_proc)(platform_hwnd);
typedef int(PLATFORM_STDCALL* platform_DestroyWindow_proc)(platform_hwnd);
typedef void(PLATFORM_STDCALL* platform_PostQuitMessage_proc)(int);
typedef int(PLATFORM_STDCALL* platform_PeekMessageA_proc)(
    PlatformMsg*,
    platform_hwnd,
    platform_uint,
    platform_uint,
    platform_uint
);
typedef int(PLATFORM_STDCALL* platform_TranslateMessage_proc)(
    const PlatformMsg*
);
typedef PlatformLResult(PLATFORM_STDCALL* platform_DispatchMessageA_proc)(
    const PlatformMsg*
);
typedef int(PLATFORM_STDCALL* platform_AdjustWindowRectEx_proc)(
    PlatformWinRect*,
    platform_dword,
    int,
    platform_dword
);
typedef int(PLATFORM_STDCALL*
                platform_GetClientRect_proc)(platform_hwnd, PlatformWinRect*);
typedef int(PLATFORM_STDCALL* platform_ClientToScreen_proc)(
    platform_hwnd,
    PlatformWinPoint*
);
typedef int(PLATFORM_STDCALL* platform_GetCursorPos_proc)(PlatformWinPoint*);
typedef int(PLATFORM_STDCALL* platform_ScreenToClient_proc)(
    platform_hwnd,
    PlatformWinPoint*
);
typedef platform_hdc(PLATFORM_STDCALL* platform_GetDC_proc)(platform_hwnd);
typedef int(PLATFORM_STDCALL* platform_ReleaseDC_proc)(
    platform_hwnd,
    platform_hdc
);
typedef int(PLATFORM_STDCALL* platform_SetCapture_proc)(platform_hwnd);
typedef int(PLATFORM_STDCALL* platform_ReleaseCapture_proc)(void);

typedef int(PLATFORM_STDCALL* platform_ChoosePixelFormat_proc)(
    platform_hdc,
    const PlatformPixelFormat*
);
typedef int(PLATFORM_STDCALL* platform_SetPixelFormat_proc)(
    platform_hdc,
    int,
    const PlatformPixelFormat*
);
typedef int(PLATFORM_STDCALL* platform_SwapBuffers_proc)(platform_hdc);

typedef platform_hglrc(PLATFORM_STDCALL* platform_wglCreateContext_proc)(
    platform_hdc
);
typedef int(PLATFORM_STDCALL* platform_wglMakeCurrent_proc)(
    platform_hdc,
    platform_hglrc
);
typedef int(PLATFORM_STDCALL* platform_wglDeleteContext_proc)(platform_hglrc);
typedef void*(PLATFORM_STDCALL* platform_wglGetProcAddress_proc)(const char*);
typedef platform_hglrc(PLATFORM_STDCALL*
                           platform_wglCreateContextAttribsARB_proc)(
    platform_hdc,
    platform_hglrc,
    const int*
);

typedef unsigned int(PLATFORM_STDCALL* platform_XInputGetState_proc)(
    platform_dword,
    PlatformXInputState*
);
typedef unsigned int(PLATFORM_STDCALL* platform_timeBeginPeriod_proc)(
    platform_uint
);
typedef int(PLATFORM_STDCALL* platform_wglSwapIntervalEXT_proc)(int);

#define PLATFORM_WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define PLATFORM_WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define PLATFORM_WGL_CONTEXT_FLAGS_ARB 0x2094
#define PLATFORM_WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define PLATFORM_WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
#define PLATFORM_WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define PLATFORM_WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

static struct {
    PlatformApi* api;
    platform_handle user32, gdi32, opengl32, winmm, xinput;
    platform_hinstance instance;
    platform_hwnd hwnd;
    platform_hdc hdc;
    platform_hglrc glrc;
    PlatformQPCLargeInteger qpc_frequency, qpc_last;
    char error[256];
    platform_RegisterClassExA_proc RegisterClassExA;
    platform_CreateWindowExA_proc CreateWindowExA;
    platform_DefWindowProcA_proc DefWindowProcA;
    platform_ShowWindow_proc ShowWindow;
    platform_UpdateWindow_proc UpdateWindow;
    platform_DestroyWindow_proc DestroyWindow;
    platform_PostQuitMessage_proc PostQuitMessage;
    platform_PeekMessageA_proc PeekMessageA;
    platform_TranslateMessage_proc TranslateMessage;
    platform_DispatchMessageA_proc DispatchMessageA;
    platform_AdjustWindowRectEx_proc AdjustWindowRectEx;
    platform_GetClientRect_proc GetClientRect;
    platform_ClientToScreen_proc ClientToScreen;
    platform_GetCursorPos_proc GetCursorPos;
    platform_ScreenToClient_proc ScreenToClient;
    platform_GetDC_proc GetDC;
    platform_ReleaseDC_proc ReleaseDC;
    platform_SetCapture_proc SetCapture;
    platform_ReleaseCapture_proc ReleaseCapture;
    platform_ChoosePixelFormat_proc ChoosePixelFormat;
    platform_SetPixelFormat_proc SetPixelFormat;
    platform_SwapBuffers_proc SwapBuffers;
    platform_wglCreateContext_proc wglCreateContext;
    platform_wglMakeCurrent_proc wglMakeCurrent;
    platform_wglDeleteContext_proc wglDeleteContext;
    platform_wglGetProcAddress_proc wglGetProcAddress;
    platform_wglCreateContextAttribsARB_proc wglCreateContextAttribsARB;
    platform_wglSwapIntervalEXT_proc wglSwapIntervalEXT;
    platform_XInputGetState_proc XInputGetState;
    platform_timeBeginPeriod_proc timeBeginPeriod;
    int initialized;
    int typing_count;
} platform_win32;

#define platform (*platform_win32.api)

static void platform_set_error(const char* text) {
    size_t n = strlen(text);
    if(n >= sizeof(platform_win32.error))
        n = sizeof(platform_win32.error) - 1;
    memcpy(platform_win32.error, text, n);
    platform_win32.error[n] = 0;
}

static void* platform_load_proc(platform_handle dll, const char* name) {
    return dll ? GetProcAddress(dll, name) : 0;
}

static int platform_load_libraries(void) {
    platform_win32.user32   = LoadLibraryA("user32.dll");
    platform_win32.gdi32    = LoadLibraryA("gdi32.dll");
    platform_win32.opengl32 = LoadLibraryA("opengl32.dll");
    platform_win32.winmm    = LoadLibraryA("winmm.dll");
    platform_win32.xinput   = LoadLibraryA("xinput1_4.dll");
    if(!platform_win32.xinput)
        platform_win32.xinput = LoadLibraryA("xinput9_1_0.dll");
    if(!platform_win32.xinput)
        platform_win32.xinput = LoadLibraryA("xinput1_3.dll");

    if(!platform_win32.user32 || !platform_win32.gdi32 ||
       !platform_win32.opengl32) {
        platform_set_error("failed to load required Windows DLLs");
        return 0;
    }

#define PLATFORM_LOAD(dll, name)                                               \
    do {                                                                       \
        platform_win32.name = (platform_##name##_proc)                         \
            platform_load_proc(platform_win32.dll, #name);                     \
        if(!platform_win32.name) {                                             \
            platform_set_error("failed to load " #name);                       \
            return 0;                                                          \
        }                                                                      \
    } while(0)
    PLATFORM_LOAD(user32, RegisterClassExA);
    PLATFORM_LOAD(user32, CreateWindowExA);
    PLATFORM_LOAD(user32, DefWindowProcA);
    PLATFORM_LOAD(user32, ShowWindow);
    PLATFORM_LOAD(user32, UpdateWindow);
    PLATFORM_LOAD(user32, DestroyWindow);
    PLATFORM_LOAD(user32, PostQuitMessage);
    PLATFORM_LOAD(user32, PeekMessageA);
    PLATFORM_LOAD(user32, TranslateMessage);
    PLATFORM_LOAD(user32, DispatchMessageA);
    PLATFORM_LOAD(user32, AdjustWindowRectEx);
    PLATFORM_LOAD(user32, GetClientRect);
    PLATFORM_LOAD(user32, ClientToScreen);
    PLATFORM_LOAD(user32, GetCursorPos);
    PLATFORM_LOAD(user32, ScreenToClient);
    PLATFORM_LOAD(user32, GetDC);
    PLATFORM_LOAD(user32, ReleaseDC);
    PLATFORM_LOAD(user32, SetCapture);
    PLATFORM_LOAD(user32, ReleaseCapture);
    PLATFORM_LOAD(gdi32, ChoosePixelFormat);
    PLATFORM_LOAD(gdi32, SetPixelFormat);
    PLATFORM_LOAD(gdi32, SwapBuffers);
    PLATFORM_LOAD(opengl32, wglCreateContext);
    PLATFORM_LOAD(opengl32, wglMakeCurrent);
    PLATFORM_LOAD(opengl32, wglDeleteContext);
    PLATFORM_LOAD(opengl32, wglGetProcAddress);
#undef PLATFORM_LOAD

    if(platform_win32.xinput)
        platform_win32.XInputGetState = (platform_XInputGetState_proc)
            platform_load_proc(platform_win32.xinput, "XInputGetState");
    if(platform_win32.winmm) {
        platform_win32.timeBeginPeriod = (platform_timeBeginPeriod_proc)
            platform_load_proc(platform_win32.winmm, "timeBeginPeriod");
        if(platform_win32.timeBeginPeriod)
            platform_win32.timeBeginPeriod(1);
    }
    return 1;
}

static int platform_key_from_vk(platform_wparam vk) {
    if(vk >= 'A' && vk <= 'Z')
        return (int)vk + 32;
    if((vk >= '0' && vk <= '9') || vk == PLATFORM_KEY_BACKSPACE ||
       vk == PLATFORM_KEY_TAB || vk == PLATFORM_KEY_ENTER ||
       vk == PLATFORM_KEY_SHIFT || vk == PLATFORM_KEY_CONTROL ||
       vk == PLATFORM_KEY_ALT || vk == PLATFORM_KEY_ESCAPE ||
       vk == PLATFORM_KEY_SPACE)
        return (int)vk;
    switch(vk) {
        case 0x25:
            return PLATFORM_KEY_LEFT;
        case 0x26:
            return PLATFORM_KEY_UP;
        case 0x27:
            return PLATFORM_KEY_RIGHT;
        case 0x28:
            return PLATFORM_KEY_DOWN;
        case 0x2D:
            return PLATFORM_KEY_INSERT;
        case 0x2E:
            return PLATFORM_KEY_DELETE;
        case 0x24:
            return PLATFORM_KEY_HOME;
        case 0x23:
            return PLATFORM_KEY_END;
        case 0x21:
            return PLATFORM_KEY_PAGE_UP;
        case 0x22:
            return PLATFORM_KEY_PAGE_DOWN;
        case 0x70:
            return PLATFORM_KEY_F1;
        case 0x71:
            return PLATFORM_KEY_F2;
        case 0x72:
            return PLATFORM_KEY_F3;
        case 0x73:
            return PLATFORM_KEY_F4;
        case 0x74:
            return PLATFORM_KEY_F5;
        case 0x75:
            return PLATFORM_KEY_F6;
        case 0x76:
            return PLATFORM_KEY_F7;
        case 0x77:
            return PLATFORM_KEY_F8;
        case 0x78:
            return PLATFORM_KEY_F9;
        case 0x79:
            return PLATFORM_KEY_F10;
        case 0x7A:
            return PLATFORM_KEY_F11;
        case 0x7B:
            return PLATFORM_KEY_F12;
    }
    return -1;
}

static void platform_set_button(Button* button, int is_down) {
    b32 down = is_down ? 1u : 0u;
    if(button->is_down != down) {
        button->went_down |= down;
        button->went_up |= !down;
        button->is_down = down;
    }
}

static void platform_set_key(platform_wparam vk, int is_down) {
    int key = platform_key_from_vk(vk);
    if(key >= 0 && key < PLATFORM_KEY_COUNT) {
        platform_set_button(&platform.keys[key], is_down);
        if(key >= 'a' && key <= 'z')
            platform_set_button(&platform.keys[key - 32], is_down);
    }
}

static void platform_update_window_metrics(void) {
    PlatformWinRect rect;
    PlatformWinPoint pt;
    if(!platform_win32.hwnd)
        return;
    if(platform_win32.GetClientRect(platform_win32.hwnd, &rect)) {
        platform.window.size.x = rect.right - rect.left;
        platform.window.size.y = rect.bottom - rect.top;
        if(!platform.draw.lock_size || platform.draw.size.x == 0 ||
           platform.draw.size.y == 0)
            platform.draw.size = platform.window.size;
        platform.draw.x = platform.draw.size.x;
        platform.draw.y = platform.draw.size.y;
    }
    pt.x = 0;
    pt.y = 0;
    if(platform_win32.ClientToScreen(platform_win32.hwnd, &pt))
        platform.window.position.x = pt.x, platform.window.position.y = pt.y;
}

static PlatformLResult PLATFORM_CALLBACK platform_wndproc(
    platform_hwnd hwnd,
    platform_uint msg,
    platform_wparam wp,
    platform_lparam lp
) {
    switch(msg) {
        case PLATFORM_WM_CLOSE:
        case PLATFORM_WM_DESTROY:
            platform.quit = 1;
            platform_win32.PostQuitMessage(0);
            return 0;
        case PLATFORM_WM_SETFOCUS:
            platform.window.focused = 1;
            return 0;
        case PLATFORM_WM_KILLFOCUS:
            platform.window.focused = 0;
            return 0;
        case PLATFORM_WM_SIZE:
            platform_update_window_metrics();
            return 0;
        case PLATFORM_WM_KEYDOWN:
        case PLATFORM_WM_SYSKEYDOWN:
            platform_set_key(wp, 1);
            return 0;
        case PLATFORM_WM_KEYUP:
        case PLATFORM_WM_SYSKEYUP:
            platform_set_key(wp, 0);
            return 0;
        case PLATFORM_WM_CHAR:
            if(wp >= 32 &&
               platform_win32.typing_count < PLATFORM_MAX_CHARS_PER_FRAME) {
                platform.typing[platform_win32.typing_count++] = (int)wp;
                platform.typing[platform_win32.typing_count]   = 0;
            }
            return 0;
        case PLATFORM_WM_LBUTTONDOWN:
            platform_set_button(
                &platform.mouse.buttons[PLATFORM_MOUSE_LEFT],
                1
            );
            platform_set_button(&platform.mouse.left, 1);
            platform_win32.SetCapture(hwnd);
            return 0;
        case PLATFORM_WM_LBUTTONUP:
            platform_set_button(
                &platform.mouse.buttons[PLATFORM_MOUSE_LEFT],
                0
            );
            platform_set_button(&platform.mouse.left, 0);
            platform_win32.ReleaseCapture();
            return 0;
        case PLATFORM_WM_MBUTTONDOWN:
            platform_set_button(
                &platform.mouse.buttons[PLATFORM_MOUSE_MIDDLE],
                1
            );
            platform_set_button(&platform.mouse.middle, 1);
            return 0;
        case PLATFORM_WM_MBUTTONUP:
            platform_set_button(
                &platform.mouse.buttons[PLATFORM_MOUSE_MIDDLE],
                0
            );
            platform_set_button(&platform.mouse.middle, 0);
            return 0;
        case PLATFORM_WM_RBUTTONDOWN:
            platform_set_button(
                &platform.mouse.buttons[PLATFORM_MOUSE_RIGHT],
                1
            );
            platform_set_button(&platform.mouse.right, 1);
            return 0;
        case PLATFORM_WM_RBUTTONUP:
            platform_set_button(
                &platform.mouse.buttons[PLATFORM_MOUSE_RIGHT],
                0
            );
            platform_set_button(&platform.mouse.right, 0);
            return 0;
        case PLATFORM_WM_MOUSEWHEEL:
            platform.mouse.wheel_delta +=
                (f32)((short)((wp >> 16) & 0xffff)) / 120.0f;
            return 0;
    }
    return platform_win32.DefWindowProcA(hwnd, msg, wp, lp);
}

static void platform_reset_frame_transitions(void) {
    int i;
    for(i = 0; i < PLATFORM_KEY_COUNT; ++i) {
        platform.keys[i].went_down = 0;
        platform.keys[i].went_up   = 0;
    }
    for(i = 0; i < PLATFORM_MOUSE_COUNT; ++i) {
        platform.mouse.buttons[i].went_down = 0;
        platform.mouse.buttons[i].went_up   = 0;
    }
    platform.mouse.left.went_down = platform.mouse.left.went_up = 0;
    platform.mouse.middle.went_down = platform.mouse.middle.went_up = 0;
    platform.mouse.right.went_down = platform.mouse.right.went_up = 0;
    for(i = 0; i < 4; ++i) {
        int b;
        for(b = 0; b < PLATFORM_GAMEPAD_COUNT; ++b) {
            platform.gamepads[i].buttons[b].went_down = 0;
            platform.gamepads[i].buttons[b].went_up   = 0;
        }
    }
    platform.mouse.delta_position.x = platform.mouse.delta_position.y = 0;
    platform.mouse.raw_delta.x = platform.mouse.raw_delta.y = 0;
    platform.mouse.wheel_delta                              = 0;
    platform.typing[0]                                      = 0;
    platform_win32.typing_count                             = 0;
}

static void* platform_get_gl_address(const char* name) {
    void* p = 0;
    if(platform_win32.wglGetProcAddress)
        p = platform_win32.wglGetProcAddress(name);
    if(!p || p == (void*)1 || p == (void*)2 || p == (void*)3 ||
       p == (void*)(intptr_t)-1)
        p = platform_load_proc(platform_win32.opengl32, name);
    return p;
}

#define PLATFORM_GL_ON_DEMAND_VOID(name, proc_type, params, args)              \
    static void PLATFORM_GLAPI platform_on_demand_##name params {              \
        name = (proc_type)platform_get_gl_address(#name);                      \
        name args;                                                             \
    }

#define PLATFORM_GL_ON_DEMAND_RET(ret, name, proc_type, params, args)          \
    static ret PLATFORM_GLAPI platform_on_demand_##name params {               \
        name = (proc_type)platform_get_gl_address(#name);                      \
        return name args;                                                      \
    }

PLATFORM_GL_ON_DEMAND_VOID(
    glClearColor,
    platform_glClearColor_proc,
    (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha),
    (red, green, blue, alpha)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glClear,
    platform_glClear_proc,
    (GLbitfield mask),
    (mask)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glViewport,
    platform_glViewport_proc,
    (GLint x, GLint y, GLsizei width, GLsizei height),
    (x, y, width, height)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glGenBuffers,
    platform_glGenBuffers_proc,
    (GLsizei n, GLuint* buffers),
    (n, buffers)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glBindBuffer,
    platform_glBindBuffer_proc,
    (GLenum target, GLuint buffer),
    (target, buffer)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glBufferData,
    platform_glBufferData_proc,
    (GLenum target, GLsizeiptr size, const void* data, GLenum usage),
    (target, size, data, usage)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glDeleteBuffers,
    platform_glDeleteBuffers_proc,
    (GLsizei n, const GLuint* buffers),
    (n, buffers)
)
PLATFORM_GL_ON_DEMAND_RET(
    GLuint,
    glCreateShader,
    platform_glCreateShader_proc,
    (GLenum type),
    (type)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glShaderSource,
    platform_glShaderSource_proc,
    (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length),
    (shader, count, string, length)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glCompileShader,
    platform_glCompileShader_proc,
    (GLuint shader),
    (shader)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glGetShaderiv,
    platform_glGetShaderiv_proc,
    (GLuint shader, GLenum pname, GLint* params),
    (shader, pname, params)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glGetShaderInfoLog,
    platform_glGetShaderInfoLog_proc,
    (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog),
    (shader, bufSize, length, infoLog)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glDeleteShader,
    platform_glDeleteShader_proc,
    (GLuint shader),
    (shader)
)
PLATFORM_GL_ON_DEMAND_RET(
    GLuint,
    glCreateProgram,
    platform_glCreateProgram_proc,
    (void),
    ()
)
PLATFORM_GL_ON_DEMAND_VOID(
    glAttachShader,
    platform_glAttachShader_proc,
    (GLuint program, GLuint shader),
    (program, shader)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glLinkProgram,
    platform_glLinkProgram_proc,
    (GLuint program),
    (program)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glUseProgram,
    platform_glUseProgram_proc,
    (GLuint program),
    (program)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glGetProgramiv,
    platform_glGetProgramiv_proc,
    (GLuint program, GLenum pname, GLint* params),
    (program, pname, params)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glGetProgramInfoLog,
    platform_glGetProgramInfoLog_proc,
    (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog),
    (program, bufSize, length, infoLog)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glDeleteProgram,
    platform_glDeleteProgram_proc,
    (GLuint program),
    (program)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glGenVertexArrays,
    platform_glGenVertexArrays_proc,
    (GLsizei n, GLuint* arrays),
    (n, arrays)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glBindVertexArray,
    platform_glBindVertexArray_proc,
    (GLuint array),
    (array)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glDeleteVertexArrays,
    platform_glDeleteVertexArrays_proc,
    (GLsizei n, const GLuint* arrays),
    (n, arrays)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glEnableVertexAttribArray,
    platform_glEnableVertexAttribArray_proc,
    (GLuint index),
    (index)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glVertexAttribPointer,
    platform_glVertexAttribPointer_proc,
    (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer),
    (index, size, type, normalized, stride, pointer)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glGenTextures,
    platform_glGenTextures_proc,
    (GLsizei n, GLuint* textures),
    (n, textures)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glBindTexture,
    platform_glBindTexture_proc,
    (GLenum target, GLuint texture),
    (target, texture)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glTexParameteri,
    platform_glTexParameteri_proc,
    (GLenum target, GLenum pname, GLint param),
    (target, pname, param)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glTexImage2D,
    platform_glTexImage2D_proc,
    (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels),
    (target, level, internalformat, width, height, border, format, type, pixels)
)
PLATFORM_GL_ON_DEMAND_VOID(
    glDeleteTextures,
    platform_glDeleteTextures_proc,
    (GLsizei n, const GLuint* textures),
    (n, textures)
)

#undef PLATFORM_GL_ON_DEMAND_VOID
#undef PLATFORM_GL_ON_DEMAND_RET

static void platform_load_gl(void) {
    glClearColor              = platform_on_demand_glClearColor;
    glClear                   = platform_on_demand_glClear;
    glViewport                = platform_on_demand_glViewport;
    glGenBuffers              = platform_on_demand_glGenBuffers;
    glBindBuffer              = platform_on_demand_glBindBuffer;
    glBufferData              = platform_on_demand_glBufferData;
    glDeleteBuffers           = platform_on_demand_glDeleteBuffers;
    glCreateShader            = platform_on_demand_glCreateShader;
    glShaderSource            = platform_on_demand_glShaderSource;
    glCompileShader           = platform_on_demand_glCompileShader;
    glGetShaderiv             = platform_on_demand_glGetShaderiv;
    glGetShaderInfoLog        = platform_on_demand_glGetShaderInfoLog;
    glDeleteShader            = platform_on_demand_glDeleteShader;
    glCreateProgram           = platform_on_demand_glCreateProgram;
    glAttachShader            = platform_on_demand_glAttachShader;
    glLinkProgram             = platform_on_demand_glLinkProgram;
    glUseProgram              = platform_on_demand_glUseProgram;
    glGetProgramiv            = platform_on_demand_glGetProgramiv;
    glGetProgramInfoLog       = platform_on_demand_glGetProgramInfoLog;
    glDeleteProgram           = platform_on_demand_glDeleteProgram;
    glGenVertexArrays         = platform_on_demand_glGenVertexArrays;
    glBindVertexArray         = platform_on_demand_glBindVertexArray;
    glDeleteVertexArrays      = platform_on_demand_glDeleteVertexArrays;
    glEnableVertexAttribArray = platform_on_demand_glEnableVertexAttribArray;
    glVertexAttribPointer     = platform_on_demand_glVertexAttribPointer;
    glGenTextures             = platform_on_demand_glGenTextures;
    glBindTexture             = platform_on_demand_glBindTexture;
    glTexParameteri           = platform_on_demand_glTexParameteri;
    glTexImage2D              = platform_on_demand_glTexImage2D;
    glDeleteTextures          = platform_on_demand_glDeleteTextures;

    platform_win32.wglSwapIntervalEXT = (platform_wglSwapIntervalEXT_proc)
        platform_get_gl_address("wglSwapIntervalEXT");
}

static f32 platform_process_stick(
    short sx,
    short sy,
    f32 deadzone,
    f32* out_x,
    f32* out_y
) {
    f32 x   = (sx < 0) ? (f32)sx / 32768.0f : (f32)sx / 32767.0f;
    f32 y   = (sy < 0) ? (f32)sy / 32768.0f : (f32)sy / 32767.0f;
    f32 mag = sqrtf(x * x + y * y);
    if(mag < deadzone) {
        *out_x = 0;
        *out_y = 0;
        return 0;
    }
    if(mag > 1.0f)
        mag = 1.0f;
    *out_x = x;
    *out_y = y;
    return mag;
}

static void platform_update_gamepads(void) {
    int i;
    static const struct {
        int index;
        platform_word mask;
    } map[] = {
        {PLATFORM_GAMEPAD_A, PlatformXInputGamepad_A},
        {PLATFORM_GAMEPAD_B, PlatformXInputGamepad_B},
        {PLATFORM_GAMEPAD_X, PlatformXInputGamepad_X},
        {PLATFORM_GAMEPAD_Y, PlatformXInputGamepad_Y},
        {PLATFORM_GAMEPAD_START, PlatformXInputGamepad_START},
        {PLATFORM_GAMEPAD_BACK, PlatformXInputGamepad_BACK},
        {PLATFORM_GAMEPAD_LEFT_SHOULDER, PlatformXInputGamepad_LEFT_SHOULDER},
        {PLATFORM_GAMEPAD_RIGHT_SHOULDER,
         PlatformXInputGamepad_RIGHT_SHOULDER},
        {PLATFORM_GAMEPAD_DPAD_UP, PlatformXInputGamepad_DPAD_UP},
        {PLATFORM_GAMEPAD_DPAD_DOWN, PlatformXInputGamepad_DPAD_DOWN},
        {PLATFORM_GAMEPAD_DPAD_LEFT, PlatformXInputGamepad_DPAD_LEFT},
        {PLATFORM_GAMEPAD_DPAD_RIGHT, PlatformXInputGamepad_DPAD_RIGHT}
    };
    for(i = 0; i < 4; ++i) {
        PlatformXInputState state;
        GamepadState* g = &platform.gamepads[i];
        memset(&state, 0, sizeof(state));
        if(platform_win32.XInputGetState &&
           platform_win32.XInputGetState((platform_dword)i, &state) ==
               PLATFORM_ERROR_SUCCESS) {
            int j;
            g->connected = 1;
            for(j = 0; j < (int)(sizeof(map) / sizeof(map[0])); ++j)
                platform_set_button(
                    &g->buttons[map[j].index],
                    (state.Gamepad.wButtons & map[j].mask) != 0
                );
            g->left_trigger         = state.Gamepad.bLeftTrigger / 255.0f;
            g->right_trigger        = state.Gamepad.bRightTrigger / 255.0f;
            g->left_stick.raw.x     = (state.Gamepad.sThumbLX < 0)
                                          ? state.Gamepad.sThumbLX / 32768.0f
                                          : state.Gamepad.sThumbLX / 32767.0f;
            g->left_stick.raw.y     = (state.Gamepad.sThumbLY < 0)
                                          ? state.Gamepad.sThumbLY / 32768.0f
                                          : state.Gamepad.sThumbLY / 32767.0f;
            g->right_stick.raw.x    = (state.Gamepad.sThumbRX < 0)
                                          ? state.Gamepad.sThumbRX / 32768.0f
                                          : state.Gamepad.sThumbRX / 32767.0f;
            g->right_stick.raw.y    = (state.Gamepad.sThumbRY < 0)
                                          ? state.Gamepad.sThumbRY / 32768.0f
                                          : state.Gamepad.sThumbRY / 32767.0f;
            g->left_stick.magnitude = platform_process_stick(
                state.Gamepad.sThumbLX,
                state.Gamepad.sThumbLY,
                g->left_stick.deadzone_inner,
                &g->left_stick.value.x,
                &g->left_stick.value.y
            );
            g->right_stick.magnitude = platform_process_stick(
                state.Gamepad.sThumbRX,
                state.Gamepad.sThumbRY,
                g->right_stick.deadzone_inner,
                &g->right_stick.value.x,
                &g->right_stick.value.y
            );
            platform.extra.gamepad = *g;
        } else {
            int b;
            g->connected = 0;
            for(b = 0; b < PLATFORM_GAMEPAD_COUNT; ++b)
                platform_set_button(&g->buttons[b], 0);
        }
    }
}

static void platform_update_time(void) {
    PlatformQPCLargeInteger now;
    f64 seconds;
    u64 ns_delta;
    QueryPerformanceCounter(&now);
    seconds = (f64)(now.QuadPart - platform_win32.qpc_last.QuadPart) /
              (f64)platform_win32.qpc_frequency.QuadPart;
    if(seconds < 0)
        seconds = 0;
    platform_win32.qpc_last = now;

    platform.time.now.seconds_delta = (f32)seconds;
    platform.time.now.seconds += seconds;
    ns_delta                   = (u64)(seconds * 1000000000.0);
    platform.time.now.ns_delta = ns_delta;
    platform.time.now.ns += ns_delta;
    platform.time.now.ms_delta = ns_delta / 1000000ULL;
    platform.time.now.ms       = platform.time.now.ns / 1000000ULL;

    platform.time.clamped = platform.time.now;
    if(platform.time.clamped_max_seconds_delta > 0 &&
       platform.time.clamped.seconds_delta >
           platform.time.clamped_max_seconds_delta)
        platform.time.clamped.seconds_delta =
            platform.time.clamped_max_seconds_delta;

    if(!platform.time.paused) {
        platform.time.pausable.seconds_delta =
            platform.time.clamped.seconds_delta;
        platform.time.pausable.seconds += platform.time.pausable.seconds_delta;
        platform.time.pausable.ns_delta =
            (u64)((f64)platform.time.pausable.seconds_delta * 1000000000.0);
        platform.time.pausable.ns += platform.time.pausable.ns_delta;
        platform.time.pausable.ms_delta =
            platform.time.pausable.ns_delta / 1000000ULL;
        platform.time.pausable.ms = platform.time.pausable.ns / 1000000ULL;
    } else {
        platform.time.pausable.seconds_delta = 0;
        platform.time.pausable.ns_delta      = 0;
        platform.time.pausable.ms_delta      = 0;
    }
}

PLATFORM_API int platform_init(PlatformApi* api) {
    PlatformWndClassEx wc;
    PlatformWinRect rect;
    PlatformPixelFormat pfd;
    int pf;
    const char* title = "platform";
    int width = 1280, height = 720;
    platform_dword style = PLATFORM_WS_OVERLAPPEDWINDOW;
    PlatformQPCLargeInteger start, end;
    Point requested_window_size   = {0, 0};
    const char* requested_title   = 0;
    b32 requested_forbid_resize   = 0;
    Point requested_draw_size     = {0, 0};
    b32 requested_draw_pixelate   = 0;
    b32 requested_draw_lock_size  = 0;
    int requested_gl_major        = 0;
    int requested_gl_minor        = 0;
    b32 requested_gl_compat       = 0;
    b32 requested_gl_debug        = 0;
    b32 requested_disable_vsync   = 0;
    f32 requested_clamp_delta   = 0;
    f32 requested_mouse_scale   = 0;
    int i;

    if(!api) {
        platform_set_error("platform_init requires a PlatformApi pointer");
        return 0;
    }

    requested_window_size  = api->window.size;
    requested_title        = api->window.title;
    requested_forbid_resize = api->window.forbid_resize;
    requested_draw_size    = api->draw.size;
    requested_draw_pixelate = api->draw.pixelate;
    requested_draw_lock_size = api->draw.lock_size;
    requested_gl_major     = api->opengl.major;
    requested_gl_minor     = api->opengl.minor;
    requested_gl_compat    = api->opengl.compatibility;
    requested_gl_debug     = api->opengl.debug_context;
    requested_disable_vsync = api->opengl.disable_vsync;
    requested_clamp_delta  = api->time.clamped_max_seconds_delta;
    requested_mouse_scale  = api->extra.fps_input.mouselook_scale;

    memset(&platform_win32, 0, sizeof(platform_win32));
    platform_win32.api = api;
    memset(api, 0, sizeof(*api));
    QueryPerformanceFrequency(&platform_win32.qpc_frequency);
    QueryPerformanceCounter(&start);
    platform_win32.qpc_last = start;

    if(requested_window_size.x > 0)
        width = requested_window_size.x;
    if(requested_window_size.y > 0)
        height = requested_window_size.y;
    if(requested_title)
        title = requested_title;
    platform.window.size.x                   = width;
    platform.window.size.y                   = height;
    platform.window.title                    = title;
    platform.window.forbid_resize            = requested_forbid_resize;
    platform.draw.size                       = requested_draw_size;
    if(platform.draw.size.x <= 0 || platform.draw.size.y <= 0)
        platform.draw.size = platform.window.size;
    platform.draw.x                          = platform.draw.size.x;
    platform.draw.y                          = platform.draw.size.y;
    platform.draw.pixelate                   = requested_draw_pixelate;
    platform.draw.lock_size                  = requested_draw_lock_size;
    platform.opengl.major                    = requested_gl_major;
    platform.opengl.minor                    = requested_gl_minor;
    platform.opengl.compatibility            = requested_gl_compat;
    platform.opengl.debug_context            = requested_gl_debug;
    platform.opengl.disable_vsync            = requested_disable_vsync;
    platform.time.clamped_max_seconds_delta =
        requested_clamp_delta > 0 ? requested_clamp_delta : 0.1f;
    platform.extra.fps_input.mouselook_scale =
        requested_mouse_scale != 0 ? requested_mouse_scale : 0.1f;
    for(i = 0; i < PLATFORM_KEY_COUNT; ++i)
        platform.input.key_for_char[i] = (unsigned char)i;
    for(i = 0; i < 4; ++i) {
        platform.gamepads[i].left_stick.deadzone_inner  = 0.24f;
        platform.gamepads[i].right_stick.deadzone_inner = 0.24f;
        platform.gamepads[i].left_stick.deadzone_outer  = 1.0f;
        platform.gamepads[i].right_stick.deadzone_outer = 1.0f;
    }

    if(!platform_load_libraries())
        return 0;
    platform_win32.instance = GetModuleHandleA(0);

    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style  = PLATFORM_CS_OWNDC | PLATFORM_CS_HREDRAW | PLATFORM_CS_VREDRAW;
    wc.lpfnWndProc   = platform_wndproc;
    wc.hInstance     = platform_win32.instance;
    wc.lpszClassName = "platform_window_class";
    if(!platform_win32.RegisterClassExA(&wc)) {
        platform_set_error("RegisterClassExA failed");
        return 0;
    }

    if(platform.window.forbid_resize)
        style &= ~(PLATFORM_WS_THICKFRAME | PLATFORM_WS_MAXIMIZEBOX);
    rect.left   = 0;
    rect.top    = 0;
    rect.right  = width;
    rect.bottom = height;
    platform_win32.AdjustWindowRectEx(&rect, style, 0, 0);
    platform_win32.hwnd = platform_win32.CreateWindowExA(
        0,
        wc.lpszClassName,
        title,
        style | PLATFORM_WS_VISIBLE,
        PLATFORM_CW_USEDEFAULT,
        PLATFORM_CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        0,
        0,
        platform_win32.instance,
        0
    );
    if(!platform_win32.hwnd) {
        platform_set_error("CreateWindowExA failed");
        return 0;
    }

    platform_win32.hdc = platform_win32.GetDC(platform_win32.hwnd);
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PLATFORM_PFD_DRAW_TO_WINDOW | PLATFORM_PFD_SUPPORT_OPENGL |
                     PLATFORM_PFD_u64BUFFER;
    pfd.iPixelType = PLATFORM_PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType   = PLATFORM_PFD_MAIN_PLANE;
    pf = platform_win32.ChoosePixelFormat(platform_win32.hdc, &pfd);
    if(!pf || !platform_win32.SetPixelFormat(platform_win32.hdc, pf, &pfd)) {
        platform_set_error("failed to set OpenGL pixel format");
        return 0;
    }
    platform_win32.glrc = platform_win32.wglCreateContext(platform_win32.hdc);
    if(!platform_win32.glrc || !platform_win32.wglMakeCurrent(
                                   platform_win32.hdc,
                                   platform_win32.glrc
                               )) {
        platform_set_error("failed to create OpenGL context");
        return 0;
    }

    platform_win32.wglCreateContextAttribsARB =
        (platform_wglCreateContextAttribsARB_proc)platform_win32
            .wglGetProcAddress("wglCreateContextAttribsARB");
    if(platform.opengl.major > 0) {
        int attribs[11];
        int n = 0;
        platform_hglrc modern_glrc;
        attribs[n++] = PLATFORM_WGL_CONTEXT_MAJOR_VERSION_ARB;
        attribs[n++] = platform.opengl.major;
        attribs[n++] = PLATFORM_WGL_CONTEXT_MINOR_VERSION_ARB;
        attribs[n++] = platform.opengl.minor;
        if(platform.opengl.major > 3 ||
           (platform.opengl.major == 3 && platform.opengl.minor >= 2)) {
            attribs[n++] = PLATFORM_WGL_CONTEXT_PROFILE_MASK_ARB;
            attribs[n++] = platform.opengl.compatibility
                               ? PLATFORM_WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
                               : PLATFORM_WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
        }
        if(platform.opengl.debug_context) {
            attribs[n++] = PLATFORM_WGL_CONTEXT_FLAGS_ARB;
            attribs[n++] = PLATFORM_WGL_CONTEXT_DEBUG_BIT_ARB;
        }
        attribs[n++] = 0;

        if(!platform_win32.wglCreateContextAttribsARB) {
            platform_set_error(
                "requested OpenGL version, but wglCreateContextAttribsARB is unavailable"
            );
            return 0;
        }

        modern_glrc = platform_win32.wglCreateContextAttribsARB(
            platform_win32.hdc,
            0,
            attribs
        );
        if(!modern_glrc) {
            platform_set_error("failed to create requested OpenGL context");
            return 0;
        }

        platform_win32.wglMakeCurrent(0, 0);
        platform_win32.wglDeleteContext(platform_win32.glrc);
        platform_win32.glrc = modern_glrc;
        if(!platform_win32.wglMakeCurrent(platform_win32.hdc, platform_win32.glrc)) {
            platform_set_error("failed to activate requested OpenGL context");
            return 0;
        }
    }
    platform_load_gl();
    platform_set_vsync(!platform.opengl.disable_vsync);

    platform_win32.ShowWindow(platform_win32.hwnd, PLATFORM_SW_SHOW);
    platform_win32.UpdateWindow(platform_win32.hwnd);
    platform_update_window_metrics();
    platform_win32.initialized = 1;
    QueryPerformanceCounter(&end);
    platform.system.startup_seconds =
        (f32)((u64)(end.QuadPart - start.QuadPart) /
                (u64)platform_win32.qpc_frequency.QuadPart);
    return 1;
}

PLATFORM_API void platform_update(void) {
    PlatformMsg msg;
    PlatformWinPoint pt;
    Point old_mouse;
    PlatformQPCLargeInteger start, end;

    if(!platform_win32.initialized)
        return;
    QueryPerformanceCounter(&start);
    platform_reset_frame_transitions();
    old_mouse = platform.mouse.position;

    while(platform_win32.PeekMessageA(&msg, 0, 0, 0, PLATFORM_PM_REMOVE)) {
        if(msg.message == PLATFORM_WM_QUIT)
            platform.quit = 1;
        platform_win32.TranslateMessage(&msg);
        platform_win32.DispatchMessageA(&msg);
    }

    if(platform_win32.GetCursorPos(&pt) &&
       platform_win32.ScreenToClient(platform_win32.hwnd, &pt)) {
        platform.mouse.position.x = pt.x;
        platform.mouse.position.y = pt.y;
        platform.mouse.x          = pt.x;
        platform.mouse.y          = pt.y;
        platform.mouse.delta_position.x =
            platform.mouse.position.x - old_mouse.x;
        platform.mouse.delta_position.y =
            platform.mouse.position.y - old_mouse.y;
    }

    platform.input.shift   = platform.keys[PLATFORM_KEY_SHIFT].is_down;
    platform.input.control = platform.keys[PLATFORM_KEY_CONTROL].is_down;
    platform.input.alt     = platform.keys[PLATFORM_KEY_ALT].is_down;
    platform.extra.fps_input.wasd_movement.x =
        (f32)(platform.keys['w'].is_down - platform.keys['s'].is_down);
    platform.extra.fps_input.wasd_movement.y =
        (f32)(platform.keys['a'].is_down - platform.keys['d'].is_down);
    platform.extra.fps_input.mouselook_degrees.x +=
        platform.mouse.delta_position.x *
        platform.extra.fps_input.mouselook_scale;
    platform.extra.fps_input.mouselook_degrees.y +=
        platform.mouse.delta_position.y *
        platform.extra.fps_input.mouselook_scale;

    platform_update_window_metrics();
    platform_update_gamepads();
    platform_update_time();
    QueryPerformanceCounter(&end);
    platform.system.update_seconds =
        (f32)((u64)(end.QuadPart - start.QuadPart) /
                (u64)platform_win32.qpc_frequency.QuadPart);
}

PLATFORM_API void platform_swap_buffers(void) {
    if(platform_win32.hdc)
        platform_win32.SwapBuffers(platform_win32.hdc);
}

PLATFORM_API void platform_shutdown(void) {
    if(platform_win32.glrc) {
        platform_win32.wglMakeCurrent(0, 0);
        platform_win32.wglDeleteContext(platform_win32.glrc);
        platform_win32.glrc = 0;
    }
    if(platform_win32.hwnd && platform_win32.hdc) {
        platform_win32.ReleaseDC(platform_win32.hwnd, platform_win32.hdc);
        platform_win32.hdc = 0;
    }
    if(platform_win32.hwnd) {
        platform_win32.DestroyWindow(platform_win32.hwnd);
        platform_win32.hwnd = 0;
    }
    if(platform_win32.xinput)
        FreeLibrary(platform_win32.xinput);
    if(platform_win32.winmm)
        FreeLibrary(platform_win32.winmm);
    if(platform_win32.opengl32)
        FreeLibrary(platform_win32.opengl32);
    if(platform_win32.gdi32)
        FreeLibrary(platform_win32.gdi32);
    if(platform_win32.user32)
        FreeLibrary(platform_win32.user32);
    platform_win32.initialized = 0;
}

PLATFORM_API void platform_request_quit(void) {
    platform.quit = 1;
}

PLATFORM_API void platform_set_vsync(int enabled) {
    platform.opengl.disable_vsync = !enabled;
    if(platform_win32.wglSwapIntervalEXT)
        platform_win32.wglSwapIntervalEXT(enabled ? 1 : 0);
}

PLATFORM_API void* platform_gl_proc(const char* name) {
    return platform_get_gl_address(name);
}

PLATFORM_API const char* platform_error(void) {
    return platform_win32.error[0] ? platform_win32.error : "";
}

#undef platform

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_IMPLEMENTATION */
