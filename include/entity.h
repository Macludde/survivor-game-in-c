#pragma once
#include <stdbool.h>

#include "debug.h"
#include "physics_math.h"
#include "raylib.h"

enum EntityType { ENTITY_TYPE_TREE, ENTITY_TYPE_PLAYER, ENTITY_TYPE_ENEMY };

typedef struct Entity {
  float health;
  PhysicsBody body;
  enum EntityType type;
} Entity;

// returns remaining health. so negative means overkill, 0 means perfect kill,
// positive means still alive
float EntityTakeDamage(Entity *entity, float damage);
// deal damage to all entities in area
void EntityAreaTakeDamage(Entity *entities[], int count, Vector2 pos,
                          float radius, float damage);
void TickEntities(Entity *entities[], int count);
void HandleAllRigidBodyCollisions(Entity *entities[], int count);

void DrawHitboxes(Entity *entities[], int count);
