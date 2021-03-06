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
#include "SDL_mixer.h"
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

int highScore = 0;
Tile map[3][3];
Character player;
int playerGold = 0;
SDL_Surface* playerWeaponImage = NULL;
WeaponType playerWeaponType;
int playerXPos;
int playerYPos;
time_t startTime;
const SDL_Color white = {255, 255, 255, 0};

SDL_Surface* berserkTile = NULL;
SDL_Surface* defenseTile = NULL;
SDL_Surface* goldTile = NULL;
SDL_Surface* healingTile = NULL;
SDL_Surface* nothingTile = NULL;
SDL_Surface* upgradeTile = NULL;

SDL_Surface* smallBerserkTile = NULL;
SDL_Surface* smallDefenseTile = NULL;
SDL_Surface* smallGoldTile = NULL;
SDL_Surface* smallHealingTile = NULL;
SDL_Surface* smallNothingTile = NULL;
SDL_Surface* smallUpgradeTile = NULL;

SDL_Surface* goldPileImage = NULL;
SDL_Surface* rapierImage = NULL;
SDL_Surface* shieldImage = NULL;
SDL_Surface* superRapierImage = NULL;
SDL_Surface* superShieldImage = NULL;
SDL_Surface* superSwordImage = NULL;
SDL_Surface* superWhipImage = NULL;
SDL_Surface* whipImage = NULL;

SDL_Surface* handheldRapierImage = NULL;
SDL_Surface* handheldShieldImage = NULL;
SDL_Surface* handheldSuperRapierImage = NULL;
SDL_Surface* handheldSuperShieldImage = NULL;
SDL_Surface* handheldSuperSwordImage = NULL;
SDL_Surface* handheldSuperWhipImage = NULL;
SDL_Surface* handheldSwordImage = NULL;
SDL_Surface* handheldWhipImage = NULL;

SDL_Surface* backgroundImage = NULL;
SDL_Surface* healthImage = NULL;
SDL_Surface* heartImage = NULL;
SDL_Surface* hero = NULL;
SDL_Surface* skeletonImage = NULL;
SDL_Surface* superSkeletonImage = NULL;

SDL_Window* window = NULL;

TTF_Font* font = NULL;

