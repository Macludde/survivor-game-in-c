#pragma once
#include <stdlib.h>

#include "entity.h"
#include "level.h"
#include "physics.h"
#include "raylib.h"
#include "weapon_type.h"  // weapon refers to player

// this scales with player speed
#define ACCELERATION_SPEED 10.0f
#define RETARDATION_SPEED 15.0f
#define PLAYER_COLOR DARKGREEN

typedef struct Player {
  Entity entity;
  float speed;
  Weapon weapon;
  int experience;
} Player;

Player InitialPlayer();

void TickPlayer(Player *player);
void DrawPlayer(Player *player);