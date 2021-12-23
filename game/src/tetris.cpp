#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <glad/glad.h>
#include <SDL.h>

#include "color.h"
#include "list_view.h"
#include "matrix.h"
#include "point.h"
#include "renderer.h"

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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
    } input = {};
    while (!SDL_QuitRequested())
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                // TODO: Should we handle the quite event instead of using
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
                    bool newState = (event.button.state == SDL_PRESSED);
                    switch (event.button.button)
                    {
                        case SDL_BUTTON_LEFT: input.leftMouseButton = newState; break;
                        case SDL_BUTTON_RIGHT: input.rightMouseButton = newState; break;
                        case SDL_BUTTON_MIDDLE: input.middleMouseButton = newState; break;
                    }
                    break;
            }
        }

        int32_t posX = input.left ? input.right ? 180 : 90 : input.right ? 270 : 180;
        int32_t posY = input.swap ? input.down ? 120 : 60 : input.down ? 180 : 120;
        Color color = input.drop ? Color{ 0, 1, 0 } : input.pause ? Color{ 1, 0, 0 } : Color{ 1, 1, 0 };
        int32_t width = input.turnLeft ? 180 : 360;
        int32_t height = input.turnRight ? 120 : 240;

        if (!(input.cursorPos.x < posX || input.cursorPos.x > (posX + width) || input.cursorPos.y < posY || input.cursorPos.y > (posY + height)))
            if (input.leftMouseButton || input.rightMouseButton || input.middleMouseButton)
                color = Color{ 0, 1, 1 };

        beginDrawing();
        drawQuad(posX, posY, width, height, color);
        endDrawing();
        SDL_GL_SwapWindow(window);
    }

    destroyRenderer();

    SDL_Quit();
}