Mix_Chunk* getGoldSoundEffect = NULL;
Mix_Chunk* healingSoundEffect = NULL;
Mix_Chunk* hitSoundEffect = NULL;
Mix_Chunk* hurtSoundEffect = NULL;
Mix_Chunk* upgradeSoundEffect = NULL;

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
    backgroundImage = SDL_LoadBMP("images/Background.bmp");
    healthImage = SDL_LoadBMP("images/Health.bmp");
    setAlphaColor(healthImage, 255, 255, 255);
    heartImage = SDL_LoadBMP("images/Heart.bmp");
    setAlphaColor(heartImage, 255, 255, 255);
    hero = SDL_LoadBMP("images/Hero.bmp");
    setAlphaColor(hero, 255, 255, 255);
    skeletonImage = SDL_LoadBMP("images/Skeleton.bmp");
    setAlphaColor(skeletonImage, 255, 255, 255);
    superSkeletonImage = SDL_LoadBMP("images/SuperSkeleton.bmp");
    setAlphaColor(superSkeletonImage, 255, 255, 255);

    berserkTile = SDL_LoadBMP("images/Berserk.bmp");
    defenseTile = SDL_LoadBMP("images/Defense.bmp");
    goldTile = SDL_LoadBMP("images/Gold.bmp");
    healingTile = SDL_LoadBMP("images/Healing.bmp");
    nothingTile = SDL_LoadBMP("images/Nothing.bmp");
    upgradeTile = SDL_LoadBMP("images/Upgrade.bmp");

    smallBerserkTile = SDL_LoadBMP("images/SmallBerserk.bmp");
    smallDefenseTile = SDL_LoadBMP("images/SmallDefense.bmp");
    smallGoldTile = SDL_LoadBMP("images/SmallGold.bmp");
    smallHealingTile = SDL_LoadBMP("images/SmallHealing.bmp");
    smallNothingTile = SDL_LoadBMP("images/SmallNothing.bmp");
    smallUpgradeTile = SDL_LoadBMP("images/SmallUpgrade.bmp");

    goldPileImage = SDL_LoadBMP("images/GoldPile.bmp");
    setAlphaColor(goldPileImage, 255, 255, 255);
    rapierImage = SDL_LoadBMP("images/Rapier.bmp");
    setAlphaColor(rapierImage, 255, 255, 255);
    shieldImage = SDL_LoadBMP("images/Shield.bmp");
    setAlphaColor(shieldImage, 255, 255, 255);
    superRapierImage = SDL_LoadBMP("images/SuperRapier.bmp");
    setAlphaColor(superRapierImage, 255, 255, 255);
    superShieldImage = SDL_LoadBMP("images/SuperShield.bmp");
    setAlphaColor(superShieldImage, 255, 255, 255);
    superSwordImage = SDL_LoadBMP("images/SuperSword.bmp");
    setAlphaColor(superSwordImage, 255, 255, 255);
    superWhipImage = SDL_LoadBMP("images/SuperWhip.bmp");
    setAlphaColor(superWhipImage, 255, 255, 255);
    whipImage = SDL_LoadBMP("images/Whip.bmp");
    setAlphaColor(whipImage, 255, 255, 255);

    handheldRapierImage = SDL_LoadBMP("images/HandheldRapier.bmp");
    setAlphaColor(handheldRapierImage, 255, 255, 255);
    handheldShieldImage = SDL_LoadBMP("images/HandheldShield.bmp");
    setAlphaColor(handheldShieldImage, 255, 255, 255);
    handheldSuperRapierImage = SDL_LoadBMP("images/HandheldSuperRapier.bmp");
    setAlphaColor(handheldSuperRapierImage, 255, 255, 255);
    handheldSuperShieldImage = SDL_LoadBMP("images/HandheldSuperShield.bmp");
    setAlphaColor(handheldSuperShieldImage, 255, 255, 255);
    handheldSuperSwordImage = SDL_LoadBMP("images/HandheldSuperSword.bmp");
    setAlphaColor(handheldSuperSwordImage, 255, 255, 255);
    handheldSuperWhipImage = SDL_LoadBMP("images/HandheldSuperWhip.bmp");
    setAlphaColor(handheldSuperWhipImage, 255, 255, 255);
    handheldSwordImage = SDL_LoadBMP("images/HandheldSword.bmp");
    setAlphaColor(handheldSwordImage, 255, 255, 255);
    handheldWhipImage = SDL_LoadBMP("images/HandheldWhip.bmp");
    setAlphaColor(handheldWhipImage, 255, 255, 255);

    // This function may be a little out of its original scope...
    getGoldSoundEffect = Mix_LoadWAV("audio/GetGold.wav");
    healingSoundEffect = Mix_LoadWAV("audio/Healing.wav");
    hitSoundEffect = Mix_LoadWAV("audio/Hit.wav");
    hurtSoundEffect = Mix_LoadWAV("audio/Hurt.wav");
    upgradeSoundEffect = Mix_LoadWAV("audio/Upgrade.wav");
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
    
    int random = rand() % 20;
    if (random < 4)
        tile.tileType = TileType::Berserk;
    else if (random < 8)
        tile.tileType = TileType::Defense;
    else if (random < 11)
        tile.tileType = TileType::Gold;
    else if (random < 12)
        tile.tileType = TileType::Healing;
    else if (random < 17)
        tile.tileType = TileType::Nothing;
    else if (random < 20)
        tile.tileType = TileType::Upgrade;
    else
    {
        SDL_Log("I broke math.");
        tile.tileType = TileType::Nothing;
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
            int random;
            int timeSinceStart = difftime(time(NULL), startTime);
            if (timeSinceStart < 20)
                random = rand() % 3;
            else if (timeSinceStart < 40)
                random = rand() % 7;
            else
                random = (rand() % 3) + 4;

            switch (random)
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
                
                case 3:
                    newItem = Item::SuperSword;
                    break;

                case 4:
                    newItem = Item::SuperShield;
                    break;

                case 5:
                    newItem = Item::SuperRapier;
                    break;

                case 6:
                    newItem = Item::SuperWhip;
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

        // Draw the player's weapon.
        // Totally not a last-minute hack.
        if (xIndex == playerXPos && yIndex == playerYPos)
        {
            SDL_BlitSurface(playerWeaponImage, NULL, SDL_GetWindowSurface(window), &tileRect);
        }
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

        case Item::SuperRapier:
            itemImage = superRapierImage;
            break;

        case Item::SuperShield:
            itemImage = superShieldImage;
            break;

        case Item::SuperSword:
            itemImage = superSwordImage;
            break;

        case Item::SuperWhip:
            itemImage = superWhipImage;
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
        // Create a new tile, but don't let there be more
        // than 3 of the same type of tile.
        while (true)
        {
            // Can't see an infinite loop based on randomness going wrong!
            TileType newTileType = getRandomTileType();
            int numTilesSameType = 0;
            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    if (map[x][y].tileType == newTileType)
                        numTilesSameType++;
                }
            }

            if (numTilesSameType < 3)
            {
                oldTile->tileType = newTileType;
                break;
            }
        }

        if (newTile->item != Item::None)
        {
            if (newTile->item == Item::GoldPile)
            {
                Mix_PlayChannel(2, getGoldSoundEffect, 0);
                playerGold++;
            }
            else if (newTile->item == Item::Health)
            {
                Mix_PlayChannel(2, healingSoundEffect, 0);
                if (player.health < 12)
                    player.health++;
            }
            else if (newTile->item == Item::Rapier)
            {
                Mix_PlayChannel(2, upgradeSoundEffect, 0);
                player.defense = 0;
                player.strength = 1;
                playerWeaponImage = handheldRapierImage;
                playerWeaponType = WeaponType::Lunge;
            }
            else if (newTile->item == Item::Shield)
            {
                Mix_PlayChannel(2, upgradeSoundEffect, 0);
                player.defense = 1;
                player.strength = 1;
                playerWeaponImage = handheldShieldImage;
                playerWeaponType = WeaponType::Straight;
            }
            else if (newTile->item == Item::Whip)
            {
                Mix_PlayChannel(2, upgradeSoundEffect, 0);
                player.defense = 0;
                player.strength = 1;
                playerWeaponImage = handheldWhipImage;
                playerWeaponType = WeaponType::Flail;
            }
            else if (newTile->item == Item::SuperRapier)
            {
                Mix_PlayChannel(2, upgradeSoundEffect, 0);
                player.defense = 0;
                player.strength = 2;
                playerWeaponImage = handheldSuperRapierImage;
                playerWeaponType = WeaponType::Lunge;
            }
            else if (newTile->item == Item::SuperShield)
            {
                Mix_PlayChannel(2, upgradeSoundEffect, 0);
                player.defense = 1;
                player.strength = 2;
                playerWeaponImage = handheldSuperShieldImage;
                playerWeaponType = WeaponType::Straight;
            }
            else if (newTile->item == Item::SuperSword)
            {
                Mix_PlayChannel(2, upgradeSoundEffect, 0);
                player.defense = 0;
                player.strength = 2;
                playerWeaponImage = handheldSuperSwordImage;
                playerWeaponType = WeaponType::Straight;
            }
            else if (newTile->item == Item::SuperWhip)
            {
                Mix_PlayChannel(2, upgradeSoundEffect, 0);
                player.defense = 0;
                player.strength = 2;
                playerWeaponImage = handheldSuperWhipImage;
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
        else if (damage > 0)
            Mix_PlayChannel(2, hurtSoundEffect, 0);

        player.health -= damage;
    }
    else
    {
        Mix_PlayChannel(2, hitSoundEffect, 0);
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

bool gameLoop()
{
    Mix_Chunk* currentMusic = Mix_LoadWAV("audio/SlowMusic1.wav");
    Mix_PlayChannel(1, currentMusic, -1);
    bool playingSlowMusic = true;

    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            map[x][y] = getRandomTile();
        }
    }
    map[1][1].character = &player;
    startTime = time(NULL);
    int startTimeLastFrame = 0;
    const int TIMER_LENGTH = 60;

    SDL_Event event;
    while (true)
    {
        startTimeLastFrame = SDL_GetTicks();

        if (difftime(time(NULL), startTime) >= TIMER_LENGTH || player.health < 1)
        {
            if (playerGold > highScore)
                highScore = playerGold;
            return true;
        }

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_DOWN:
                            if (playerYPos < 2)
                            {
                                if (playerWeaponType == WeaponType::Flail)
                                {
                                    // Whip attack
                                    bool attacking = false;
                                    if (map[playerXPos][playerYPos + 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos + 1]);
                                    }
                                    if (playerXPos - 1 >= 0 && map[playerXPos - 1][playerYPos + 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos + 1]);
                                    }
                                    if (playerXPos + 1 < 3 && map[playerXPos + 1][playerYPos + 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos + 1]);
                                    }

                                    if (attacking)
                                    {
                                        moveEnemies();
                                    }
                                    else if (!attacking && moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos + 1]))
                                    {
                                        playerYPos++;
                                        moveEnemies();
                                        activateTile();
                                    }
                                }
                                else if (map[playerXPos][playerYPos + 1].character != NULL)
                                {
                                    // Standard attack
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
                                    // There's no one to attack and we don't have a whip.
                                    playerYPos++;
                                    moveEnemies();
                                    activateTile();
                                }
                            }
                            break;

                        case SDLK_LEFT:
                            if (playerXPos > 0)
                            {
                                if (playerWeaponType == WeaponType::Flail)
                                {
                                    // Whip attack
                                    bool attacking = false;
                                    if (map[playerXPos - 1][playerYPos].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos]);
                                    }
                                    if (playerYPos - 1 >= 0 && map[playerXPos - 1][playerYPos - 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos - 1]);
                                    }
                                    if (playerYPos + 1 < 3 && map[playerXPos - 1][playerYPos + 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos + 1]);
                                    }

                                    if (attacking)
                                    {
                                        moveEnemies();
                                    }
                                    else if (!attacking && moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos]))
                                    {
                                        playerXPos--;
                                        moveEnemies();
                                        activateTile();
                                    }
                                }
                                else if (map[playerXPos - 1][playerYPos].character != NULL)
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
                                if (playerWeaponType == WeaponType::Flail)
                                {
                                    // Whip attack
                                    bool attacking = false;
                                    if (map[playerXPos + 1][playerYPos].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos]);
                                    }
                                    if (playerYPos - 1 >= 0 && map[playerXPos + 1][playerYPos - 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos - 1]);
                                    }
                                    if (playerYPos + 1 < 3 && map[playerXPos + 1][playerYPos + 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos + 1]);
                                    }

                                    if (attacking)
                                    {
                                        moveEnemies();
                                    }
                                    else if (!attacking && moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos]))
                                    {
                                        playerXPos++;
                                        moveEnemies();
                                        activateTile();
                                    }
                                }
                                else if (map[playerXPos + 1][playerYPos].character != NULL)
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
                                if (playerWeaponType == WeaponType::Flail)
                                {
                                    // Whip attack
                                    bool attacking = false;
                                    if (map[playerXPos][playerYPos - 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos - 1]);
                                    }
                                    if (playerXPos - 1 >= 0 && map[playerXPos - 1][playerYPos - 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos - 1][playerYPos - 1]);
                                    }
                                    if (playerXPos + 1 < 3 && map[playerXPos + 1][playerYPos - 1].character != NULL)
                                    {
                                        attacking = true;
                                        attackCharacter(&map[playerXPos][playerYPos], &map[playerXPos + 1][playerYPos - 1]);
                                    }

                                    if (attacking)
                                    {
                                        moveEnemies();
                                    }
                                    else if (!attacking && moveCharacter(&map[playerXPos][playerYPos], &map[playerXPos][playerYPos - 1]))
                                    {
                                        playerYPos--;
                                        moveEnemies();
                                        activateTile();
                                    }
                                }
                                else if (map[playerXPos][playerYPos - 1].character != NULL)
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
                            return false;
                    }
                    break;

                case SDL_QUIT:
                    return false;
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

        int health = player.health;
        for (; health > 0; health--)
        {
            SDL_Rect tileRect;
            tileRect.x = (health - 1) * 10;
            tileRect.y = 0;
            SDL_BlitSurface(heartImage, NULL, SDL_GetWindowSurface(window), &tileRect);           
        }

        // Draw the amount of gold
        SDL_Rect goldTextRect;
        goldTextRect.x = 0;
        goldTextRect.y = 12;
        char buffer[5];
        char goldText[16];
        strcpy(goldText, "Gold: ");
        strcat(goldText, itoa(playerGold, buffer, 10));
        SDL_Surface* goldTextSurface = TTF_RenderText_Solid(font, goldText, white);
        SDL_BlitSurface(goldTextSurface, NULL, SDL_GetWindowSurface(window), &goldTextRect);
        SDL_FreeSurface(goldTextSurface);

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
        SDL_FreeSurface(textSurface);

        SDL_UpdateWindowSurface(window);


        // Switch the music track halfway through
        if (playingSlowMusic && secondsSinceStart == 29)
        {
            SDL_Log("Changing the music.");
            Mix_Chunk* newMusic = Mix_LoadWAV("audio/Music1.wav");
            Mix_FreeChunk(currentMusic);
            Mix_PlayChannel(1, newMusic, 0);
            playingSlowMusic = false;
            SDL_Log("Successfully changed the music.");
        }

        // Sleep until the next frame (60 FPS)
        int timeSinceFrameStart = SDL_GetTicks() - startTimeLastFrame;
        if (timeSinceFrameStart < 16)
        {
            SDL_Delay(16 - timeSinceFrameStart);
        }
    } // End of the endless while loop
}

