#pragma once
#include <stdlib.h>

#include "raylib.h"

typedef struct Bullet {
  PhysicsBody body;
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