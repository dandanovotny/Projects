#include "enemy.h"
#include "room.h"
#include "globals.h"
#include "player.h"
#include <stdlib.h>
#include <math.h>
#include <raymath.h>
#include <raylib.h>

void InitEnemies(Enemy enemies[], Room *room) {
    for (size_t i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].position = (Vector2){0, 0};
        enemies[i].hp = 10;
        enemies[i].dmgHP = 5;
        enemies[i].dmgARMOR = 1;
        enemies[i].speed = 80;
        enemies[i].attackCooldown = 0.5f;
        enemies[i].attackTimer = 0.0f;
        enemies[i].alive = false;
        enemies[i].coinReward = 5;
        enemies[i].detectionRange = 250;
        enemies[i].rect = (Rectangle){0, 0, SIZE, SIZE};
    }
    if (!room->enemyPositions) {
        room->enemyPositions = malloc(sizeof(Vector2) * room->enemyCount);
    }

    for (size_t j = 0; j < (size_t)room->enemyCount; j++) {
        int attempts = 0;
        Vector2 pos;
        bool valid = false;

        while (attempts < 50 && !valid) {
            pos = (Vector2){
                50 + rand() % ((int)room->bounds.width - 100),
                50 + rand() % ((int)room->bounds.height - 100)
            };

            valid = true;

            for (size_t k = 0; k < j; k++) {
                if (Vector2Distance(pos, room->enemyPositions[k]) < SIZE * 2) {
                    valid = false;
                    break;
                }
            }
            attempts++;
        }

        room->enemyPositions[j] = pos;
        enemies[j].alive = true;
        enemies[j].position = pos;
        enemies[j].rect = (Rectangle){pos.x, pos.y, SIZE, SIZE};
    }
}

void DrawEnemy(Enemy enemies[], Vector2 offset, float scale) {
    int EnemySize = SIZE * scale;

    for (size_t i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;

            DrawRectangle(
                offset.x + enemies[i].position.x * scale,
                offset.y + enemies[i].position.y * scale,
                EnemySize,
                EnemySize,
                RED
            ); 
    }  
}

void UpdateEnemies(Enemy enemies[], Player *player1, Player *player2, float dt) {
    for (size_t i = 0; i < MAX_ENEMIES; i++) {
        Enemy *enemy = &enemies[i];
        if (!enemy->alive) continue;

        enemy->prevPosition = enemy->position;

        enemy->attackTimer += dt;

        float EnemySpeed = enemy->speed * dt;

        float d1 = Vector2Distance(enemy->position, player1->position);
        float d2 = Vector2Distance(enemy->position, player2->position);

        Player* target = 0;

        if (player1->alive && d1 <= enemy->detectionRange) target = player1;
        if (player2->alive && d2 <= enemy->detectionRange) {
            if (!target || d2 < d1) target = player2;
        }

        if (!target) continue;

        Vector2 dir = Vector2Subtract(target->position, enemy->position);

        if (Vector2Length(dir) > 0.1f) {
            dir = Vector2Normalize(dir);
            enemy->position.x += dir.x * EnemySpeed;
            enemy->position.y += dir.y * EnemySpeed;
        }
    }
}

void ResolveEnemyCollision(Enemy enemies[]) {
    for (size_t i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;

        for (size_t j = i + 1; j < MAX_ENEMIES; j++) {
            if (!enemies[j].alive) continue;

            Rectangle r1 = {enemies[i].position.x, enemies[i].position.y, SIZE, SIZE};
            Rectangle r2 = {enemies[j].position.x, enemies[j].position.y, SIZE, SIZE};

            if (CheckCollisionRecs(r1, r2)) {
                float overlapX = (r1.width + r2.width) / 2 - fabs((r1.x + r1.width / 2) - (r2.x + r2.width / 2));
                float overlapY = (r1.height + r2.height) / 2 - fabs((r1.y + r1.height / 2) - (r2.y + r2.height / 2));

                if (overlapX < overlapY) {
                    if (r1.x < r2.x) {
                        enemies[i].position.x -= overlapX / 2;
                        enemies[j].position.x += overlapX / 2;
                    } else {
                        enemies[i].position.x += overlapX / 2;
                        enemies[j].position.x -= overlapX / 2;
                    }
                } else {
                    if (r1.y < r2.y) {
                        enemies[i].position.y -= overlapY / 2;
                        enemies[j].position.y += overlapY / 2;
                    } else {
                        enemies[i].position.y += overlapY / 2;
                        enemies[j].position.y -= overlapY / 2;
                    }
                }
            }
        }
    }
}

void CheckEnemyPlayerCollision(Enemy *enemy, Player *player) {
    if (!enemy->alive) return;

    Rectangle enemyRec = {
        enemy->position.x,
        enemy->position.y,
        SIZE,
        SIZE
    };

    if (CheckCollisionRecs(player->rect, enemyRec)) {
        if (enemy->attackTimer >= enemy->attackCooldown) {
            DamagePlayer(player, enemy);
            enemy->attackTimer = 0.0f;
        }
    }
}

size_t RemainingEnemiesCount(const Enemy enemies[], size_t enemyCount) {
    size_t alive = 0;

    for (size_t i = 0; i < enemyCount; i++) {
        if (enemies[i].alive) {
            alive++;
        }
    }

    return alive;
}