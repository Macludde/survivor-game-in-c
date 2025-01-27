#include "./render.h"

#include "flecs.h"
#include "modules/movement.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(CircleShape);
ECS_COMPONENT_DECLARE(RectShape);
ECS_COMPONENT_DECLARE(Rotation);

ECS_SYSTEM_DECLARE(RenderCircle);
ECS_SYSTEM_DECLARE(RenderRect);
ECS_SYSTEM_DECLARE(RenderRotatedRect);

static void RenderCircle(ecs_iter_t *it) {
  // Get fields from system query
  CircleShape *shape = ecs_field(it, CircleShape, 0);
  Position *p = ecs_field(it, Position, 1);

  for (int i = 0; i < it->count; ++i)
    DrawCircleV(Vector2Add(p[i], shape[i].offset), shape[i].radius,
                shape[i].color);
}

static void RenderRect(ecs_iter_t *it) {
  // Get fields from system query
  RectShape *shape = ecs_field(it, RectShape, 0);
  Rotation *r = ecs_field(it, Rotation, 1);
  Position *p = ecs_field(it, Position, 2);
  for (int i = 0; i < it->count; ++i) {
    Vector2 center = Vector2Add(p[i], shape[i].offset);
    Rectangle rect = {center.x, center.y, shape[i].size.x, shape[i].size.y};
    if (r[i] == 0)
      DrawRectangleRec(rect, shape[i].color);
    else
      DrawRectanglePro(rect,
                       (Vector2){shape[i].size.x / 2, shape[i].size.y / 2},
                       r[i] * RAD2DEG, shape[i].color);
  }
}

static void RenderRotatedRect(ecs_iter_t *it) {
  // Get fields from system query
  RectShape *shape = ecs_field(it, RectShape, 0);
  Rotation *r = ecs_field(it, Rotation, 1);
  Position *p = ecs_field(it, Position, 2);
  for (int i = 0; i < it->count; ++i) {
    Vector2 center = Vector2Add(p[i], shape[i].offset);
    Rectangle rect = {center.x, center.y, shape[i].size.x, shape[i].size.y};
    if (r[i] == 0)
      DrawRectangleRec(rect, shape[i].color);
    else
      DrawRectanglePro(rect,
                       (Vector2){shape[i].size.x / 2, shape[i].size.y / 2},
                       r[i] * RAD2DEG, shape[i].color);
  }
}

void RenderImport(ecs_world_t *world) {
  ECS_MODULE(world, Render);
  ECS_IMPORT(world, Movement);
  ECS_COMPONENT_DEFINE(world, CircleShape);
  ECS_COMPONENT_DEFINE(world, RectShape);
  ECS_COMPONENT_DEFINE(world, Rotation);

  ECS_SYSTEM_DEFINE(world, RenderCircle, EcsOnStore,
                    CircleShape, [in] movement.Position);
  ECS_SYSTEM_DEFINE(world, RenderRect, EcsOnStore, RectShape,
                    !Rotation, [in] movement.Position);
  ECS_SYSTEM_DEFINE(world, RenderRotatedRect, EcsOnStore, RectShape,
                    Rotation, [in] movement.Position);
}
