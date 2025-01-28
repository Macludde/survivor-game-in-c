#include "./render.h"

#include "flecs.h"
#include "modules/camera.h"
#include "modules/movement.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(CircleShape);
ECS_COMPONENT_DECLARE(RectShape);

DECLARE_RENDER_SYSTEM(RenderRect);
DECLARE_RENDER_SYSTEM(RenderCircle);

ECS_DECLARE(BackgroundRenderLayer);
ECS_DECLARE(ForegroundRenderLayer);

static ecs_query_t *cam_query;

typedef void (*RenderFn)(ecs_iter_t *it, int i);

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
  for (int i = 0; i < it->count; ++i) {
    Vector2 center = Vector2Add(p[i], shape[i].offset);
    Vector2 halfSize = Vector2Scale(shape[i].size, 0.5);
    Vector2 topLeft = Vector2Subtract(center, halfSize);
    Rectangle rect = {topLeft.x, topLeft.y, shape[i].size.x, shape[i].size.y};
    if (shape[i].rotation == 0) {
      DrawRectangleRec(rect, shape[i].color);
    } else {
      DrawRectanglePro(rect, (Vector2){halfSize.x, halfSize.y},
                       shape[i].rotation * RAD2DEG, shape[i].color);
    }
  }
}

void RenderImport(ecs_world_t *world) {
  ECS_MODULE(world, Render);
  ECS_IMPORT(world, Movement);

  ECS_COMPONENT_DEFINE(world, CircleShape);
  ECS_COMPONENT_DEFINE(world, RectShape);

  ECS_TAG_DEFINE(world, BackgroundRenderLayer);
  ECS_TAG_DEFINE(world, ForegroundRenderLayer);

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
                               "movement.Position",
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
      world, {
                 .entity = ecs_entity(
                     world, {.name = "RenderRect",
                             .add = ecs_ids(ecs_dependson(EcsOnStore))}),
                 .query.expr = "[in] RectShape, [in] movement.Position",
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
                               "movement.Position",
                 .callback = RenderRect,
             });
}
