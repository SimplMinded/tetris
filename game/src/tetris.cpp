#include <cstdio>
#include <glad/glad.h>
#include <SDL.h>

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        // TODO: Logging.
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

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

    glClearColor(1, 0, 1, 1);

    while (!SDL_QuitRequested())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(window);
    }

    SDL_Quit();
}
