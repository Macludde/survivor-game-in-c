#pragma once

#include "flecs.h"

typedef struct Player {
  int level;
  float experience;
} Player;

typedef float DropsExperience;
typedef float Experience;

int ExperienceRequiredToLevelUp(int level);

extern ECS_DECLARE(PlayerTeam);
extern ECS_COMPONENT_DECLARE(Player);
extern ECS_COMPONENT_DECLARE(DropsExperience);
extern ECS_COMPONENT_DECLARE(Experience);

extern ECS_SYSTEM_DECLARE(PickUpExperience);

void PlayersImport(ecs_world_t *world);