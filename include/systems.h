#pragma once

#include "flecs.h"
#include "systems/gui.h"
#include "systems/move_camera_to_player.h"

// Here all systems which are not part of a module are set-up. All modules are
// assumed to have been imported when called
inline static void SetupSystems(ecs_world_t *world) {
  ECS_SYSTEM(world, MoveCameraToPosition, EcsPostUpdate, movement.Position,
             [in] players.Player);  // to player

  ECS_SYSTEM(world, DrawExperienceBar, EcsPostFrame, [in] players.Player);
}