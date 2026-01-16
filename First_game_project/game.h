#ifndef GAME_H
#define GAME_H

typedef enum {
    GAME_MENU,
    GAME_PLAYING,
    GAME_PAUSED,
    GAME_SETTINGS,
    GAME_FINISHED
} GameMode;

typedef struct GameSettings {
    float masterVolume;
    int targetFPS;
} GameSettings;

#endif