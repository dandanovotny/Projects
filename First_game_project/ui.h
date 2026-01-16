#ifndef UI_H
#define UI_H

#include "player.h"
#include "room.h"
#include <stddef.h>


void DrawPlayerUI(Player player, int x, int y, int SCR_WIDTH, int SCR_HEIGHT, int TEXT_SIZE, float scale);
void DrawTopUI(Room *currentRoom, int y, int SCR_WIDTH, int TEXT_SIZE, size_t remainingEnemies);

#endif