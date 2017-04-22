#include <assert.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <windows.h>

#include "SDL.h"
#include "SDL_ttf.h"


enum Item {None, GoldPile, Rapier, Spear, SuperRapier, SuperSpear, SuperSword, SuperWhip, Whip};
enum TileType {Nothing, Gold, Upgrade, Berserk, Defense, Healing};


typedef struct
{
    SDL_Surface* image;
    bool isPlayer;
    int health;
} Character;

typedef struct
{
    TileType tileType;
    Character* character;
    Item item;
} Tile;

Tile map[3][3];
Character player;
int playerGold = 0;
int playerXPos;
int playerYPos;
time_t startTime;

SDL_Surface* berserkTile = NULL;
SDL_Surface* defenseTile = NULL;
SDL_Surface* goldTile = NULL;
SDL_Surface* healingTile = NULL;
SDL_Surface* nothingTile = NULL;
SDL_Surface* upgradeTile = NULL;

SDL_Surface* goldPileImage = NULL;

SDL_Surface* backgroundImage = NULL;
SDL_Surface* heartImage = NULL;
SDL_Surface* hero = NULL;
SDL_Surface* skeletonImage = NULL;

SDL_Window* window = NULL;

TTF_Font* font = NULL;

void initTiles()
{
    backgroundImage = SDL_LoadBMP("Background.bmp");
    heartImage = SDL_LoadBMP("Heart.bmp");
    hero = SDL_LoadBMP("Hero.bmp");
    skeletonImage = SDL_LoadBMP("Skeleton.bmp");

    berserkTile = SDL_LoadBMP("Berserk.bmp");
    defenseTile = SDL_LoadBMP("Defense.bmp");
    goldTile = SDL_LoadBMP("Gold.bmp");
    healingTile = SDL_LoadBMP("Healing.bmp");
    nothingTile = SDL_LoadBMP("Nothing.bmp");
    upgradeTile = SDL_LoadBMP("Upgrade.bmp");

    goldPileImage = SDL_LoadBMP("GoldPile.bmp");
}

void logInfo(char* info)
{
    std::ofstream logFile;
    logFile.open("log.txt", std::ios::out | std::ios::app);
    logFile << info << "\n";
    logFile.close();
    logFile.clear();
}

Tile getRandomTile()
{
    Tile tile;
    tile.character = NULL;
    tile.item = Item::None;
    
    switch (rand() % 6)
    {
        case 0:
            tile.tileType = TileType::Berserk;
            break;

        case 1:
            tile.tileType = TileType::Defense;
            break;

        case 2:
            tile.tileType = TileType::Gold;
            break;

        case 3:
            tile.tileType = TileType::Healing;
            break;
        
        case 4:
            tile.tileType = TileType::Nothing;
            break;

        case 5:
            tile.tileType = TileType::Upgrade;
            break;

        default:
            SDL_Log("Whelp, 0-5 doesn't equal any number 0-5. I broke math.");
            tile.tileType = TileType::Nothing;
            break;
    }
    return tile;
}

TileType getRandomTileType()
{
    return getRandomTile().tileType;
}

// I'm assuming the player's position is correctly updated
// before this is called.
// Activates the tile the player's standing on.
void activateTile()
{
    switch (map[playerXPos][playerYPos].tileType)
    {
        case TileType::Gold:
            std::vector<Tile*> openTiles;
            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    if (map[x][y].item == Item::None && !(x == playerXPos && y == playerYPos))
                        openTiles.push_back(&map[x][y]);
                }
            }

            if (openTiles.size() > 0)
            {
                SDL_Log("Giving gold");
                int random = rand() % openTiles.size();
                openTiles[random]->item = Item::GoldPile;
            }

            // Spawn an enemy in a different tile
            openTiles.clear();
            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    if (map[x][y].character == NULL)
                        openTiles.push_back(&map[x][y]);
                }
            }

            if (openTiles.size() > 0)
            {
                SDL_Log("Adding an enemy");
                Character* enemy = new Character;
                enemy->health = 1;
                enemy->image = skeletonImage;
                enemy->isPlayer = false;

                int random = rand() % openTiles.size();
                openTiles[random]->character = enemy;
            }
            break;
    }

    map[playerXPos][playerYPos].tileType = getRandomTileType();
}

