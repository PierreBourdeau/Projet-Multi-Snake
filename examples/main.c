/*******************************************************************************************
*
*   raylib - sample game: snake (Multiplayer mode)
*
*   Sample game originally developed by Ian Eito, Albert Martos and Ramon Santamaria
* 
*   Modified by : Pierre Bourdeau adding features such as : Multiplayer mode, Menus, Options, Lives, Wall generation, Cross wall, Speed Increase, GameModes...
*
*   This game has been created using raylib v1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*   
********************************************************************************************/

#include <stdio.h>
#include "raylib.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SNAKE_LENGTH   256
#define SQUARE_SIZE     31
#define WALL_NBR 10 //Number of wall generated on the game board
//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Snake {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
    unsigned int lives;
    int counterTail;
    Vector2 snakePosition[SNAKE_LENGTH];
} Snake;

typedef struct FoodOrWall {
    Vector2 position;
    Vector2 size;
    bool active;
    Color color;
    bool isWall; //Added field to specify if an entity is dangerous or not
} FoodOrWall;

typedef struct multiplayers {
    unsigned int nbrOfPlayer;
    Snake* snakes[SNAKE_LENGTH];
} MultiP;
//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 1280;
static const int screenHeight =720;
unsigned int gameFps = 60;
static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;
static FoodOrWall fruit = { 0 };
static FoodOrWall wall[WALL_NBR] = { 0 };
static bool allowMove = false;
static Vector2 offset = { 0 };
static Snake player1[SNAKE_LENGTH] = { 0 };
static Snake player2[SNAKE_LENGTH] = { 0 };
static unsigned int gameMode = 0;
MultiP players = { 0 };
static bool gotLives; // Variable corresponding to the number of lives
static unsigned int menuSelector = 0; //Variable to navigate into options menu
static bool options = false; //Variable that determine the active state of option page
static bool crosswall = false; //Variable to allow or disable cross wall game rule
static bool multiplayer = true; // Variable to set multiplayer mode on or off
static bool menu = true; //Variable that determine the active state of the menu
//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);                  // Initialize game
static void UpdateGame(void);                // Update game (one frame)
static void DrawGame(void);                  // Draw game (one frame)
static void UnloadGame(void);                // Unload game
static void UpdateDrawFrame(void);           // Update and Draw (one frame)
static void WallGeneration(void);            //Randomized wall generation & wall collision
static void SpeedIncrease(void);             //In game mode 2, increase game fps of +10 every 10 fruits eaten
static void EndOfTheGame(Snake *aSnake);     //Check for the end of the game and manages the player lives
static void CrossWall(Snake* aSnake);        //Permit to cross the map from right to left, from top to bottom and vice-versa
//------------------------------------------------------------------------------------
// Score storage variables declaration
//------------------------------------------------------------------------------------
typedef enum {
    STORAGE_POSITION_SCORE = 0,
    STORAGE_POSITION_HISCORE = 1,
    STORAGE_POSITION_SCORE2 = 3
} StorageData;
static int score; //score of the last run (player 1)
static int score2; //score of the last run (player 2)
static int hiscore; //highest score

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    //Setting default data values of scores to 0 in the file storage.data (1st start of the game)
    //---------------------------------------------------------
    if (LoadStorageValue(STORAGE_POSITION_SCORE) == NULL) {
        SaveStorageValue(STORAGE_POSITION_HISCORE, 0);
        SaveStorageValue(STORAGE_POSITION_SCORE, 0);
        SaveStorageValue(STORAGE_POSITION_SCORE2, 0);
    }

    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth , screenHeight + 100, "SNAKE V.2");

    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

