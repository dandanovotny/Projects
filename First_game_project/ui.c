#include "ui.h"
#include "globals.h"
#include <raymath.h>
#include <raylib.h>

void DrawPlayerUI(Player player, int x, int y, int SCR_HEIGHT, int HP_WIDTH, int TEXT_SIZE, float scale) {
    int SPACING = SCR_HEIGHT * 0.03;
    int HP_HEIGHT = SCR_HEIGHT * 0.025;
    const int HP_TEXT_Y = y + 1;
    int ARMOR_X = HP_WIDTH;
    int ARMOR_Y = y + SPACING;
    int ARMOR_HEIGHT = HP_HEIGHT;
    const int ARMOR_TEXT_Y = ARMOR_Y + 1;
    int COINS_Y = y + SPACING * 2.5;
    int WEAPON_Y = y + SPACING * 4;
    int weaponSize = (int)(SIZE * scale);
        
    //HP
    float hpPercent = (float)player.hp / 100.0f;
    DrawRectangle(x, y, HP_WIDTH * hpPercent, HP_HEIGHT, RED);
    DrawRectangleLines(x, y, HP_WIDTH, HP_HEIGHT, BLACK);

    if (player.hp <= 50) {
        DrawRectangle(x, y, HP_WIDTH * hpPercent, HP_HEIGHT, ORANGE);
    }

    int textWidthHp = MeasureText(TextFormat("%d", player.hp), TEXT_SIZE);
    int HP_TEXT_X = x + (HP_WIDTH - textWidthHp) / 2;
    DrawText(TextFormat("%d", player.hp), HP_TEXT_X, HP_TEXT_Y, TEXT_SIZE, BLACK);

    //Armor
    float armorPercent = (float)player.armor / player.maxArmor;
    DrawRectangle(x, ARMOR_Y, ARMOR_X * armorPercent, ARMOR_HEIGHT, ORANGE);
    DrawRectangleLines(x, ARMOR_Y, ARMOR_X, ARMOR_HEIGHT, BLACK);

    int textWidthArmor = MeasureText(TextFormat("%d", player.armor), TEXT_SIZE);
    int ARMOR_TEXT_X = x + (ARMOR_X - textWidthArmor) / 2;
    DrawText(TextFormat("%d", player.armor), ARMOR_TEXT_X, ARMOR_TEXT_Y, TEXT_SIZE, BLACK);

    //Coins
    DrawText(TextFormat("Coins: %d", player.coins), x, COINS_Y, TEXT_SIZE, BLACK);

    //Weapon
    DrawRectangle(x, WEAPON_Y, weaponSize, weaponSize, DARKGRAY);
    DrawRectangleLines(x, WEAPON_Y, weaponSize, weaponSize, BLACK);
    DrawTextureEx(player.weaponSprite, (Vector2){x, WEAPON_Y}, 0.0f, scale, WHITE);
}

void DrawTopUI(Room *currentRoom, int y, int SCR_WIDTH, int TEXT_SIZE, size_t remainingEnemies) {
    int spacing = SCR_WIDTH * 0.25;
    int ENEMIES_TEXT_Y = y + 5; 
    int ROOM_TEXT_Y = ENEMIES_TEXT_Y;

    //Remaing Enemies / Room Number
    int textWidthEnemies = MeasureText(TextFormat("Enemies: %d", remainingEnemies), TEXT_SIZE);

    int textWidthRoom = MeasureText(TextFormat("Room: %d", currentRoom->RoomNumber), TEXT_SIZE);

    int totalWidth = textWidthEnemies + spacing +textWidthRoom;
    int startX = (SCR_WIDTH - totalWidth) / 2;
    DrawText(TextFormat("Enemies: %zu", remainingEnemies), startX, ENEMIES_TEXT_Y, TEXT_SIZE, BLACK);
    DrawText(TextFormat("Room: %d", currentRoom->RoomNumber), startX + textWidthEnemies + spacing, ROOM_TEXT_Y, TEXT_SIZE, BLACK);
}