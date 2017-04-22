#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "SDL.h"


enum TileType {Nothing};

typedef struct
{
    TileType tileType;
} Tile;

Tile map[3][3];

SDL_Surface* heartImage = NULL;
SDL_Surface* nothingTile = NULL;
SDL_Window* window = NULL;

void initTiles()
{
    heartImage = SDL_LoadBMP("Heart.bmp");
    nothingTile = SDL_LoadBMP("Nothing.bmp");
}

void logInfo(char* info)
{
    std::ofstream logFile;
    logFile.open("log.txt", std::ios::out | std::ios::app);
    logFile << info << "\n";
    logFile.close();
    logFile.clear();
}

void drawTile(int xIndex, int yIndex)
{
    // Draw stuff
    SDL_Rect tileRect;
    tileRect.x = 32 + (xIndex * 32);
    tileRect.y = 64 + (yIndex * 32);

    SDL_Surface* tileSurface = NULL;
    switch (map[xIndex][yIndex].tileType)
    {
        case TileType::Nothing:
        default:
            tileSurface = nothingTile;
            break;
    }

    SDL_BlitSurface(tileSurface, NULL, SDL_GetWindowSurface(window), &tileRect);
}

void gameLoop()
{
    // TODO: Better title
    window = SDL_CreateWindow("A Small World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              160, 192, 0);

    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            map[x][y].tileType = TileType::Nothing;
        }
    }

    SDL_Event test_event;
    while (true)
    {
        while (SDL_PollEvent(&test_event))
        {
            if (test_event.type == SDL_KEYDOWN && test_event.key.keysym.sym == SDLK_ESCAPE)
            {
                return;
            }
            else if (test_event.type == SDL_QUIT)
            {
                return;
            }
        }

        // Draw stuff
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 3; y++)
            {
                drawTile(x, y);
            }
        }

        int health = 10;
        for (; health > 0; health--)
        {
            SDL_Rect tileRect;
            tileRect.x = (health - 1) * 10;
            tileRect.y = 0;
            SDL_BlitSurface(heartImage, NULL, SDL_GetWindowSurface(window), &tileRect);           
        }

        SDL_UpdateWindowSurface(window);
    }
}

int main()
{
    SDL_Log("Initializing SDL");
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    initTiles();
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