//Game Options window display and control
//---------------------------------------------------------
void gOptions(void) {
    ClearBackground(RAYWHITE);
    DrawText("OPTIONS", GetScreenWidth() / 2 - MeasureText("OPTIONS", 20) / 2, GetScreenHeight() / 2 - 100, 20, GRAY);
    DrawText("[ENTER] TO SAVE", GetScreenWidth() / 2 - MeasureText("[ENTER] TO SAVE", 20) / 2, GetScreenHeight() - 100, 20, LIGHTGRAY);
    //Navigate through the menu 
    if (IsKeyPressed(KEY_UP) && menuSelector != 0) menuSelector--;
    else if (IsKeyPressed(KEY_DOWN) && menuSelector != 3) menuSelector++;
    else if (IsKeyPressed(KEY_UP) && menuSelector == 0) menuSelector = 3;
    else if (IsKeyPressed(KEY_DOWN) && menuSelector == 3) menuSelector = 0;
    else if (IsKeyPressed(KEY_ENTER)) options = false;
    //control options : game-mode (difficulty)
    if (menuSelector == 0)
    {
        DrawText(TextFormat("> SELECT DIFFICULTY ([1], [2] or [3]) : %i", gameMode + 1), GetScreenWidth() / 2 - MeasureText("SELECT DIFFICULTY ([1], [2] or [3]) : 3", 20) / 2, GetScreenHeight() / 2 - 50, 20, RED);
        DrawText(TextFormat("MULTIPLE LIVES : %i", players.snakes[0]->lives), GetScreenWidth() / 2 - MeasureText("MULTIPLE LIVES : 3", 20) / 2, GetScreenHeight() / 2, 20, GRAY);
        DrawText(TextFormat("CROSS WALLS : %i", crosswall), GetScreenWidth() / 2 - MeasureText("CROSS WALLS : 1", 20) / 2, GetScreenHeight() / 2 + 50, 20, GRAY);
        DrawText(TextFormat("MULTIPLAYER : %i", multiplayer), GetScreenWidth() / 2 - MeasureText("MULTIPLAYER : 1", 20) / 2, GetScreenHeight() / 2 + 100, 20, GRAY);
        if (IsKeyPressed('1'))
        {
            gameMode = 0;
        }
        else if (IsKeyPressed('2'))
        {
            gameMode = 1;
        }
        else if (IsKeyPressed('3'))
        {
            gameMode = 2;
        }
    }
    //control options : enable/disable multiple lives
    else if (menuSelector == 1) {
        DrawText(TextFormat("SELECT DIFFICULTY (1, 2 or 3) : %i", gameMode + 1), GetScreenWidth() / 2 - MeasureText("SELECT DIFFICULTY 1 - 2 - 3 : 3", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);
        DrawText(TextFormat("> MULTIPLE LIVES [Y/N] : %i", players.snakes[0]->lives), GetScreenWidth() / 2 - MeasureText("MULTIPLE LIVES [Y/N] : 3", 20) / 2, GetScreenHeight() / 2, 20, RED);
        DrawText(TextFormat("CROSS WALLS : %i", crosswall), GetScreenWidth() / 2 - MeasureText("CROSS WALLS : 1", 20) / 2, GetScreenHeight() / 2 + 50, 20, GRAY);
        DrawText(TextFormat("MULTIPLAYER : %i", multiplayer), GetScreenWidth() / 2 - MeasureText("MULTIPLAYER : 1", 20) / 2, GetScreenHeight() / 2 + 100, 20, GRAY);
        if (IsKeyPressed('Y'))
        {
            players.snakes[0]->lives = 3;
            if (players.nbrOfPlayer == 2) players.snakes[1]->lives = 3;
            gotLives = true;
        }
        else if (IsKeyPressed('N'))
        {
            players.snakes[0]->lives = 1;
            if (players.nbrOfPlayer == 2) players.snakes[1]->lives = 1;
            gotLives = false;
        }
    }
    //control options : enable/disable cross wall
    else if (menuSelector == 2)
    {
        DrawText(TextFormat("SELECT DIFFICULTY (1, 2 or 3) : %i", gameMode + 1), GetScreenWidth() / 2 - MeasureText("SELECT DIFFICULTY 1 - 2 - 3 : 3", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);
        DrawText(TextFormat("MULTIPLE LIVES : %i", players.snakes[0]->lives), GetScreenWidth() / 2 - MeasureText("MULTIPLE LIVES : 3", 20) / 2, GetScreenHeight() / 2, 20, GRAY);
        DrawText(TextFormat("> CROSS WALLS [Y/N] : %i", crosswall), GetScreenWidth() / 2 - MeasureText("CROSS WALLS [Y/N] : 1", 20) / 2, GetScreenHeight() / 2 + 50, 20, RED);
        DrawText(TextFormat("MULTIPLAYER : %i", multiplayer), GetScreenWidth() / 2 - MeasureText("MULTIPLAYER : 1", 20) / 2, GetScreenHeight() / 2 + 100, 20, GRAY);
        if (IsKeyPressed('Y'))
        {
            crosswall = true;
        }
        else if (IsKeyPressed('N')) {
            crosswall = false;
        }
    }
    //control options : enable/disable multiplayer mode
    else if (menuSelector == 3)
    {
        DrawText(TextFormat("SELECT DIFFICULTY (1, 2 or 3) : %i", gameMode + 1), GetScreenWidth() / 2 - MeasureText("SELECT DIFFICULTY 1 - 2 - 3 : 3", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);
        DrawText(TextFormat("MULTIPLE LIVES : %i", players.snakes[0]->lives), GetScreenWidth() / 2 - MeasureText("MULTIPLE LIVES : 3", 20) / 2, GetScreenHeight() / 2, 20, GRAY);
        DrawText(TextFormat("CROSS WALLS : %i", crosswall), GetScreenWidth() / 2 - MeasureText("CROSS WALLS : 1", 20) / 2, GetScreenHeight() / 2 + 50, 20, GRAY);
        DrawText(TextFormat("> MULTIPLAYER [Y/N] : %i", multiplayer), GetScreenWidth() / 2 - MeasureText("MULTIPLAYER [Y/N] : 1", 20) / 2, GetScreenHeight() / 2 + 100, 20, RED);
        if (IsKeyPressed('Y'))
        {
            multiplayer = true;
        }
        else if (IsKeyPressed('N')) {
            multiplayer = false;
        }
    }
}
//Permit to cross the map from right to left, from top to bottom and vice-versa
//---------------------------------------------------------
void CrossWall(Snake * aSnake) {
    if (aSnake[0].speed.x == SQUARE_SIZE && aSnake[0].speed.y == 0)
    {
        aSnake[0].position.x = offset.x / 2;
    }
    else if (aSnake[0].speed.x == -SQUARE_SIZE && aSnake[0].speed.y == 0)
    {
        aSnake[0].position.x = (screenWidth - offset.x / 2 - SQUARE_SIZE);
    }
    else if (aSnake[0].speed.x == 0 && aSnake[0].speed.y == -SQUARE_SIZE)
    {
        aSnake[0].position.y = (screenHeight - offset.y / 2 - SQUARE_SIZE);
    }
    else if (aSnake[0].speed.x == 0 && aSnake[0].speed.y == SQUARE_SIZE)
    {
        aSnake[0].position.y = offset.y / 2;
    }
}
//Checking for the end of the game and manages the player lives
//---------------------------------------------------------
void EndOfTheGame(Snake* aSnake) {
    aSnake->lives--;
    if (aSnake->lives == 0)
    {
        gameOver = true;
        //Scoring storage (/!\ at the end of each game to minimize file reading)
        SaveStorageValue(STORAGE_POSITION_SCORE, players.snakes[0]->counterTail);
        if (multiplayer) SaveStorageValue(STORAGE_POSITION_SCORE2, players.snakes[1]->counterTail);
        if (players.snakes[0]->counterTail > hiscore)
           SaveStorageValue(STORAGE_POSITION_HISCORE, players.snakes[0]->counterTail);
        else if (multiplayer && players.snakes[1]->counterTail > hiscore)
            SaveStorageValue(STORAGE_POSITION_HISCORE, players.snakes[1]->counterTail);
    }
    else
    {
        if (!multiplayer) {
            aSnake->position = fruit.position; //if the player have other lifes, he restarts on the last fruit present on the map (only in Solo mode)
            aSnake->counterTail--;
        }
        else if (multiplayer) { //if the player have other lifes, he restarts at the spawning point (only in multiplayers)
            if (aSnake = players.snakes[0]) { 
                players.snakes[0]->position = (Vector2){ offset.x / 2, offset.y / 2 }; 
                players.snakes[0]->speed = (Vector2){ SQUARE_SIZE, 0 };
            }
            if (aSnake = players.snakes[1]) {
                players.snakes[1][0].position = (Vector2){ screenWidth - offset.x / 2 - SQUARE_SIZE, screenHeight - offset.y / 2 - SQUARE_SIZE };
                players.snakes[1][0].speed = (Vector2){ -SQUARE_SIZE, 0 };
            }
        }
    }
}

//Randomized wall generation & wall collision
//---------------------------------------------------------
void WallGeneration(void) {
    //Generate walls if not already active
    if (!wall->active)
    {
        wall->active = true;
        for (int y = 0; y < WALL_NBR; y++) {
            wall[y].position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
            for (int k = 0; k < players.nbrOfPlayer; k++) { // loop on nbrOfPlayer
                for (int l = 0; l < players.snakes[k]->counterTail; l++) { // loop on both snakes length
                    if ((wall[y].position.x == players.snakes[k][l].position.x) && (wall[y].position.y == players.snakes[k][l].position.y))
                    {
                        wall[y].position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
                        y = y - 1;
                    }
                }
            }
        }
    }
    // Collision between snake and walls
    for (int i = 0; i < WALL_NBR; i++) {
        for (int j = 0; j < players.nbrOfPlayer; j++) {
            if ((players.snakes[j]->position.x < (wall[i].position.x + wall->size.x) && (players.snakes[j]->position.x + players.snakes[j]->size.x) > wall[i].position.x) &&
                (players.snakes[j]->position.y < (wall[i].position.y + wall->size.y) && (players.snakes[j]->position.y + players.snakes[j]->size.y) > wall[i].position.y))
            {
                EndOfTheGame(players.snakes[j]);
            }
        }
    }
}

//Speed increase difficulty W*I*P
//---------------------------------------------------------
void SpeedIncrease(void) {
    if (gameFps <= 100) {
        gameFps += 10;
        SetTargetFPS(gameFps);
    }
}

// Initialize game variables
//---------------------------------------------------------
void InitGame(void)
{
    framesCounter = 0;
    gameOver = false;
    pause = false;
    if (multiplayer)players.nbrOfPlayer = 2;
    else players.nbrOfPlayer = 1;
    players.snakes[0] = player1;
    if (players.nbrOfPlayer == 2) {
        players.snakes[1] = player2;
    }
    allowMove = false;
    score = LoadStorageValue(STORAGE_POSITION_SCORE);
    score2 = LoadStorageValue(STORAGE_POSITION_SCORE2);
    hiscore = LoadStorageValue(STORAGE_POSITION_HISCORE);
    offset.x = screenWidth % SQUARE_SIZE;
    offset.y = screenHeight % SQUARE_SIZE;

    for (int i = 0; i < players.nbrOfPlayer; i++) {
        players.snakes[i]->counterTail = 1;
        if (gotLives)players.snakes[i]->lives = 3;
        else players.snakes[i]->lives = 1;
        for (int j = 0; j < SNAKE_LENGTH; j++)
        {
            players.snakes[i][j].size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
            if (i == 0 && j == 0) // player 1 snake head
            {
                players.snakes[i][j].color = DARKBLUE;
            }
            if (i == 0) // player 1 snake
            {
                players.snakes[i][j].color = BLUE;
                players.snakes[i][j].position = (Vector2){offset.x / 2, offset.y / 2 };
                players.snakes[i][j].speed = (Vector2){ SQUARE_SIZE, 0 };
            }
            if (i == 1 && j == 0) // player 2 snake head
            {
                players.snakes[i][j].color = ORANGE;
            }
            if (i == 1) // player 2 snake
            {
                players.snakes[i][j].color = GOLD;
                players.snakes[i][j].position = (Vector2){ screenWidth - offset.x / 2 - SQUARE_SIZE, screenHeight - offset.y / 2 - SQUARE_SIZE };
                players.snakes[i][j].speed = (Vector2){ -SQUARE_SIZE, 0 };
            }
        }
    }

    for (int n = 0; n < players.nbrOfPlayer; n++) {
        for (int i = 0; i < SNAKE_LENGTH; i++)
        {
            players.snakes[n]->snakePosition[i] = (Vector2){ 0.0f, 0.0f };;
        }
    }

    fruit.size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
    fruit.color = SKYBLUE;
    fruit.active = false;

    if (gameMode != 0) {
        for (int i = 0; i < WALL_NBR; i++) {
            wall[i].isWall = true;
            wall[i].size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
            wall[i].color = BEIGE;
            wall[i].active = false;
        }
    }
}

// Update game (one frame)
//---------------------------------------------------------
void UpdateGame(void)
{
    if (!gameOver && !menu)
    {
        if(framesCounter == 0) SetTargetFPS(gameFps);
        if (IsKeyPressed('P')) pause = !pause;

        if (pause && IsKeyPressed('E')) {
            menu = true;
        }
        if (!pause)
        {
            // Player 1 control
            if (IsKeyPressed(KEY_RIGHT) && (players.snakes[0][0].speed.x == 0) && allowMove)
            {
                players.snakes[0][0].speed = (Vector2){ SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT) && (players.snakes[0][0].speed.x == 0) && allowMove)
            {
                players.snakes[0][0].speed = (Vector2){ -SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_UP) && (players.snakes[0][0].speed.y == 0) && allowMove)
            {
                players.snakes[0][0].speed = (Vector2){ 0, -SQUARE_SIZE };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN) && (players.snakes[0][0].speed.y == 0) && allowMove)
            {
                players.snakes[0][0].speed = (Vector2){ 0, SQUARE_SIZE };
                allowMove = false;
            }
            //Player 2 control
            if (multiplayer)
            {
                if (IsKeyPressed('G') && (players.snakes[1][0].speed.x == 0) && allowMove)
                {
                    players.snakes[1][0].speed = (Vector2){ SQUARE_SIZE, 0 };
                    allowMove = false;
                }
                if (IsKeyPressed('D') && (players.snakes[1][0].speed.x == 0) && allowMove)
                {
                    players.snakes[1][0].speed = (Vector2){ -SQUARE_SIZE, 0 };
                    allowMove = false;
                }
                if (IsKeyPressed('R') && (players.snakes[1][0].speed.y == 0) && allowMove)
                {
                    players.snakes[1][0].speed = (Vector2){ 0, -SQUARE_SIZE };
                    allowMove = false;
                }
                if (IsKeyPressed('F') && (players.snakes[1][0].speed.y == 0) && allowMove)
                {
                    players.snakes[1][0].speed = (Vector2){ 0, SQUARE_SIZE };
                    allowMove = false;
                }
            }
            // Snake movement
            for (int n = 0; n < players.nbrOfPlayer; n++)
            {
                for (int i = 0; i < players.snakes[n]->counterTail; i++)
                {
                    players.snakes[n]->snakePosition[i] = players.snakes[n][i].position;
                }
            }
            if ((framesCounter % 5) == 0)
            {
                for (int n = 0; n < players.nbrOfPlayer; n++) {
                    for (int i = 0; i < players.snakes[n]->counterTail; i++)
                    {
                        if (i == 0)
                        {
                            players.snakes[n][0].position.x += players.snakes[n][0].speed.x;
                            players.snakes[n][0].position.y += players.snakes[n][0].speed.y;
                            allowMove = true;
                        }
                        else players.snakes[n][i].position = players.snakes[n]->snakePosition[i - 1];
                    }
                }
            }
            // Wall (board) behaviour
            for (int n = 0; n < players.nbrOfPlayer; n++) {
                if (((players.snakes[n][0].position.x) > (screenWidth - offset.x)) ||
                    ((players.snakes[n][0].position.y) > (screenHeight - offset.y)) ||
                    (players.snakes[n][0].position.x < 0) || (players.snakes[n][0].position.y < 0))
                {
                    if (crosswall) CrossWall(players.snakes[n]);
                    else EndOfTheGame(players.snakes[n]);
                }
            }
            // Collision with yourself or with other snake
            for (int n = 0; n < players.nbrOfPlayer; n++)
            {
                for (int i = 1; i < players.snakes[n]->counterTail; i++)
                {
                    if ((players.snakes[n][0].position.x == players.snakes[n][i].position.x) && (players.snakes[n][0].position.y == players.snakes[n][i].position.y)) EndOfTheGame(players.snakes[n]);
                }
                if (players.nbrOfPlayer == 2) {
                    for (int j = 0; j < players.snakes[n]->counterTail; j++)
                    {
                        if ((players.snakes[0][0].position.x == players.snakes[1][j].position.x) && (players.snakes[0][0].position.y == players.snakes[1][j].position.y)) EndOfTheGame(players.snakes[0]);
                        else if ((players.snakes[1][0].position.x == players.snakes[0][j].position.x) && (players.snakes[1][0].position.y == players.snakes[0][j].position.y)) EndOfTheGame(players.snakes[1]);
                    }
                }
            }
            // Fruit position calculation
            if (!fruit.active)
            {
                fruit.active = true;
                fruit.position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
                for (int i = 0; i < WALL_NBR; i++) {
                    if ((fruit.position.x == wall[i].position.x) && (fruit.position.y == wall[i].position.y)) {
                        fruit.position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
                        i = 0;
                        i = i - 1;
                    }
                }
                for (int n = 0; n < players.nbrOfPlayer; n++)
                {
                    for (int i = 0; i < players.snakes[n]->counterTail; i++)
                    {
                        while ((fruit.position.x == players.snakes[n][i].position.x) && (fruit.position.y == players.snakes[n][i].position.y))
                        {
                            fruit.position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
                            i = 0;
                        }
                    }
                }
            }
            // Collision with fruit
            for (int n = 0; n < players.nbrOfPlayer; n++)
            {
                if ((players.snakes[n][0].position.x < (fruit.position.x + fruit.size.x) && (players.snakes[n][0].position.x + players.snakes[n][0].size.x) > fruit.position.x) &&
                    (players.snakes[n][0].position.y < (fruit.position.y + fruit.size.y) && (players.snakes[n][0].position.y + players.snakes[n][0].size.y) > fruit.position.y))
                {
                    players.snakes[n][players.snakes[n]->counterTail].position = players.snakes[n]->snakePosition[players.snakes[n]->counterTail - 1];
                    players.snakes[n]->counterTail += 1;
                    fruit.active = false;
                    //If playing in gameMode 2, generate new wall configuration each 10 fruits eaten
                    if (gameMode == 2 && players.snakes[0]->counterTail % 10 == 0) {
                        wall->active = false;
                        WallGeneration();
                        SpeedIncrease();
                    }
                }
            }
            //GameMode Features : wall generation
            if (gameMode == 1 || gameMode == 2)
                WallGeneration();

            framesCounter++;
        }
    }
    //GameOver page controls
    else if (!menu && gameOver)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
        }
        else if (IsKeyPressed('E')) {
            InitGame();
            menu = true;
        }
    }
    //Menu controls
    else
    {
        if (IsKeyPressed(KEY_O)) {
            options = true;
        }
        else if (IsKeyPressed(KEY_SPACE)) {
            InitGame();
            menu = false;
        }
    }
}

