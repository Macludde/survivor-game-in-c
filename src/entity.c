#include "entity.h"

#include "debug.h"
#include "physics.h"
#include "raylib.h"

void HandleAllRigidBodyCollisions(Entity *entities[], int count) {
  for (int i = 0; i < count; ++i) {
    for (int j = i + 1; j < count; ++j) {
      if (entities[i]->team == entities[j]->team == ENEMIES) continue;
      if (CheckCollision(entities[i]->body, entities[j]->body)) {
        RigidCollision(&entities[i]->body, &entities[j]->body);
      }
    }
  }
}

void DrawHitboxes(Entity *entities[], int count) {
  for (int i = 0; i < count; ++i) {
    DrawCircleLinesV(entities[i]->body.pos, entities[i]->body.radius, PINK);
  }
}