#pragma once
#include <stdlib.h>

typedef struct Weapon Weapon;
#include "level.h"
#include "player.h"
#include "raylib.h"

#define DEFAULT_BULLET_CAPACITY 100

typedef struct Bullet {
  PhysicsBody body;
  bool spawned;
  float size;
} Bullet;

struct Weapon {
  float damage;
  float speed;
  float range;
  float fireRate;
  double lastFired;
  Bullet *bullets;
  int bulletCapacity;
};

Weapon *InitialWeapon();

void TickWeapon(Weapon *weapon, Player *player);
void DrawWeapon(Weapon *weapon);