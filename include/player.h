#pragma once
#include <stdlib.h>

typedef struct Player Player;
#include "entity.h"
#include "item.h"
#include "level.h"
#include "physics.h"
#include "raylib.h"
#include "weapon.h"

// this scales with player speed
#define ACCELERATION_SPEED 10.0f
#define RETARDATION_SPEED 15.0f
#define PLAYER_COLOR DARKGREEN

struct Player {
  Entity entity;
  float speed;
  Weapon *weapon;
  int experience;
  PlayerItem **items;
};

Player InitialPlayer();

void TickPlayer(Player *player);
void DrawPlayer(Player *player);