void drawTile(int xIndex, int yIndex)
{
    // Draw the type of tile
    SDL_Rect tileRect;
    tileRect.x = 32 + (xIndex * 32);
    tileRect.y = 64 + (yIndex * 32);

    SDL_Surface* tileSurface = NULL;
    switch (map[xIndex][yIndex].tileType)
    {
        case TileType::Berserk:
            tileSurface = berserkTile;
            break;

        case TileType::Defense:
            tileSurface = defenseTile;
            break;

        case TileType::Gold:
            tileSurface = goldTile;
            break;

        case TileType::Healing:
            tileSurface = healingTile;
            break;

        case TileType::Nothing:
            tileSurface = nothingTile;
            break;

        case TileType::Upgrade:
            tileSurface = upgradeTile;
            break;

        default:
            SDL_Log("Unknown tile type.");
            tileSurface = nothingTile;
            break;
    }

    SDL_BlitSurface(tileSurface, NULL, SDL_GetWindowSurface(window), &tileRect);


    // Draw the character on the tile
    if (map[xIndex][yIndex].character != NULL)
    {
        SDL_BlitSurface(map[xIndex][yIndex].character->image, NULL, SDL_GetWindowSurface(window), &tileRect);
    }

    // Draw the item on the tile
    if (map[xIndex][yIndex].item != Item::None)
    {
        SDL_BlitSurface(goldPileImage, NULL, SDL_GetWindowSurface(window), &tileRect);
    }
}

bool moveCharacter(Tile* oldTile, Tile* newTile)
{
    if (oldTile->character == NULL)
    {
        SDL_Log("Moving a nonexistant character.");
        return false;
    }

    newTile->character = oldTile->character;
    oldTile->character = NULL;

    if (newTile->item == Item::GoldPile && newTile->character->isPlayer)
    {
        newTile->item = Item::None;
        playerGold++;
    }
    return true;
}

int distance(int x1, int y1, int x2, int y2)
{
    return std::abs((int)(x1 - x2)) + std::abs((int)(y1 - y2));
}

