#include "player.h"
#include "enemy.h"
#include "room.h"
#include "globals.h"

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

void InitPlayer(Player *player, Vector2 position, const char *playerTex, const char *weaponTex) {
        player->position = position;

        player->position = (Vector2){position.x, position.y};
        player->rect = (Rectangle){position.x, position.y, SIZE, SIZE};
        player->hp = 100;
        player->armor = 5;
        player->maxArmor = 5;
        player->coins = 0;
        player->speed = 100;
        player->alive = true;

        player->weapon.dmg = 5;
        player->weapon.range = 20;
        player->weapon.attackCooldown = 0.5f;
        player->weapon.attackTimer = 0.0f;

        player->footstepTimer = 0.0f;
        player->footstepIntervar = 0.3f;

        
        player->sprite = LoadTexture(playerTex);
        player->weaponSprite = LoadTexture(weaponTex);
    }

void UpdatePlayer(Player *player, float dt, int upKey, int downKey, int leftKey, int rightKey, Sound playerWalk) {
    player->prevPosition = player->position;

    float speed_DT = player->speed * dt;
    bool moving = false;

    if (player->alive) {
        if (IsKeyDown(upKey)) { player->position.y -= speed_DT; moving = true; }
        if (IsKeyDown(downKey)) { player->position.y += speed_DT; moving = true; }
        if (IsKeyDown(leftKey)) { player->position.x -= speed_DT; moving = true; }
        if (IsKeyDown(rightKey)) { player->position.x += speed_DT; moving = true; }
    }

    if (moving) {
        player->footstepTimer -= dt;
        if (player->footstepTimer <= 0) {
            PlaySound(playerWalk);
            player->footstepTimer = player->footstepIntervar;
        }
    } else {
        player->footstepTimer = 0;
    }

    player->rect.x = player->position.x;
    player->rect.y = player->position.y;
}

void DrawPlayer(Player player, Vector2 offset, float scale) {
    if (!player.alive) return;

        DrawTextureEx(
            player.sprite,
            (Vector2){
                offset.x + player.position.x * scale,
                offset.y + player.position.y * scale
            },
            0.0f,
            scale,
            WHITE
        );
}

void DrawPlayerRange(Player player, Vector2 offset, float scale) {
    float rangePixels = player.weapon.range * scale;
        
    Vector2 center = {
        offset.x + player.position.x * scale + SIZE * scale / 2,
        offset.y + player.position.y * scale + SIZE * scale / 2
    };

    DrawCircleLines(center.x, center.y, rangePixels, LIGHTGRAY);
}

void DamagePlayer(Player *player, Enemy *enemy) {
    if (!player->alive) return;

    if (player->armor > 0) player->armor -= enemy->dmgARMOR;
    else player->hp -= enemy->dmgHP;

    if (player->hp <= 0) {
        player->hp = 0;
        player->alive = false;
    }
}

void PlayerAttack(Player *player, Enemy *enemy, float dt, Sound playerAttack) {
    if (!player->alive || !enemy->alive) return;

        float distance = Vector2Distance(player->position, enemy->position);

        player->weapon.attackTimer -= dt;
        if (distance <= player->weapon.range && player->weapon.attackTimer <= 0) {
            PlaySound(playerAttack);
            enemy->hp -= player->weapon.dmg;

            if (enemy->hp <= 0) {
                enemy->hp = 0;
                enemy->alive = false;
                player->coins += enemy->coinReward;
            }
            player->weapon.attackTimer = player->weapon.attackCooldown;
        }
}

void CheckRoomCollision(Player *player, Room *currentRoom, Vector2 offset, float scale) {
    float playerX = offset.x + player->position.x * scale;
    float playerY = offset.y + player->position.y * scale;
    float playerSize = SIZE * scale;

    float roomLeft = offset.x;
    float roomTop = offset.y;
    float roomRight = offset.x + currentRoom->bounds.width * scale;
    float roomBottom = offset.y + currentRoom->bounds.height * scale;

    if (playerX < roomLeft) player->position.x = (roomLeft - offset.x) / scale;
    if (playerY < roomTop) player->position.y = (roomTop - offset.y) / scale;
    if (playerX + playerSize > roomRight) player->position.x = currentRoom->bounds.width - SIZE;
    if (playerY + playerSize > roomBottom) player->position.y = currentRoom->bounds.height - SIZE;
}

void CheckPlayerCollision(Player *player1, Player *player2) {
    if (!player1->alive || !player2->alive) return;

    Rectangle r1 = player1->rect;
    Rectangle r2 = player2->rect;

    if (CheckCollisionRecs(r1, r2)) {
        float overlapX = (r1.width + r2.width) / 2 - fabs((r1.x + r1.width / 2) - (r2.x + r2.width / 2));
        float overlapY = (r1.height + r2.height) / 2 - fabs((r1.y + r1.height / 2) - (r2.y + r2.height / 2));

        if (overlapX < overlapY) {
            if (r1.x < r2.x) player1->position.x -= overlapX / 2;
            else player1->position.x += overlapX / 2;
            
            if (r1.x < r2.x) player2->position.x += overlapX / 2;
            else player2->position.x -= overlapX / 2;

        } else {
            if (r1.y < r2.y) player1->position.y -= overlapY / 2;
            else player1->position.y += overlapY / 2;

            if (r1.y < r2.y) player2->position.y += overlapY / 2;
            else player2->position.y -= overlapY / 2;
        }

        player1->rect.x = player1->position.x;
        player1->rect.y = player1->position.y;
        player2->rect.x = player2->position.x;
        player2->rect.y = player2->position.y;
    }
}

bool IsPlayerAtDoor(const Player *player, const Room *room) {
    Rectangle playerRec = {
        player->position.x,
        player->position.y,
        SIZE,
        SIZE
    };

    return CheckCollisionRecs(playerRec, room->door);
}