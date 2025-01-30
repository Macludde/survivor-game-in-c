#include "./collisions.h"

#include "./movement.h"
#include "./physics.h"
#include "flecs.h"
#include "physics_math.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(Collidable);
ECS_COMPONENT_DECLARE(CollidesWith);

ECS_SYSTEM_DECLARE(Collide);

// TODO: use a spatial grid or BVH to improve performance of collision detection

void Collide(ecs_iter_t *it) {
  ecs_query_t *q_collide = it->ctx;  // Get query from system context
  const Collidable *c1 = ecs_field(it, Collidable, 0);
  const Position *p1 = ecs_field(it, Position, 1);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e1 = it->entities[i];

    // For each matching entity, iterate the query
    ecs_iter_t qit = ecs_query_iter(it->world, q_collide);
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
          Vector2 normal = SafeNormalize(Vector2Subtract(p1[i], p2[j]));
          ecs_set_pair(it->world, e1, CollidesWith, e2, {normal});
        }
      }
    }
  }
}

void CollisionsImport(ecs_world_t *world) {
  ECS_MODULE(world, Collisions);

  ECS_IMPORT(world, Movement);

  ECS_COMPONENT_DEFINE(world, Collidable);
  ECS_COMPONENT_DEFINE(world, CollidesWith);

  // ecs_add_id(world, ecs_id(CollidesWith), EcsSymmetric);
  ecs_add_id(world, ecs_id(CollidesWith), EcsRelationship);

  ecs_query_t *q_position =
      ecs_query(world, {.expr = "[in] Collidable, [in] movement.Position"});

  ecs_entity_t ecs_id(Collide) = ecs_system(
      world,
      {
          .entity = ecs_entity(world,
                               {
                                   .name = "Collide",
                                   .add = ecs_ids(ecs_dependson(EcsOnValidate)),
                               }),
          .query.expr = "[in] Collidable, [in] movement.Position",
          .callback = Collide,
          .ctx = q_position,
      });
}