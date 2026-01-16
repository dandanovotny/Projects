#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

typedef struct Enemy Enemy;
typedef struct Room Room;

typedef struct Weapon {
        int dmg;
        float range;
        float attackCooldown;
        float attackTimer;
        //Texture2D sprite; -WIP-
    } Weapon;

typedef struct Player {
        Vector2 position;
        Vector2 prevPosition;
        Rectangle rect;
        int hp;
        int armor;
        int maxArmor;
        int coins;
        int speed;
        bool alive;

        Weapon weapon;

        Texture2D sprite;
        Texture2D weaponSprite;

        float footstepTimer;
        float footstepIntervar;
        
        float scale;
        Vector2 offset;
    } Player;

void InitPlayer(Player *player, Vector2 position, const char *playerTex, const char *weaponTex);
void UpdatePlayer(Player *player, float dt, int upKey, int downKey, int leftKey, int rightKey, Sound playerWalk);
void DrawPlayer(Player player, Vector2 offset, float scale);
void DrawPlayerRange(Player player, Vector2 offset, float scale);
void DamagePlayer(Player *player, Enemy *enemy);
void PlayerAttack(Player *player, Enemy *enemy, float dt, Sound playerAttack);
void CheckRoomCollision(Player *player, Room *currentRoom, Vector2 offset, float scale);
void CheckPlayerCollision(Player *player1, Player *player2);
bool IsPlayerAtDoor(const Player *player, const Room *room);

#endif