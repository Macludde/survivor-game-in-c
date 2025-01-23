#pragma once
#include <stdlib.h>

#include "level.h"
#include "raylib.h"
#include "weapon.h"

// this scales with player speed
#define ACCELERATION_SPEED 5.0f
#define RETARDATION_SPEED 10.0f
#define PLAYER_COLOR DARKGREEN

#define DEFAULT_BULLET_CAPACITY 100

typedef struct Player {
  Vector2 pos;
  Vector2 velocity;
  float speed;
  float size;
  Weapon weapon;
} Player;

Player InitialPlayer();

void TickPlayer(Player *player);
void DrawPlayer(Player *player);