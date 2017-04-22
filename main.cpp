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


enum Item {None, GoldPile, Health, Rapier, Shield, SuperRapier, SuperShield, SuperSword, SuperWhip, Whip};
enum TileType {Nothing, Gold, Upgrade, Berserk, Defense, Healing};
enum WeaponType {Flail, Lunge, Straight};


typedef struct
{
    SDL_Surface* image;
    int defense;
    bool isPlayer;
    bool hasMoved;
    int health;
    int strength;
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
WeaponType playerWeaponType;
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
SDL_Surface* rapierImage = NULL;
SDL_Surface* shieldImage = NULL;
SDL_Surface* whipImage = NULL;

SDL_Surface* backgroundImage = NULL;
SDL_Surface* healthImage = NULL;
SDL_Surface* heartImage = NULL;
SDL_Surface* hero = NULL;
SDL_Surface* skeletonImage = NULL;
SDL_Surface* superSkeletonImage = NULL;

SDL_Window* window = NULL;

TTF_Font* font = NULL;

void setAlphaColor(SDL_Surface* surface, int r, int g, int b)
{
    SDL_Log("Setting the color key");
    int color = SDL_MapRGB(surface->format, r, g, b);
    int result = SDL_SetColorKey(surface, SDL_TRUE, color);
    if (result != 0)
    {
        SDL_Log("Error setting the color key.");
    }
}

void initTiles()
{
    backgroundImage = SDL_LoadBMP("Background.bmp");
    healthImage = SDL_LoadBMP("Health.bmp");
    setAlphaColor(healthImage, 255, 255, 255);
    heartImage = SDL_LoadBMP("Heart.bmp");
    setAlphaColor(heartImage, 255, 255, 255);
    hero = SDL_LoadBMP("Hero.bmp");
    setAlphaColor(hero, 255, 255, 255);
    skeletonImage = SDL_LoadBMP("Skeleton.bmp");
    setAlphaColor(skeletonImage, 255, 255, 255);
    superSkeletonImage = SDL_LoadBMP("SuperSkeleton.bmp");
    setAlphaColor(superSkeletonImage, 255, 255, 255);

    berserkTile = SDL_LoadBMP("Berserk.bmp");
    defenseTile = SDL_LoadBMP("Defense.bmp");
    goldTile = SDL_LoadBMP("Gold.bmp");
    healingTile = SDL_LoadBMP("Healing.bmp");
    nothingTile = SDL_LoadBMP("Nothing.bmp");
    upgradeTile = SDL_LoadBMP("Upgrade.bmp");

    goldPileImage = SDL_LoadBMP("GoldPile.bmp");
    setAlphaColor(goldPileImage, 255, 255, 255);
    rapierImage = SDL_LoadBMP("Rapier.bmp");
    setAlphaColor(rapierImage, 255, 255, 255);
    shieldImage = SDL_LoadBMP("Shield.bmp");
    setAlphaColor(shieldImage, 255, 255, 255);
    whipImage = SDL_LoadBMP("Whip.bmp");
    setAlphaColor(whipImage, 255, 255, 255);
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

void spawnItem(Item item)
{
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
        int random = rand() % openTiles.size();
        openTiles[random]->item = item;
    }
}

// I'm assuming the player's position is correctly updated
// before this is called.
// Activates the tile the player's standing on.
void activateTile()
{
    std::vector<Tile*> openTiles;
    switch (map[playerXPos][playerYPos].tileType)
    {
        case TileType::Gold:
            spawnItem(Item::GoldPile);

            // Spawn an enemy in a different tile
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
                // NOTE: Would be better if the hard-coded 50 seconds was based on a percentage of the time limit.
                int timeInGame = difftime(time(NULL), startTime);
                if (rand() % 50 > timeInGame)
                {
                    // Create a regular skeleton
                    enemy->defense = 0;
                    enemy->health = 1;
                    enemy->image = skeletonImage;
                    enemy->isPlayer = false;
                    enemy->strength = 1;
                }
                else
                {
                    // Create a Super Skeleton
                    enemy->defense = 0;
                    enemy->health = 2;
                    enemy->image = superSkeletonImage;
                    enemy->isPlayer = false;
                    enemy->strength = 2;
                }

                int random = rand() % openTiles.size();
                openTiles[random]->character = enemy;
            }
            break;

        case TileType::Healing:
            spawnItem(Item::Health);
            break;

        case TileType::Upgrade:
            Item newItem;
            switch (rand() % 3)
            {
                case 0:
                    newItem = Item::Rapier;
                    break;

                case 1:
                    newItem = Item::Shield;
                    break;

                case 2:
                    newItem = Item::Whip;
                    break;
            }
            spawnItem(newItem);
            break;
    }
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
    tileRect.h = 16;
    SDL_Surface* itemImage = NULL;
    switch (map[xIndex][yIndex].item)
    {
        case Item::GoldPile:
            itemImage = goldPileImage;
            break;

        case Item::Health:
            itemImage = healthImage;
            break;

        case Item::Rapier:
            itemImage = rapierImage;
            break;

        case Item::Shield:
            itemImage = shieldImage;
            break;

        case Item::Whip:
            itemImage = whipImage;
            break;
    }
    SDL_BlitSurface(itemImage, NULL, SDL_GetWindowSurface(window), &tileRect);
}

