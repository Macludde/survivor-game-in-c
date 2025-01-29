#include "entity.h"

#include "debug.h"
#include "enemy.h"
#include "physics_math.h"
#include "raylib.h"

float EntityTakeDamage(Entity *entity, float damage) {
  entity->health -= damage;
  if (entity->health <= 0) {
    switch (entity->type) {
      case ENTITY_TYPE_PLAYER:
        break;
      case ENTITY_TYPE_TREE:
        break;
      case ENTITY_TYPE_ENEMY:
        HandleEnemyDeath((Enemy *)entity);
        break;
    }
  }
  return entity->health;
}

void EntityAreaTakeDamage(Entity *entities[], int count, Vector2 pos,
                          float radius, float damage) {
  for (int i = 0; i < count; ++i) {
    if (CheckCollisionCircles(pos, radius, entities[i]->body.pos,
                              entities[i]->body.radius)) {
      EntityTakeDamage(entities[i], damage);
    }
  }
}

void HandleAllRigidBodyCollisions(Entity *entities[], int count) {
  for (int i = 0; i < count; ++i) {
    for (int j = i + 1; j < count; ++j) {
      if (entities[i]->type == ENTITY_TYPE_ENEMY &&
          entities[j]->type == ENTITY_TYPE_ENEMY)
        continue;
      if (CheckCollision(entities[i]->body, entities[j]->body)) {
        // RigidCollision(&entities[i]->body, &entities[j]->body);
      }
    }
  }
}

void TickEntities(Entity *entities[], int count) {
  for (int i = 0; i < count; ++i) {
    // MoveBody(&entities[i]->body);
  }
}

void DrawHitboxes(Entity *entities[], int count) {
  for (int i = 0; i < count; ++i) {
    DrawCircleLinesV(entities[i]->body.pos, entities[i]->body.radius, PINK);
  }
}