#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "SDL.h"

namespace
{
    std::ofstream logFile;
}

void logInfo(char* info)
{
    std::ofstream logFile;
    logFile.open("log.txt", std::ios::out | std::ios::app);
    logFile << info << "\n";
    logFile.close();
    logFile.clear();
}

void gameLoop()
{
    // TODO: Better title
    SDL_Window* window = SDL_CreateWindow("A Small World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          160, 192, 0);

    SDL_Event test_event;
    while (true)
    {
        while (SDL_PollEvent(&test_event))
        {
            if (test_event.type == SDL_KEYDOWN && test_event.key.keysym.sym == SDLK_ESCAPE)
            {
                return;
            }
        }

        SDL_UpdateWindowSurface(window);
    }
}

int main()
{
    SDL_Log("Initializing SDL");
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    SDL_Log("Finished initializing SDL");

    gameLoop();

    SDL_Log("Shutting down");
    SDL_Quit();
    SDL_Log("Smell ya later!");
    return 0;
}

// VS Windows applications need WinMain as an entry point
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main();
}