bool moveCharacter(Tile* oldTile, Tile* newTile)
{
    if (oldTile->character == NULL)
    {
        SDL_Log("Moving a nonexistant character.");
        return false;
    }
    else if (oldTile->character->hasMoved)
    {
        SDL_Log("Moving a character that has already moved.");
        return false;
    }

    newTile->character = oldTile->character;
    oldTile->character = NULL;

    if (newTile->character->isPlayer)
    {
        oldTile->tileType = getRandomTileType();
        if (newTile->item != Item::None)
        {
            if (newTile->item == Item::GoldPile)
            {
                playerGold++;
            }
            else if (newTile->item == Item::Health)
            {
                player.health++;
            }
            else if (newTile->item == Item::Rapier)
            {
                player.defense = 0;
                player.strength = 1;
                playerWeaponType = WeaponType::Lunge;
            }
            else if (newTile->item == Item::Shield)
            {
                player.defense = 1;
                player.strength = 1;
                playerWeaponType = WeaponType::Straight;
            }
            else if (newTile->item == Item::Whip)
            {
                player.defense = 0;
                player.strength = 1;
                playerWeaponType = WeaponType::Flail;
            }

            newTile->item = Item::None;
        }
    }

    newTile->character->hasMoved = true;
    return true;
}

int distance(int x1, int y1, int x2, int y2)
{
    return std::abs((int)(x1 - x2)) + std::abs((int)(y1 - y2));
}

void attackCharacter(Tile* attackerTile, Tile* victimTile)
{
    if (attackerTile->character == NULL || victimTile->character == NULL)
        return;

    int damage = attackerTile->character->strength - victimTile->character->defense;
    if (victimTile->character->isPlayer)
    {
        if (victimTile->tileType == TileType::Berserk)
            damage++;
        else if (victimTile->tileType == TileType::Defense)
            damage--;

        if (damage < 0)
            damage = 0;

        player.health -= damage;
    }
    else
    {
        victimTile->character->health -= damage;
        if (victimTile->character->health <= 0)
        {
            victimTile->character = NULL;
        }
    }
}

void moveEnemies()
{
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            if (map[x][y].character != NULL && !map[x][y].character->isPlayer && !map[x][y].character->hasMoved)
            {
                // Can we attack the player?
                if (distance(x, y, playerXPos, playerYPos) == 1)
                {
                    // Yes! Attack!
                    attackCharacter(&map[x][y], &map[playerXPos][playerYPos]);
                    map[x][y].character->hasMoved = true;
                }

                // Apologies: There's gotta be a better way to program AI movement. This code feels awful :(
                if (y == playerYPos)
                {
                    // Same row
                    if (x > playerXPos) moveCharacter(&map[x][y], &map[x - 1][y]);
                    else moveCharacter(&map[x][y], &map[x + 1][y]);
                }
                else if (x == playerXPos)
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
    const int TIMER_LENGTH = 60;

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
                                if (map[playerXPos][playerYPos + 1].character != NULL)
                                {
                                    attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos + 1]);
                                    if (playerWeaponType == WeaponType::Lunge && map[playerXPos][playerYPos + 1].character == NULL)
                                    {
                                        moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos + 1]);
                                        playerYPos++;
                                        moveEnemies();
                                        activateTile();
                                    }
                                    else
                                    {
                                        moveEnemies();
                                    }
                                }
                                else if (moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos + 1]))
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
                                if (map[playerXPos - 1][playerYPos].character != NULL)
                                {
                                    attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos]);
                                    if (playerWeaponType == WeaponType::Lunge && map[playerXPos - 1][playerYPos].character == NULL)
                                    {
                                        moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos]);
                                        playerXPos--;
                                        moveEnemies();
                                        activateTile();
                                    }
                                    else
                                    {
                                        moveEnemies();
                                    }
                                }
                                else if (moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos]))
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
                                if (map[playerXPos + 1][playerYPos].character != NULL)
                                {
                                    attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos]);
                                    if (playerWeaponType == WeaponType::Lunge && map[playerXPos + 1][playerYPos].character == NULL)
                                    {
                                        moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos]);
                                        playerXPos++;
                                        moveEnemies();
                                        activateTile();
                                    }
                                    else
                                    {
                                        moveEnemies();
                                    }
                                }
                                else if (moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos]))
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
                                if (map[playerXPos][playerYPos - 1].character != NULL)
                                {
                                    attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos - 1]);
                                    if (playerWeaponType == WeaponType::Lunge && map[playerXPos][playerYPos - 1].character == NULL)
                                    {
                                        moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos - 1]);
                                        playerYPos--;
                                        moveEnemies();
                                        activateTile();
                                    }
                                    else
                                    {
                                        moveEnemies();
                                    }
                                }
                                else if (moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos - 1]))
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

        // Let everyone move again
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 3; y++)
            {
                if (map[x][y].character != NULL)
                    map[x][y].character->hasMoved = false;
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
    player.defense = 0;
    player.health = 3;
    player.image = hero;
    player.isPlayer = true;
    player.strength = 1;
    playerGold = 0;
    playerWeaponType = WeaponType::Straight;
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