#pragma once
#include <stdlib.h>

#include "level.h"
#include "raylib.h"

// this scales with player speed
#define ACCELERATION_SPEED 5.0f
#define RETARDATION_SPEED 10.0f
#define PLAYER_COLOR DARKGREEN

#define DEFAULT_BULLET_CAPACITY 100
typedef struct Bullet {
  Vector2 pos;
  Vector2 velocity;
  bool spawned;
  float size;
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
  Vector2 pos;
  Vector2 velocity;
  float speed;
  float size;
  Weapon weapon;
} Player;

Weapon InitialWeapon();
Player InitialPlayer();

void TickPlayer(Player *player, Level *level);
void DrawPlayer(Player *player);