bool gameOverScreen()
{
    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                            return true;

                        case SDLK_ESCAPE:
                            return false;
                    }
                    break;

                case SDL_QUIT:
                    return false;
            }
        }

        SDL_BlitSurface(backgroundImage, NULL, SDL_GetWindowSurface(window), NULL);

        SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "Game Over!", white);
        SDL_Rect targetRect;
        targetRect.x = 50;
        targetRect.y = 40;
        SDL_BlitSurface(gameOverSurface, NULL, SDL_GetWindowSurface(window), &targetRect);
        SDL_FreeSurface(gameOverSurface);

        targetRect.x = 60;
        targetRect.y = 70;
        SDL_BlitSurface(goldPileImage, NULL, SDL_GetWindowSurface(window), &targetRect);

        char scoreText[4];
        char buffer[4];
        strcpy(scoreText, itoa(playerGold, buffer, 10));
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, white);
        targetRect.x = 100;
        targetRect.y = 90;
        SDL_BlitSurface(scoreSurface, NULL, SDL_GetWindowSurface(window), &targetRect);
        SDL_FreeSurface(scoreSurface);

        char highScoreText[100];
        strcpy(highScoreText, "High Score: ");
        strcat(highScoreText, itoa(highScore, buffer, 10));
        SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreText, white);
        targetRect.x = 30;
        targetRect.y = 110;
        SDL_BlitSurface(highScoreSurface, NULL, SDL_GetWindowSurface(window), &targetRect);
        SDL_FreeSurface(highScoreSurface);
        
        SDL_Surface* tryAgainSurface = TTF_RenderText_Solid(font, "Press Enter to retry.", white);
        targetRect.x = 5;
        targetRect.y = 140;
        SDL_BlitSurface(tryAgainSurface, NULL, SDL_GetWindowSurface(window), &targetRect);
        SDL_FreeSurface(tryAgainSurface);

        SDL_Surface* quitSurface = TTF_RenderText_Solid(font, "Press Escape to quit.", white);
        targetRect.x = 5;
        targetRect.y = 160;
        SDL_BlitSurface(quitSurface, NULL, SDL_GetWindowSurface(window), &targetRect);
        SDL_FreeSurface(quitSurface);

        SDL_UpdateWindowSurface(window);
    }

    SDL_Log("How'd we get out of an endless loop?");
    return false;
}

