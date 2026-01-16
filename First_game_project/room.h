#ifndef ROOM_H
#define ROOM_H

#include <raylib.h>
#include "globals.h"


typedef struct GameArea {
    Rectangle bounds;
} GameArea;

typedef struct Room {
    Rectangle bounds;
    int width;
    int height;
    int RoomNumber;
    int enemiesRemaining;
    int enemyCount;
    Vector2 *enemyPositions;
    Rectangle door;
    Rectangle decorDoor;
} Room;

typedef struct RoomTransform {
    Vector2 offset;
    float scale;
} RoomTransform;

RoomTransform RecalculateGameArea(GameArea game, Room *currentRoom, int SCR_WIDTH, int SCR_HEIGHT);
void InitRoom(Room rooms[]);
void DrawRoom(Room *currentRoom, Vector2 offset, float scale);

#endif