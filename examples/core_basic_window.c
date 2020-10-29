/*******************************************************************************************
*
*   raylib - sample game: snake
*
*   Sample game developed by Ian Eito, Albert Martos and Ramon Santamaria
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
#define WALL_NBR 10

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Snake {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct FoodOrWall {
    Vector2 position;
    Vector2 size;
    bool active;
    Color color;
    bool isWall;
} FoodOrWall;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 1600;
static const int screenHeight = 900;
unsigned int gameFps = 60;
static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;
static bool menu = true;
static FoodOrWall fruit = { 0 };
static FoodOrWall wall[WALL_NBR] = { 0 };
static Snake snake[SNAKE_LENGTH] = { 0 };
static Vector2 snakePosition[SNAKE_LENGTH] = { 0 };
static bool allowMove = false;
static Vector2 offset = { 0 };
static int counterTail = 0;
static unsigned int gameMode = 0;
unsigned int lives;
//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)
// SCORES 
typedef enum {
    STORAGE_POSITION_SCORE = 0,
    STORAGE_POSITION_HISCORE = 1
} StorageData;
static int score;
static int hiscore;
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)

{
    //Setting default data values of scores to 0 in the file storage.data (1st start of the game)
    if (LoadStorageValue(STORAGE_POSITION_SCORE) == NULL) {
        SaveStorageValue(STORAGE_POSITION_HISCORE, 0);
        SaveStorageValue(STORAGE_POSITION_SCORE, 0);
    }
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "SNAKE V.2");

    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(gameFps);
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

//Checking for the end of the game and manages the player lives
void EndOfTheGame(void) {
    lives--;
    Vector2 direction = snake[0].speed;
    int temp = 0;
    if (lives == 0)
    {
        //Scoring storage
        SaveStorageValue(STORAGE_POSITION_SCORE, counterTail);
        if (counterTail > hiscore) {
            SaveStorageValue(STORAGE_POSITION_HISCORE, counterTail);
        }
        gameOver = true;
    }
    else
    {
        snake[0].position = fruit.position;
    }
}

//Randomized wall generation 
void WallGeneration(void) {
    //Generate walls if not already active
    if (!wall->active)
    {
        wall->active = true;
        for (int y = 0; y < WALL_NBR; y++) {
            wall[y].position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
            if ((wall[y].position.x == snake[y].position.x) && (wall[y].position.y == snake[y].position.y))
            {
                wall[y].position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
                y = y-1;
            }
        }
    }
    // Collision between snake and walls
    for (int i =0; i<WALL_NBR; i++) {
        if ((snake[0].position.x < (wall[i].position.x + wall->size.x) && (snake[0].position.x + snake[0].size.x) > wall[i].position.x) &&
            (snake[0].position.y < (wall[i].position.y + wall->size.y) && (snake[0].position.y + snake[0].size.y) > wall[i].position.y))
        {
            EndOfTheGame();
        }
    }
}

//Speed increase difficulty
void SpeedIncrease(void) {
    if (gameFps <= 120) {
        gameFps == gameFps + 20;
        SetTargetFPS(gameFps);
    }
}



// Initialize game variables
void InitGame(void)
{
    framesCounter = 0;
    gameOver = false;
    pause = false;

    counterTail = 1;
    allowMove = false;
    score = LoadStorageValue(STORAGE_POSITION_SCORE);
    hiscore = LoadStorageValue(STORAGE_POSITION_HISCORE);
    offset.x = screenWidth % SQUARE_SIZE;
    offset.y = screenHeight % SQUARE_SIZE;
    lives = 3;
    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){ offset.x / 2, offset.y / 2 };
        snake[i].size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
        snake[i].speed = (Vector2){ SQUARE_SIZE, 0 };

        if (i == 0) snake[i].color = DARKBLUE;
        else snake[i].color = BLUE;
    }

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
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
void UpdateGame(void)
{
    if (!gameOver && !menu)
    {
        if (IsKeyPressed('P')) pause = !pause;
        
        if ( pause && IsKeyPressed('E')) {
            menu = true;
        }
        if (!pause)
        {
           // WallGeneration();

            // Player control
            if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ -SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, -SQUARE_SIZE };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, SQUARE_SIZE };
                allowMove = false;
            }

            // Snake movement
            for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].position;

            if ((framesCounter % 5) == 0)
            {
                for (int i = 0; i < counterTail; i++)
                {
                    if (i == 0)
                    {
                        snake[0].position.x += snake[0].speed.x;
                        snake[0].position.y += snake[0].speed.y;
                        allowMove = true;
                    }
                    else snake[i].position = snakePosition[i - 1];
                }
            }

            // Wall behaviour
            if (((snake[0].position.x) > (screenWidth - offset.x)) ||
                ((snake[0].position.y) > (screenHeight - offset.y)) ||
                (snake[0].position.x < 0) || (snake[0].position.y < 0))
            {
                EndOfTheGame();
            }

            // Collision with yourself
            for (int i = 1; i < counterTail; i++)
            {
                if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y)) gameOver = true;
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
                for (int i = 0; i < counterTail; i++)
                {
                    while ((fruit.position.x == snake[i].position.x) && (fruit.position.y == snake[i].position.y))
                    {
                        fruit.position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
                        i = 0;
                    }
                }
            }

            // Collision
            if ((snake[0].position.x < (fruit.position.x + fruit.size.x) && (snake[0].position.x + snake[0].size.x) > fruit.position.x) &&
                (snake[0].position.y < (fruit.position.y + fruit.size.y) && (snake[0].position.y + snake[0].size.y) > fruit.position.y))
            {
                snake[counterTail].position = snakePosition[counterTail - 1];
                counterTail += 1;
                fruit.active = false;
                if (gameMode == 2 && counterTail % 10 == 0) {
                    wall->active = false;
                    WallGeneration();
                    SpeedIncrease();
                }
            }
            //GameMode Features
            if (gameMode == 1 || gameMode ==2)
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
            gameOver = false;
        }
        else if (IsKeyPressed('E')) {
            InitGame();
            menu = true;
        }
    }
    //Menu controls
    else
    {
        if (IsKeyPressed('1'))
        {
            gameMode = 0;
            InitGame();
            menu = false;
        }
        else if (IsKeyPressed('2'))
        {
            gameMode = 1;
            InitGame();
            menu = false;
        }
        else if (IsKeyPressed('3'))
        {
            gameMode = 2;
            InitGame();
            menu = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (!gameOver && !menu)
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
        for (int i =0; i< WALL_NBR; i++) DrawRectangleV(wall[i].position, wall[i].size, wall->color);

        // Draw snake
        for (int i = 0; i < counterTail; i++) DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);

        // Draw fruit to pick
        DrawRectangleV(fruit.position, fruit.size, fruit.color);

        //Draw score
        DrawText(TextFormat("Score : %i", counterTail), 10, (GetScreenHeight() - 20), 20, BLUE);

        //Draw pause during game
        if (pause) 
        {
            DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
            DrawText("PRESS [E] FOR MENU", screenWidth / 2 - MeasureText("PRESS [E] FOR MENU", 40) / 2, screenHeight / 2, 40, RED);
        }
    }
    //Draw gameOver page
    else if (!menu && gameOver) {
        DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);
        DrawText("PRESS [E] FOR MENU", GetScreenWidth() / 2 - MeasureText("PRESS [E] FOR MENU", 20) / 2, GetScreenHeight() / 2, 20, RED);
    }
    //Draw menu
    else if (menu)
    {
        DrawText("MENU", GetScreenWidth() / 2 - MeasureText("MENU", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);
        DrawText("SELECT DIFFICULTY 1 - 2 - 3", GetScreenWidth() / 2 - MeasureText("SELECT DIFFICULTY 1 - 2 - 3", 20) / 2, GetScreenHeight() / 2, 20, RED);
        DrawText(TextFormat("Last score : %i || Best score : %i", score, hiscore), GetScreenWidth()/2 - MeasureText("Last score : 10 || Best score : 20", 20) /2 , GetScreenHeight() /2+50, 20,ORANGE);
        DrawText("PRESS [ESC] TO LEAVE", GetScreenWidth() / 2 - MeasureText("PRESS [ESC] TO LEAVE", 20) / 2, GetScreenHeight()- 50, 20 , DARKGRAY);
    }
    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}