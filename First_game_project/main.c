#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

#include "player.h"
#include "enemy.h"
#include "room.h"
#include "ui.h"
#include "game.h"
#include "globals.h"

#define START_SCR_WIDTH 800
#define START_SCR_HEIGHT 600

int main() {
    int SCR_WIDTH = START_SCR_WIDTH;
    int SCR_HEIGHT = START_SCR_HEIGHT;
    bool shouldExit = false;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(START_SCR_WIDTH, START_SCR_HEIGHT, "Dungeon Slayer");
    SetExitKey(KEY_NULL);
    SetWindowMinSize(START_SCR_WIDTH, START_SCR_HEIGHT);

    Image icon = LoadImage("./Textures/Icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    InitAudioDevice();

    Sound menuButton = LoadSound("./Sounds/menuButton.WAV");
    Sound playerAttack = LoadSound("./Sounds/playerAttack.WAV");
    Sound playerWalk = LoadSound("./Sounds/playerWalk.WAV");
    Sound cleared = LoadSound("./Sounds/cleared.WAV");

    Player player1, player2;
    int p1SpawnX = 150;
    int p1SpawnY = 400;
    int p2SpawnX = 250;
    int p2SpawnY = 400;

    InitPlayer(&player1, (Vector2){p1SpawnX, p1SpawnY}, "./Textures/Player_1.png", "./Textures/Weapon_1.png");
    InitPlayer(&player2, (Vector2){p2SpawnX, p2SpawnY}, "./Textures/Player_2.png", "./Textures/Weapon_2.png");

    GameArea game;
    Room rooms[NUM_ROOM];
    InitRoom(rooms);
    int currentRoomIndex = 0;
    Room *currentRoom = &rooms[currentRoomIndex];

    Enemy enemies[MAX_ENEMIES];
    InitEnemies(enemies, currentRoom);

    GameMode mode = GAME_MENU;
    GameMode returnMode = GAME_MENU;

    GameSettings settings = {.masterVolume = 0.5f, .targetFPS = 60};
    SetTargetFPS(settings.targetFPS);

    bool pauseHintShown = false;

    while(!WindowShouldClose() && !shouldExit) {
        SCR_WIDTH = GetScreenWidth();
        SCR_HEIGHT = GetScreenHeight();
        float dt = GetFrameTime();

        if ((mode == GAME_MENU || mode == GAME_PAUSED) && IsKeyPressed(KEY_N)) {
            PlaySound(menuButton);
            returnMode = mode;
            mode = GAME_SETTINGS;
        }
        else if (mode == GAME_SETTINGS && IsKeyPressed(KEY_N)) {
            PlaySound(menuButton);
            mode = returnMode;
        }

        if (mode == GAME_PLAYING && IsKeyPressed(KEY_ESCAPE)) {
            PlaySound(menuButton);
            mode = GAME_PAUSED;
        }
        else if (mode == GAME_PAUSED && IsKeyPressed(KEY_ESCAPE)) {
            PlaySound(menuButton);
            mode = GAME_PLAYING;
        }

        if (mode == GAME_PLAYING && !pauseHintShown) {
            DrawText("ESC - Pause", SCR_WIDTH - 120, SCR_HEIGHT - 30, 16, DARKGRAY);
        }

        if (mode == GAME_PAUSED && !pauseHintShown) {
            pauseHintShown = true;
        }

        if (IsKeyPressed(KEY_F11)) {
            static bool fullscreen = false;
            if (!fullscreen) {
                SetWindowState(FLAG_WINDOW_UNDECORATED);
                SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                SetWindowPosition(0, 0);
            } else {
                SetWindowSize(START_SCR_WIDTH, START_SCR_HEIGHT);
                SetWindowPosition(500, 300);
                ClearWindowState(FLAG_WINDOW_UNDECORATED);
            }
            fullscreen = !fullscreen;
        }

        int HP_WIDTH = SCR_WIDTH * 0.125;
        int player2X = SCR_WIDTH - HP_WIDTH - 10;
        int TEXT_SIZE = SCR_HEIGHT * 0.025;
        int centerY = SCR_HEIGHT / 2;

        BeginDrawing();
        ClearBackground(WHITE);

        if (mode == GAME_MENU) {
            const char *menuTitle = "DUNGEON SLAYER";
            const char *menuHint = "Press ENTER to start";
            const char *menuHint2 = "Press N to settings";
            const char *menuHint3 = "Press ESC to exit";
            const char *menuHint4 = "Press F11 to fullscreen";

            int titleSize = 50;
            int hintSize = 20;
            float alpha = sinf(GetTime() * 1.5f) * 0.5f + 0.8f;
            Color textCol = Fade(GRAY, alpha);

            int titleX = (SCR_WIDTH - MeasureText(menuTitle, titleSize)) / 2;
            int hintX = (SCR_WIDTH - MeasureText(menuHint, hintSize)) / 2;
            int hint2X = (SCR_WIDTH - MeasureText(menuHint2, hintSize)) / 2;
            int hint3X = (SCR_WIDTH - MeasureText(menuHint3, hintSize)) / 2;
            int hint4X = (SCR_WIDTH - MeasureText(menuHint4, hintSize)) / 2;

            DrawText(menuTitle, titleX + 3, centerY - 197, titleSize, Fade(BLACK, alpha));
            DrawText(menuTitle, titleX, centerY - 200, titleSize, textCol);
            DrawText(menuHint, hintX, centerY - 50, hintSize,   DARKGRAY);
            DrawText(menuHint2, hint2X, centerY - 10, hintSize, DARKGRAY);
            DrawText(menuHint3, hint3X, centerY + 30, hintSize, DARKGRAY);
            DrawText(menuHint4, hint4X, 10, hintSize, DARKGRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(menuButton);
                mode = GAME_PLAYING;

                currentRoomIndex = 0;
                currentRoom = &rooms[0];
                InitEnemies(enemies, currentRoom);

                InitPlayer(&player1, (Vector2){p1SpawnX, p1SpawnY}, "./Textures/Player_1.png", "./Textures/Weapon_1.png");
                InitPlayer(&player2, (Vector2){p2SpawnX, p2SpawnY}, "./Textures/Player_2.png", "./Textures/Weapon_2.png");
            }
            else if (IsKeyPressed(KEY_ESCAPE)) shouldExit = true;
        }
        else if (mode == GAME_PLAYING) {
            size_t remainingEnemies = RemainingEnemiesCount(enemies, MAX_ENEMIES);
            bool roomCleared = (remainingEnemies == 0);

            RoomTransform transform = RecalculateGameArea(game, currentRoom, SCR_WIDTH, SCR_HEIGHT);

            bool p1AtDoor = IsPlayerAtDoor(&player1, currentRoom);
            bool p2AtDoor = IsPlayerAtDoor(&player2, currentRoom);
            bool allAlivePlayersAtDoor = true;
            if (player1.alive && !p1AtDoor) allAlivePlayersAtDoor = false;
            if (player2.alive && !p2AtDoor) allAlivePlayersAtDoor = false;

            if (roomCleared && allAlivePlayersAtDoor) {
                currentRoomIndex++;

            if (currentRoomIndex >= NUM_ROOM) {
                PlaySound(cleared);
                mode = GAME_FINISHED;

            } else {
                currentRoom = &rooms[currentRoomIndex];

                float spawnY = currentRoom->decorDoor.y - currentRoom->decorDoor.height + 10;
                float spawnX1 = currentRoom->bounds.width / 2 - 50;
                float spawnX2 = currentRoom->bounds.width / 2 + 50;

                player1.position = (Vector2){spawnX1, spawnY};
                player2.position = (Vector2){spawnX2, spawnY};

                player1.rect.x = player1.position.x;
                player1.rect.y = player1.position.y;
                player2.rect.x = player2.position.x;
                player2.rect.y = player2.position.y;

                InitEnemies(enemies, currentRoom);
            }
        }

        UpdatePlayer(&player1, dt, KEY_W, KEY_S, KEY_A, KEY_D, playerWalk);
        UpdatePlayer(&player2, dt, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, playerWalk);
        UpdateEnemies(enemies, &player1, &player2, dt);

        ResolveEnemyCollision(enemies);
        CheckPlayerCollision(&player1, &player2);
        CheckRoomCollision(&player1, currentRoom, transform.offset, transform.scale);
        CheckRoomCollision(&player2, currentRoom, transform.offset, transform.scale);

        DrawRoom(currentRoom, transform.offset, transform.scale);

        for (size_t i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].alive) continue;

                PlayerAttack(&player1, &enemies[i], dt, playerAttack);
                PlayerAttack(&player2, &enemies[i], dt, playerAttack);

                CheckEnemyPlayerCollision(&enemies[i], &player1);
                CheckEnemyPlayerCollision(&enemies[i], &player2);
            }

            DrawEnemy(enemies, transform.offset, transform.scale);
            DrawPlayerRange(player1, transform.offset, transform.scale);
            DrawPlayerRange(player2, transform.offset, transform.scale);
        
            DrawPlayer(player1, transform.offset, transform.scale);
            DrawPlayer(player2, transform.offset, transform.scale);

            DrawPlayerUI(player1, 10, 10, SCR_HEIGHT, HP_WIDTH, TEXT_SIZE, transform.scale);
            DrawPlayerUI(player2, player2X, 10, SCR_HEIGHT, HP_WIDTH, TEXT_SIZE, transform.scale);
            DrawTopUI(currentRoom, 10, SCR_WIDTH, TEXT_SIZE, remainingEnemies);
        }
        else if (mode == GAME_PAUSED) {
            DrawRectangle(0, 0, SCR_WIDTH, SCR_HEIGHT, GRAY);

            const char *pauseTitle = "PAUSED";
            const char *pauseHint = "ESC - Resume";
            const char *pauseHint2 = "N - Settings";
            const char *pauseHint3 = "ENTER - Back to menu";

            int titleSize = 50;
            int hintSize = 20;

            int titleX = (SCR_WIDTH - MeasureText(pauseTitle, titleSize)) / 2;
            int hintX = (SCR_WIDTH - MeasureText(pauseHint, hintSize)) / 2;
            int hintX2 = (SCR_WIDTH - MeasureText(pauseHint2, hintSize)) / 2;
            int hintX3 = (SCR_WIDTH - MeasureText(pauseHint3, hintSize)) / 2;

            DrawText(pauseTitle, titleX, SCR_HEIGHT / 2 - 200, titleSize, WHITE);
            DrawText(pauseHint, hintX, SCR_HEIGHT / 2 - 50, hintSize, LIGHTGRAY);
            DrawText(pauseHint2, hintX2, SCR_HEIGHT / 2 - 10, hintSize, LIGHTGRAY);
            DrawText(pauseHint3, hintX3, SCR_HEIGHT / 2 + 30, hintSize, LIGHTGRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(menuButton);
                mode = GAME_MENU;
            }
        }
        else if (mode == GAME_SETTINGS) {
            DrawRectangle(0, 0, SCR_WIDTH, SCR_HEIGHT, Fade(BLACK, 0.6f));

            if (IsKeyPressed(KEY_RIGHT)) settings.masterVolume += 0.1f;
            if (IsKeyPressed(KEY_LEFT)) settings.masterVolume -= 0.1f;

            settings.masterVolume = Clamp(settings.masterVolume, 0.0f, 1.0f);
            SetMasterVolume(settings.masterVolume);

            if (IsKeyPressed(KEY_UP)) settings.targetFPS += 30;
            if (IsKeyPressed(KEY_DOWN)) settings.targetFPS -= 30;

            if (settings.targetFPS < 30) settings.targetFPS = 30;

            SetTargetFPS(settings.targetFPS);

            const char *settingsTitle = "SETTINGS";
            const char *settingsSub = "Volume: %d%%";
            const char *settingsSub2 = "FPS: %d";
            const char *settingsHint = "LEFT / RIGHT - VOLUME";
            const char *settingsHint2 = "UP / DOWN - FPS";
            const char *settingsHint3 = "N - Back";

            int titleSize = 50;
            int subSize = 25;
            int hintSize = 20;

            int titleX = (SCR_WIDTH - MeasureText(settingsTitle, titleSize)) / 2;
            int subX = (SCR_WIDTH - MeasureText(settingsSub, subSize)) / 2;
            int subX2 = (SCR_WIDTH - MeasureText(settingsSub2, subSize)) / 2;
            int hintX = (SCR_WIDTH - MeasureText(settingsHint, hintSize)) / 2;
            int hintX2 = (SCR_WIDTH - MeasureText(settingsHint2, hintSize)) / 2;
            int hintX3 = (SCR_WIDTH - MeasureText(settingsHint3, hintSize)) / 2;

            DrawText(settingsTitle, titleX, SCR_HEIGHT / 2 - 200, titleSize, WHITE);
            DrawText(TextFormat(settingsSub, (int)(settings.masterVolume * 100)), subX, SCR_HEIGHT / 2 - 80, subSize, WHITE);
            DrawText(TextFormat(settingsSub2, settings.targetFPS), subX2, SCR_HEIGHT / 2 - 40, subSize, WHITE);
            DrawText(settingsHint, hintX, SCR_HEIGHT / 2 + 20, hintSize, LIGHTGRAY);
            DrawText(settingsHint2, hintX2, SCR_HEIGHT / 2 + 60, hintSize, LIGHTGRAY);
            DrawText(settingsHint3, hintX3, SCR_HEIGHT / 2 + 100, hintSize, LIGHTGRAY);
        }
        else if (mode == GAME_FINISHED) {
            const char *endTitle = "YOU CLEARED THE DUNGEON!";
            const char *endSub = "Thanks for playing";
            const char *endHint = "Press ENTER to return to menu";

            int titleSize = 50;
            int subSize = 25;
            int hintSize = 20;

            int titleX = (SCR_WIDTH - MeasureText(endTitle, titleSize)) / 2;
            int subX = (SCR_WIDTH - MeasureText(endSub, subSize)) / 2;
            int hintX = (SCR_WIDTH - MeasureText(endHint, hintSize)) / 2;

            DrawText(endTitle, titleX, centerY - 200, titleSize, BLACK);
            DrawText(endSub, subX, centerY - 100, subSize, GREEN);
            DrawText(endHint, hintX, centerY - 10, hintSize, DARKGRAY);
        
            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(menuButton);
                mode = GAME_MENU;
            }
        }

        DrawText(TextFormat("FPS: %d", GetFPS()), 10, SCR_HEIGHT - 20, 15, GREEN);

        EndDrawing();
    }

    UnloadTexture(player1.sprite);
    UnloadTexture(player1.weaponSprite);
    UnloadTexture(player2.sprite);
    UnloadTexture(player2.weaponSprite);

    UnloadSound(menuButton);
    UnloadSound(playerWalk);
    UnloadSound(playerAttack);
    UnloadSound(cleared);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}