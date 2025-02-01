#include "modules/player.h"

#include "flecs.h"
#include "modules/collisions.h"
#include "modules/health.h"
#include "modules/movement.h"

static ECS_PREFAB_DECLARE(ExperiencePrefab);

ECS_DECLARE(PlayerTeam);
ECS_COMPONENT_DECLARE(Player);
ECS_COMPONENT_DECLARE(DropsExperience);
ECS_COMPONENT_DECLARE(Experience);

ECS_SYSTEM_DECLARE(PickUpExperience);
ECS_OBSERVER_DECLARE(SpawnExperienceOnEnemyDeath);

int ExperienceRequiredToLevelUp(int level) { return 100 * level; }

static void LevelUpIfPossible(Player *player) {
  if (player->experience >= ExperienceRequiredToLevelUp(player->level)) {
    player->experience = -ExperienceRequiredToLevelUp(player->level);
    player->level++;
  }
}
static void PickUpExperience(ecs_iter_t *it) {
  Player *player = ecs_field(it, Player, 0);
  Experience *experience = ecs_field(it, Experience, 2);
  ecs_entity_t experienceEntity = it->variables[1].entity;

  for (int i = 0; i < it->count; i++) {
    player[i].experience += experience[i];
    LevelUpIfPossible(&player[i]);
  }
  ecs_delete(it->world, experienceEntity);
}

static void SpawnExperienceOnEnemyDeath(ecs_iter_t *it) {
  if ((ecs_should_quit(it->world))) return;

  DropsExperience *droppedExperience = ecs_field(it, DropsExperience, 0);

  for (int i = 0; i < it->count; i++) {
    const Position *pos = ecs_get(it->world, it->entities[i], Position);
    // there is a bug/issue here: this method is called when an entity is
    // deleted. That entity's ID is freed, so this ecs_new call returns the
    // deleted entity's ID
    // This causes a crash when we try to add
    ecs_entity_t experience = ecs_new(it->world);
    ecs_add_pair(it->world, experience, EcsIsA, ExperiencePrefab);
    ecs_set(it->world, experience, Position, {pos->x, pos->y});
    ecs_set(it->world, experience, Experience, {droppedExperience[i]});
  }
}

void PlayersImport(ecs_world_t *world) {
  ECS_MODULE(world, Players);
  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Collisions);

  ECS_COMPONENT_DEFINE(world, DropsExperience);
  ECS_COMPONENT_DEFINE(world, Experience);

  ECS_OBSERVER_DEFINE(world, SpawnExperienceOnEnemyDeath, EcsOnRemove,
                      DropsExperience);

  ECS_PREFAB_DEFINE(world, ExperiencePrefab, movement.Rotation,
                    collisions.Collidable);
  ecs_set(world, ExperiencePrefab, Rotation, {45 * DEG2RAD});
  ecs_set(world, ExperiencePrefab, Collidable, {.radius = 15});
}