#include "modules/render.h"

#include "flecs.h"
#include "modules/camera.h"
#include "modules/collisions.h"
#include "modules/movement.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(CircleShape);
ECS_COMPONENT_DECLARE(RectShape);

ECS_SYSTEM_DECLARE(RenderCollidables);
DECLARE_RENDER_SYSTEM(RenderRect);
DECLARE_RENDER_SYSTEM(RenderCircle);

ECS_DECLARE(BackgroundRenderLayer);
ECS_DECLARE(ForegroundRenderLayer);

static void RenderCollidables(ecs_iter_t *it) {
  Position *p = ecs_field(it, Position, 0);
  Collidable *col = ecs_field(it, Collidable, 1);
  for (int i = 0; i < it->count; ++i) {
    DrawCircleLinesV(p[i], col[i].radius, PINK);
  }
}

static void RenderCircle(ecs_iter_t *it) {
  CircleShape *shape = ecs_field(it, CircleShape, 0);
  Position *p = ecs_field(it, Position, 1);
  for (int i = 0; i < it->count; ++i) {
    Vector2 center = Vector2Add(p[i], shape[i].offset);
    DrawCircleV(center, shape[i].radius, shape[i].color);
  }
}

static void RenderRect(ecs_iter_t *it) {
  RectShape *shape = ecs_field(it, RectShape, 0);
  Position *p = ecs_field(it, Position, 1);
  Rotation *rot = ecs_field(it, Rotation, 2);
  bool hasRotation = rot;
  for (int i = 0; i < it->count; ++i) {
    Vector2 center = Vector2Add(p[i], shape[i].offset);
    Vector2 halfSize = Vector2Scale(shape[i].size, 0.5);
    Rectangle rect = {center.x, center.y, shape[i].size.x, shape[i].size.y};
    float rotation = hasRotation ? rot[i] * RAD2DEG : 0;
    DrawRectanglePro(rect, (Vector2){halfSize.x, halfSize.y}, rotation,
                     shape[i].color);
  }
}

void RenderImport(ecs_world_t *world) {
  ECS_MODULE(world, Render);
  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Collisions);

  ECS_COMPONENT_DEFINE(world, CircleShape);
  ECS_COMPONENT_DEFINE(world, RectShape);

  ECS_TAG_DEFINE(world, BackgroundRenderLayer);
  ECS_TAG_DEFINE(world, ForegroundRenderLayer);

  ECS_SYSTEM_DEFINE(world, RenderCollidables, EcsOnStore, movement.Position,
                    collisions.Collidable);

  // This is horrible code, I just don't know how to do it better
  // I've tried using the macros, but each system needs its own name
  // I've tried using a macro to generate these, simlar to the
  // DECLARE_RENDER_SYSTEM, but I need to declare them in the correct order I've
  // tried creating render layers, using RenderTexture2D but it ended up being
  // even more complex

  ecs_entity_t ecs_id(BGRenderCircle) = ecs_system(
      world, {
                 .entity = ecs_entity(
                     world, {.name = "BGRenderCircle",
                             .add = ecs_ids(ecs_dependson(EcsOnStore))}),
                 .query.expr = "[in] (CircleShape, BackgroundRenderLayer), "
                               "[in] movement.Position",
                 .callback = RenderCircle,
             });
  ecs_entity_t ecs_id(BGRenderRect) = ecs_system(
      world, {
                 .entity = ecs_entity(
                     world, {.name = "BGRenderRect",
                             .add = ecs_ids(ecs_dependson(EcsOnStore))}),
                 .query.expr = "[in] (RectShape, BackgroundRenderLayer), [in] "
                               "movement.Position, ?movement.Rotation",
                 .callback = RenderRect,
             });

  ecs_entity_t ecs_id(RenderCircle) = ecs_system(
      world, {
                 .entity = ecs_entity(
                     world, {.name = "RenderCircle",
                             .add = ecs_ids(ecs_dependson(EcsOnStore))}),
                 .query.expr = "[in] CircleShape, [in] movement.Position",
                 .callback = RenderCircle,
             });
  ecs_entity_t ecs_id(RenderRect) = ecs_system(
      world,
      {
          .entity =
              ecs_entity(world, {.name = "RenderRect",
                                 .add = ecs_ids(ecs_dependson(EcsOnStore))}),
          .query.expr =
              "[in] RectShape, [in] movement.Position, ?movement.Rotation",
          .callback = RenderRect,
      });

  ecs_entity_t ecs_id(FGRenderCircle) = ecs_system(
      world, {
                 .entity = ecs_entity(
                     world, {.name = "FGRenderCircle",
                             .add = ecs_ids(ecs_dependson(EcsOnStore))}),
                 .query.expr = "[in] (CircleShape, ForegroundRenderLayer), "
                               "[in] movement.Position",
                 .callback = RenderCircle,
             });
  ecs_entity_t ecs_id(FGRenderRect) = ecs_system(
      world, {
                 .entity = ecs_entity(
                     world, {.name = "FGRenderRect",
                             .add = ecs_ids(ecs_dependson(EcsOnStore))}),
                 .query.expr = "[in] (RectShape, ForegroundRenderLayer), [in] "
                               "movement.Position, ?movement.Rotation",
                 .callback = RenderRect,
             });
}
