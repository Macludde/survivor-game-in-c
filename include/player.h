#pragma once
#include <stdlib.h>

#include "level.h"
#include "physics.h"
#include "raylib.h"
#include "weapon.h"

// this scales with player speed
#define ACCELERATION_SPEED 10.0f
#define RETARDATION_SPEED 15.0f
#define PLAYER_COLOR DARKGREEN

#define DEFAULT_BULLET_CAPACITY 100

typedef struct Bullet {
  PhysicsBody body;
  bool spawned;
} Bullet;

typedef struct Weapon {
  float damage;
  float speed;
  float range;
  float fireRate;
  double lastFired;
  Bullet *bullets;
  int bulletCapacity;
} Weapon;
typedef struct Player {
  PhysicsBody body;
  float speed;
  Weapon weapon;
} Player;

Player InitialPlayer();

void TickPlayer(Player *player);
void DrawPlayer(Player *player);