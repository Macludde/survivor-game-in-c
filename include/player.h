#pragma once
#include <stdlib.h>

typedef struct OLD_Player OLD_Player;
#include "entity.h"
#include "item.h"
#include "level.h"
#include "physics_math.h"
#include "raylib.h"

// this scales with player speed
#define ACCELERATION_SPEED 10.0f
#define RETARDATION_SPEED 15.0f
#define PLAYER_COLOR DARKGREEN

// all of these stats are modifiers, default is 1.0f for all
typedef struct PlayerStats {
  float damage;
  float bulletSpeed;
  float movementSpeed;
  float attackSpeed;
  float range;
} PlayerStats;
struct OLD_Player {
  Entity entity;
  int experience;
  PlayerStats stats;
  PlayerItem **items;
};

OLD_Player InitialPlayer();

void TickPlayer(OLD_Player *player);
void DrawPlayer(OLD_Player *player);