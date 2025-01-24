#pragma once
#include <stdbool.h>

#include "physics.h"
#include "raylib.h"

enum Team { PLAYER, ENEMIES, NEUTRAL };

typedef struct Entity {
  float health;
  PhysicsBody body;
  enum Team team;
} Entity;

// returns remaining health. so negative means overkill, 0 means perfect kill,
// positive means still alive
inline float EntityTakeDamage(Entity *entity, float damage) {
  entity->health -= damage;
  return entity->health;
}