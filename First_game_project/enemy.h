#ifndef ENEMY_H
#define ENEMY_H

#include <stddef.h>
#include <raylib.h>
#include "globals.h"

typedef struct Player Player;
typedef struct Room Room;

typedef struct Enemy {
    Vector2 position;
    Vector2 prevPosition;
    Rectangle rect;
    int hp;
    int dmgHP;
    int dmgARMOR;
    int speed;
    float attackCooldown;
    float attackTimer;
    bool alive;
    int coinReward;
    float detectionRange;
    //Texture2D sprite; -WIP-
} Enemy;

void InitEnemies(Enemy enemies[], Room *room);
void DrawEnemy(Enemy enemies[], Vector2 offset, float scale);
void UpdateEnemies(Enemy enemies[], Player *player1, Player *player2, float dt);
void ResolveEnemyCollision(Enemy enemies[]);
void CheckEnemyPlayerCollision(Enemy *enemy, Player *player);
size_t RemainingEnemiesCount(const Enemy enemies[], size_t enemyCount);

#endif