#include <SDL.h>

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        // TODO: Logging.
        return 1;
    }

    SDL_CreateWindow("Tetris", 0, 0, 720, 480, SDL_WINDOW_OPENGL);

    while (!SDL_QuitRequested())
    {
        
    }

    SDL_Quit();
}
