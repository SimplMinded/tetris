#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <glad/glad.h>
#include <cmath>
#include <SDL.h>

#include "color.h"
#include "list_view.h"
#include "matrix.h"
#include "point.h"
#include "renderer.h"
#include "vector.h"

static void generateSineWave(int16_t* buffer, int sampleCount, int soundFreq, int start)
{
    for (int i = 0; i < sampleCount; i++)
    {
        buffer[i] = (int16_t)(sinf((((i + start) % soundFreq) / (float)soundFreq) * 3.1415f) * (INT16_MAX / 4));
    }
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        // TODO: Logging.
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG);

    SDL_Window* window = SDL_CreateWindow("Tetris", 0, 0, 720, 480, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        // TODO: Logging.
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if (SDL_GL_CreateContext(window) == nullptr) {
        // TODO: Logging.
        fprintf(stderr, "OpenGL context creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        // TODO: Logging.
        fprintf(stderr, "GLAD initialization failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_AudioSpec inputSpec, outputSpec;
    inputSpec.freq = 48000;
    inputSpec.format = AUDIO_S16;
    inputSpec.channels = 1;
    inputSpec.samples = 4096;
    inputSpec.callback = nullptr;
    inputSpec.userdata = nullptr;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr, false, &inputSpec, &outputSpec, 0);
    
    int16_t* buffer = new int16_t[outputSpec.freq / 60];
    int start = 0;

    SDL_PauseAudioDevice(device, false);

    initRenderer(1, 720, 480);

    glClearColor(1, 0, 1, 1);

    struct {
        bool left;
        bool right;
        bool down;
        bool drop;
        bool swap;
        bool pause;
        bool turnLeft;
        bool turnRight;
        Point cursorPos;
        bool leftMouseButton;
        bool rightMouseButton;
        bool middleMouseButton;
        Vector scroll;
    } input = {};
    while (!SDL_QuitRequested())
    {
        input.scroll = {};

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                // TODO: Should we handle the quit event instead of using
                //       SDL_QuitRequested?
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_a: input.left = true; break;
                        case SDLK_d: input.right = true; break;
                        case SDLK_s: input.down = true; break;
                        case SDLK_SPACE: input.drop = true; break;
                        case SDLK_w: input.swap = true; break;
                        case SDLK_p: input.pause = true; break;
                        case SDLK_q: input.turnLeft = true; break;
                        case SDLK_e: input.turnRight = true; break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_a: input.left = false; break;
                        case SDLK_d: input.right = false; break;
                        case SDLK_s: input.down = false; break;
                        case SDLK_SPACE: input.drop = false; break;
                        case SDLK_w: input.swap = false; break;
                        case SDLK_p: input.pause = false; break;
                        case SDLK_q: input.turnLeft = false; break;
                        case SDLK_e: input.turnRight = false; break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    input.cursorPos = Point{ (float)event.motion.x, (float)event.motion.y };
                    break;
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                    {
                        bool newState = (event.button.state == SDL_PRESSED);
                        switch (event.button.button)
                        {
                            case SDL_BUTTON_LEFT: input.leftMouseButton = newState; break;
                            case SDL_BUTTON_RIGHT: input.rightMouseButton = newState; break;
                            case SDL_BUTTON_MIDDLE: input.middleMouseButton = newState; break;
                        }
                        break;
                    }
                case SDL_MOUSEWHEEL:
                    input.scroll.x = event.wheel.x;
                    input.scroll.y =
                        event.wheel.y * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ?
                            -1 :
                            1);
                    break;
            }
        }

        int32_t posX = (input.left || input.scroll.x < 0) ? (input.right || input.scroll.x > 0) ? 180 : 90 : (input.right || input.scroll.x > 0) ? 270 : 180;
        int32_t posY = (input.swap || input.scroll.y > 0) ? (input.down || input.scroll.y < 0) ? 120 : 60 : (input.down || input.scroll.y < 0) ? 180 : 120;
        Color color = input.drop ? Color{ 0, 1, 0 } : input.pause ? Color{ 1, 0, 0 } : Color{ 1, 1, 0 };
        int32_t width = input.turnLeft ? 180 : 360;
        int32_t height = input.turnRight ? 120 : 240;

        if (!(input.cursorPos.x < posX || input.cursorPos.x > (posX + width) || input.cursorPos.y < posY || input.cursorPos.y > (posY + height)))
            if (input.leftMouseButton || input.rightMouseButton || input.middleMouseButton)
                color = Color{ 0, 1, 1 };

        if (!input.pause) {
            generateSineWave(buffer, outputSpec.freq / 60, outputSpec.freq / 400, start);
            start = (start + (outputSpec.freq / 60)) % (outputSpec.freq / 400);
            SDL_QueueAudio(device, buffer, (outputSpec.freq / 60) * sizeof(int16_t));
        }

        beginDrawing();
        drawQuad(posX, posY, width, height, color);
        endDrawing();
        SDL_GL_SwapWindow(window);
    }

    delete[] buffer;

    destroyRenderer();

    SDL_CloseAudioDevice(device);

    SDL_Quit();
}
