#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define DEBUG_SHOW_HITBOXES

#include "raylib.h"
#include "raymath.h"
#define STB_DS_IMPLEMENTATION
#include "flecs.h"
#include "lib/stb_ds.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 800
#define GAME_NAME "SurvivorGame"

#pragma once
#include <stdbool.h>

#include "flecs.h"
#include "raylib.h"

typedef Vector2 Position;
typedef struct Collidable {
  float radius;
} Collidable;
typedef struct CollidesWith {
  Vector2 normal;
} CollidesWith;
typedef struct Killable {
  float health;
  float maxHealth;
} Killable;
#define KILLABLE(health) {health, health}
typedef float Damage;
typedef float DropsExperience;
typedef float Experience;

ECS_COMPONENT_DECLARE(Collidable);
ECS_COMPONENT_DECLARE(CollidesWith);
ECS_COMPONENT_DECLARE(Killable);
ECS_COMPONENT_DECLARE(Damage);
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(DropsExperience);
ECS_COMPONENT_DECLARE(Experience);
ECS_COMPONENT_DECLARE(Collidable);
ECS_COMPONENT_DECLARE(CollidesWith);

ecs_query_t *q_collidables = NULL;

// TODO: use a spatial grid or BVH to improve performance of collision detection

void Collide(ecs_iter_t *it) {
  const Collidable *c1 = ecs_field(it, Collidable, 0);
  const Position *p1 = ecs_field(it, Position, 1);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e1 = it->entities[i];

    // For each matching entity, iterate the query
    ecs_iter_t qit = ecs_query_iter(it->world, q_collidables);
    while (ecs_query_next(&qit)) {
      const Collidable *c2 = ecs_field(&qit, Collidable, 0);
      const Position *p2 = ecs_field(&qit, Position, 1);
      for (int j = 0; j < qit.count; j++) {
        ecs_entity_t e2 = qit.entities[j];
        if (e1 == e2) continue;  // don't collide with self
        if (e2 > e1)
          continue;  // Prevent collisions from being detected twice with the
                     // entities reversed.

        if (CheckCollisionCircles(p1[i], c1[i].radius, p2[j], c2[j].radius)) {
          Vector2 normal = Vector2Normalize(Vector2Subtract(p1[i], p2[j]));
          // if (!ecs_has_pair(it->world, e1, ecs_id(CollidesWith), e2))
          ecs_set_pair(it->world, e1, CollidesWith, e2, {normal});
        } else if (ecs_has_pair(it->world, e1, ecs_id(CollidesWith), e2)) {
          ecs_remove_pair(it->world, e1, ecs_id(CollidesWith), e2);
        }
      }
    }
  }
}

ecs_entity_t CreateEnemy(ecs_world_t *world) {
  float size = 40;
  ecs_entity_t enemy = ecs_new(world);
  ecs_set(world, enemy, Killable, KILLABLE(20));
  ecs_set(world, enemy, DropsExperience, {20});
  ecs_set(world, enemy, Position, {10, 10});
  ecs_set(world, enemy, Collidable, {.radius = size / 2});
  ecs_set(world, enemy, Damage, {10});

  return enemy;
}

void KillHealthless(ecs_iter_t *it) {
  Killable *k = ecs_field(it, Killable, 0);
  for (int i = 0; i < it->count; i++) {
    if (k[i].health <= 1e-6) {
      ecs_delete(it->world, it->entities[i]);
    }
  }
}

void DamageAll(ecs_iter_t *it) {
  Killable *killable = ecs_field(it, Killable, 0);
  for (int i = 0; i < it->count; ++i) {
    killable[i].health -= 100.0f * it->delta_time;
  }
}

static void SpawnExperienceOnEnemyDeath(ecs_iter_t *it) {
  DropsExperience *droppedExperience = ecs_field(it, DropsExperience, 0);

  for (int i = 0; i < it->count; i++) {
    const Position *pos = ecs_get(it->world, it->entities[i], Position);
    ecs_entity_t experience = ecs_new(it->world);
    ecs_set(it->world, experience, Experience, {droppedExperience[i]});
  }
}

int main() {
  ecs_world_t *world = ecs_init();

  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Killable);
  ECS_COMPONENT_DEFINE(world, Damage);
  ECS_COMPONENT_DEFINE(world, DropsExperience);
  ECS_COMPONENT_DEFINE(world, Experience);
  ECS_COMPONENT_DEFINE(world, Collidable);
  ECS_COMPONENT_DEFINE(world, CollidesWith);
  ecs_add_id(world, ecs_id(CollidesWith), EcsSymmetric);
  ecs_add_id(world, ecs_id(CollidesWith), EcsRelationship);

  ECS_SYSTEM(world, KillHealthless, EcsPreStore, Killable);
  ECS_SYSTEM(world, DamageAll, EcsPostUpdate, [inout] Killable);

  ECS_SYSTEM(
      world, Collide,
      EcsOnValidate, [in] Collidable, [in] Position, [out] CollidesWith());

  ECS_OBSERVER(world, SpawnExperienceOnEnemyDeath, EcsOnRemove,
               DropsExperience);

  q_collidables = ecs_query(
      world, {.expr = "[in] Collidable, [in] Position, [out] CollidesWith()"});
  for (int i = 0; i < 100; ++i) {
    CreateEnemy(world);
  }

  ecs_entity_t player = ecs_entity(world, {.name = "Player"});
  ecs_set(world, player, Collidable, {.radius = 20});

  while (true) ecs_progress(world, 1.0f);
  ecs_fini(world);

  return 0;
}