// Draw game (one frame)
//---------------------------------------------------------
void DrawGame(void)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (!gameOver && !menu && !options)
    {
        // Draw grid lines
        for (int i = 0; i < screenWidth / SQUARE_SIZE + 1; i++)
        {
            DrawLineV((Vector2) { SQUARE_SIZE* i + offset.x / 2, offset.y / 2 }, (Vector2) { SQUARE_SIZE* i + offset.x / 2, screenHeight - offset.y / 2 }, LIGHTGRAY);
        }

        for (int i = 0; i < screenHeight / SQUARE_SIZE + 1; i++)
        {
            DrawLineV((Vector2) { offset.x / 2, SQUARE_SIZE* i + offset.y / 2 }, (Vector2) { screenWidth - offset.x / 2, SQUARE_SIZE* i + offset.y / 2 }, LIGHTGRAY);
        }

        //Drawn walls
        for (int i = 0; i < WALL_NBR; i++) DrawRectangleV(wall[i].position, wall[i].size, wall->color);

        // Draw snake
        for (int n = 0; n < players.nbrOfPlayer; n++)
        {
            for (int i = 0; i < players.snakes[n]->counterTail; i++)
            {
                DrawRectangleV(players.snakes[n][i].position, players.snakes[n][i].size, players.snakes[n][i].color);
            }
        }

        // Draw fruit to pick
        DrawRectangleV(fruit.position, fruit.size, fruit.color);

        //Draw score
        DrawText(TextFormat("Score Player 1 : %i", players.snakes[0]->counterTail), 10, (GetScreenHeight() - 80), 25, BLUE);
        DrawText(TextFormat("Player 1 : %i lives", players.snakes[0]->lives), 10, (GetScreenHeight() - 45), 25, MAROON);
        DrawText(TextFormat("HISCORE : %i", hiscore), GetScreenWidth() / 2 - MeasureText("HISCORE : %i", 30) / 2, (GetScreenHeight() - 80), 30, GRAY);
        if (players.nbrOfPlayer == 2)
        {
            DrawText(TextFormat("Score Player 2 : %i", players.snakes[1]->counterTail), GetScreenWidth() - MeasureText("Score Player 2 : %i", 25) - 10, (GetScreenHeight() - 80), 25, ORANGE);
            DrawText(TextFormat("Player 1 : %i lives", players.snakes[1]->lives), GetScreenWidth() - MeasureText("Score Player 2 : %i", 25) - 10, (GetScreenHeight() - 45), 25, MAROON);
        }
        //Draw pause during game
        if (pause)
        {
            DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
            DrawText("PRESS [E] FOR MENU", screenWidth / 2 - MeasureText("PRESS [E] FOR MENU", 40) / 2, screenHeight / 2, 40, RED);
        }
    }
    //Draw gameOver page
    else if (!menu && gameOver && !options) {
        DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);
        DrawText("PRESS [E] FOR MENU", GetScreenWidth() / 2 - MeasureText("PRESS [E] FOR MENU", 20) / 2, GetScreenHeight() / 2, 20, RED);
    }
    //Draw menu
    else if (menu && !options)
    {
        DrawText("MENU", GetScreenWidth() / 2 - MeasureText("MENU", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);
        DrawText("[O] - OPTIONS", GetScreenWidth() / 2 - MeasureText("[O] - OPTIONS", 20) / 2, GetScreenHeight() / 2, 20, RED);
        DrawText(TextFormat("Last scores : Player-1 : %i || Player-2 : %i", score, score2), GetScreenWidth() / 2 - MeasureText("Last score : Player-1 10 || Player-2 : 20", 20) / 2, GetScreenHeight() / 2 + 50, 20, ORANGE);
        DrawText(TextFormat("HISCORE : %i", hiscore), GetScreenWidth() / 2 - MeasureText("HISCORE : 20", 20) / 2, GetScreenHeight() / 2 + 100, 20, MAROON);
        DrawText("[SPACE] TO START", GetScreenWidth() / 2 - MeasureText("[SPACE] TO START", 20) / 2, GetScreenHeight() / 2 + 150, 20, LIGHTGRAY);
        DrawText("PRESS [ESC] TO LEAVE", GetScreenWidth() / 2 - MeasureText("PRESS [ESC] TO LEAVE", 20) / 2, GetScreenHeight() - 50, 20, DARKGRAY);
    }
    else if (options)
    {
        gOptions();
    }
    EndDrawing();
}

// Unload game variables
//---------------------------------------------------------
void UnloadGame(void)
{

}

// Update and Draw (one frame)
//---------------------------------------------------------
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}
