#include "room.h"
#include "globals.h"
#include <stdlib.h>
#include <raymath.h>
#include <raylib.h>

RoomTransform RecalculateGameArea(GameArea game, Room *currentRoom, int SCR_WIDTH, int SCR_HEIGHT) {
    int TOP_UI_HEIGHT = SCR_HEIGHT * 0.05;
    int SIDE_UI_WIDTH = SCR_WIDTH * 0.15;
    int PADDING = 10;

    game.bounds = (Rectangle){
        SIDE_UI_WIDTH + PADDING,
        TOP_UI_HEIGHT + PADDING,
        SCR_WIDTH - (SIDE_UI_WIDTH * 2) - PADDING * 2,
        SCR_HEIGHT - TOP_UI_HEIGHT - PADDING * 2
    };

    float scaleX = game.bounds.width / currentRoom->bounds.width;
    float scaleY = game.bounds.height / currentRoom->bounds.height;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    Vector2 offset = {
        game.bounds.x + (game.bounds.width - currentRoom->bounds.width * scale) / 2,
        game.bounds.y + (game.bounds.height - currentRoom->bounds.height * scale) / 2
    };

    RoomTransform result;
    result.offset = offset;
    result.scale = scale;
    return result;
}

void InitRoom(Room rooms[]) {
    for (int i = 0; i < NUM_ROOM; i++) {
        rooms[i].bounds.width = 800;
        rooms[i].bounds.height = 600;
        rooms[i].RoomNumber = i + 1;
        rooms[i].enemiesRemaining = MAX_ENEMIES;

        rooms[i].enemyCount = 3 + i;

        rooms[i].enemyPositions = malloc(sizeof(Vector2) * rooms[i].enemyCount);
        for (int j = 0; j < rooms[i].enemyCount; j++) {
            rooms[i].enemyPositions[j] = (Vector2){
                50 + rand() % ((int)rooms[i].bounds.width - 100),
                50 + rand() % ((int)rooms[i].bounds.height - 100)
            };
        }

        int doorWidth = 80;
        int doorHeight = 30;
        int doorOffsetX = doorWidth / 2;

        if (i == 0) {
            rooms[i].door = (Rectangle){rooms[i].bounds.width / 2 - doorOffsetX, 0, doorWidth, doorHeight};
            rooms[i].decorDoor = (Rectangle){0,0,0,0};
        } else {
            rooms[i].door = (Rectangle){rooms[i].bounds.width / 2 - doorOffsetX, 0, doorWidth, doorHeight};
            rooms[i].decorDoor = (Rectangle){rooms[i].bounds.width / 2 - doorOffsetX, rooms[i].bounds.height - doorHeight, doorWidth, doorHeight};
        }
    }   
}

void DrawRoom(Room *currentRoom, Vector2 offset, float scale) {
    DrawRectangle(
        offset.x,
        offset.y,
        currentRoom->bounds.width * scale,
        currentRoom->bounds.height * scale,
        GRAY
    );

    DrawRectangle(
        offset.x + currentRoom->door.x * scale,
        offset.y + currentRoom->door.y * scale,
        currentRoom->door.width * scale,
        currentRoom->door.height * scale,
        DARKBROWN
    );

    if (currentRoom->decorDoor.width > 0 && currentRoom->decorDoor.height > 0) {
        DrawRectangle(
            offset.x + currentRoom->decorDoor.x * scale,
            offset.y + currentRoom->decorDoor.y * scale,
            currentRoom->decorDoor.width * scale,
            currentRoom->decorDoor.height * scale,
            DARKBROWN
    );
    }
}