void moveEnemies()
{
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            if (map[x][y].character != NULL && !map[x][y].character->isPlayer)
            {
                // Can we attack the player?
                if (distance(x, y, playerXPos, playerYPos) == 1)
                {
                    // Yes! Attack!
                    if (map[playerXPos][playerYPos].tileType == TileType::Defense)
                    {
                        // The player takes one less damage, so they're safe!
                    }
                    else if (map[playerXPos][playerYPos].tileType == TileType::Berserk)
                    {
                        // One extra damage
                        player.health -= 2;
                    }
                    else
                    {
                        player.health--;
                    }
                }

                // Apologies: There's gotta be a better way to program AI movement. This code feels awful :(
                if (x == playerXPos)
                {
                    // Same row
                    if (x > playerXPos) moveCharacter(&map[x][y], &map[x - 1][y]);
                    else moveCharacter(&map[x][y], &map[x + 1][y]);
                }
                else if (y == playerYPos)
                {
                    // Same column
                    if (y > playerYPos) moveCharacter(&map[x][y], &map[x][y - 1]);
                    else moveCharacter(&map[x][y], &map[x][y + 1]);
                }
                else
                {
                    // They're not in a straight line.
                    bool moveHorizontal = (rand() % 2) == 0;
                    if (moveHorizontal)
                    {
                        if (x > playerXPos) moveCharacter(&map[x][y], &map[x - 1][y]);
                        else moveCharacter(&map[x][y], &map[x + 1][y]);
                    }
                    else
                    {
                        if (y > playerYPos) moveCharacter(&map[x][y], &map[x][y - 1]);
                        else moveCharacter(&map[x][y], &map[x][y + 1]);
                    }
                }
            }
        }
    }
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
            map[x][y] = getRandomTile();
        }
    }
    map[1][1].character = &player;
    startTime = time(NULL);
    const int TIMER_LENGTH = 10;

    SDL_Event event;
    while (true)
    {
        // TODO: Game over screen
        if (difftime(time(NULL), startTime) >= TIMER_LENGTH || player.health < 1)
            return;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        // TODO: Don't kill other characters by walking on them.
                        case SDLK_DOWN:
                            if (playerYPos < 2)
                            {
                                if (moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos + 1]))
                                {
                                    playerYPos++;
                                    moveEnemies();
                                    activateTile();
                                }
                            }
                            break;

                        case SDLK_LEFT:
                            if (playerXPos > 0)
                            {
                                if (moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos]))
                                {
                                    playerXPos--;
                                    moveEnemies();
                                    activateTile();
                                }
                            }
                            break;
                        
                        case SDLK_RIGHT:
                            if (playerXPos < 2)
                            {
                                if (moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos]))
                                {
                                    playerXPos++;
                                    moveEnemies();
                                    activateTile();
                                }
                            }
                            break;

                        case SDLK_UP:
                            if (playerYPos > 0)
                            {
                                if (moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos - 1]))
                                {
                                    playerYPos--;
                                    moveEnemies();
                                    activateTile();
                                }
                            }
                            break;

                        case SDLK_ESCAPE:
                            return;
                    }
                    break;

                case SDL_QUIT:
                    return;
            }
        }

        // Draw stuff
        // Redraw the background
        SDL_BlitSurface(backgroundImage, NULL, SDL_GetWindowSurface(window), NULL);
        
        // Draw each tile
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 3; y++)
            {
                drawTile(x, y);
            }
        }

        // TODO: Max health.
        int health = player.health;
        for (; health > 0; health--)
        {
            SDL_Rect tileRect;
            tileRect.x = (health - 1) * 10;
            tileRect.y = 0;
            SDL_BlitSurface(heartImage, NULL, SDL_GetWindowSurface(window), &tileRect);           
        }

        // Draw the amount of gold
        SDL_Color white = {255, 255, 255, 0};
        SDL_Rect goldTextRect;
        goldTextRect.x = 0;
        goldTextRect.y = 12;
        char buffer[5];
        char goldText[16];
        strcpy(goldText, "Gold: ");
        strcat(goldText, itoa(playerGold, buffer, 10));
        SDL_Surface* goldTextSurface = TTF_RenderText_Solid(font, goldText, white);
        SDL_BlitSurface(goldTextSurface, NULL, SDL_GetWindowSurface(window), &goldTextRect);

        // Draw the timer
        int secondsSinceStart = difftime(time(NULL), startTime);
        char timeLeftText[5];
        char timePrependText[5];
        
        if (secondsSinceStart >= 60)
        {
            strcpy(timePrependText, "1:");
            secondsSinceStart -= 60;
        }
        else if ((TIMER_LENGTH - secondsSinceStart) < 10)
        {
            strcpy(timePrependText, "0:0");
        }
        else
        {
            strcpy(timePrependText, "0:");
        }

        strcpy(timeLeftText, timePrependText);
        strcat(timeLeftText, itoa(TIMER_LENGTH - secondsSinceStart, buffer, 10));

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, timeLeftText, white);
        SDL_Rect textRect;
        textRect.x = 130;
        textRect.y = 0;
        SDL_BlitSurface(textSurface, NULL, SDL_GetWindowSurface(window), &textRect);

        SDL_UpdateWindowSurface(window);
    } // End of the endless while loop
}

int main()
{
    srand(time(NULL));
    SDL_Log("Initializing SDL");
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    TTF_Init();
    SDL_Log("Finished initializing SDL");

    initTiles();
    font = TTF_OpenFont("cour.ttf", 12);
    player.health = 3;
    player.image = hero;
    player.isPlayer = true;
    playerGold = 0;
    playerXPos = 1;
    playerYPos = 1;

    gameLoop();

    SDL_Log("Shutting down");
    TTF_Quit();
    SDL_Quit();
    SDL_Log("Smell ya later!");
    return 0;
}

// VS Windows applications need WinMain as an entry point
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main();
}