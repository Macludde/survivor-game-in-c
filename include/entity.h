#pragma once
#include <stdbool.h>

#include "debug.h"
#include "physics.h"
#include "raylib.h"

enum Team { NEUTRAL, PLAYER, ENEMIES };

typedef struct Entity {
  float health;
  PhysicsBody body;
  enum Team team;
} Entity;

// returns remaining health. so negative means overkill, 0 means perfect kill,
// positive means still alive
inline static float EntityTakeDamage(Entity *entity, float damage) {
  entity->health -= damage;
  return entity->health;
}

void HandleAllRigidBodyCollisions(Entity *entities[], int count);

void DrawHitboxes(Entity *entities[], int count);