void drawText(char* text, int x, int y)
{
    SDL_Rect targetRect;
    targetRect.x = x;
    targetRect.y = y;
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, white);
    SDL_BlitSurface(textSurface, NULL, SDL_GetWindowSurface(window), &targetRect);
    SDL_FreeSurface(textSurface);
}

bool titleScreen()
{
    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                            return true;

                        case SDLK_ESCAPE:
                            return false;
                    }
                    break;

                case SDL_QUIT:
                    return false;
            }
        }

        drawText("Tiny Dungeon", 30, 5);
        drawText("Collect the gold", 20, 40);
        
        SDL_Rect exampleTileRect;
        exampleTileRect.x = 5;
        exampleTileRect.y = 60;
        SDL_BlitSurface(smallGoldTile, NULL, SDL_GetWindowSurface(window), &exampleTileRect);
        drawText("Gold + Enemy", 15, 58);

        exampleTileRect.y = 75;
        SDL_BlitSurface(smallUpgradeTile, NULL, SDL_GetWindowSurface(window), &exampleTileRect);
        drawText("New weapon", 15, 73);

        exampleTileRect.y = 90;
        SDL_BlitSurface(smallHealingTile, NULL, SDL_GetWindowSurface(window), &exampleTileRect);
        drawText("Health potion", 15, 88);

        exampleTileRect.y = 105;
        SDL_BlitSurface(smallBerserkTile, NULL, SDL_GetWindowSurface(window), &exampleTileRect);
        drawText("+Strength, -Defense", 15, 103);

        exampleTileRect.y = 120;
        SDL_BlitSurface(smallDefenseTile, NULL, SDL_GetWindowSurface(window), &exampleTileRect);
        drawText("Take less damage", 15, 118);

        exampleTileRect.y = 135;
        SDL_BlitSurface(smallNothingTile, NULL, SDL_GetWindowSurface(window), &exampleTileRect);
        drawText("Nothing", 15, 133);

        drawText("Press Enter to start.", 10, 160);

        SDL_UpdateWindowSurface(window);
    }
}

