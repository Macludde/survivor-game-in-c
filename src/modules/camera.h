#pragma once
#include <stdbool.h>

#include "flecs.h"
#include "raylib.h"

#define CAMERA_MOVEMENT_SPEED 2.0f

// declare the Component type
typedef Camera2D FollowCam;

extern ECS_COMPONENT_DECLARE(FollowCam);

extern ECS_SYSTEM_DECLARE(MoveCameraToPosition);
extern ECS_SYSTEM_DECLARE(Begin2D);
extern ECS_SYSTEM_DECLARE(End2D);
#ifdef DEBUG
extern ECS_SYSTEM_DECLARE(MouseScrollZoom);
#endif

void CameraImport(ecs_world_t *world);

bool IsPointOnScreen(Vector2 pos);
bool IsCircleOnScreen(Vector2 pos, float radius);
bool IsRectOnScreen(Vector2 pos, Vector2 size);