int main()
{
    srand(time(NULL));
    SDL_Log("Initializing SDL");
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    TTF_Init();
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Volume(1, 128);
    Mix_Volume(2, 64);
    SDL_Log("Finished initializing SDL");

    // TODO: Better title
    window = SDL_CreateWindow("Tiny Dungeon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              160, 192, 0);
    initTiles();
    SDL_SetWindowIcon(window, hero);  // TODO: Doesn't work.
    font = TTF_OpenFont("images/cour.ttf", 12);

    std::ifstream highScoreInput("HighScore.txt");
    if (highScoreInput.is_open())
    {
        highScoreInput >> highScore;
        highScoreInput.close();
    }

    bool startPlaying = titleScreen();
    if (startPlaying)
    {
        while (true)
        {
            player.defense = 0;
            player.health = 3;
            player.image = hero;
            player.isPlayer = true;
            player.strength = 1;
            playerGold = 0;
            playerWeaponImage = handheldSwordImage;
            playerWeaponType = WeaponType::Straight;
            playerXPos = 1;
            playerYPos = 1;

            bool completedGame = gameLoop();
            Mix_HaltChannel(1);
            if (!completedGame)
                break;

            bool playAgain = gameOverScreen();
            if (!playAgain)
                break;
        }
    }

    SDL_Log("Shutting down");
    std::ofstream highScoreOutput;
    highScoreOutput.open("HighScore.txt");
    if (highScoreOutput.is_open())
    {
        highScoreOutput << highScore;
        highScoreOutput.close();
    }

    Mix_CloseAudio();
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