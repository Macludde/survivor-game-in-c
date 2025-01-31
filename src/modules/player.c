#include "modules/player.h"

#include "flecs.h"
#include "helpers.h"
#include "modules/collisions.h"
#include "modules/health.h"
#include "modules/movement.h"
#include "modules/render.h"

ECS_DECLARE(PlayerTeam);
ECS_COMPONENT_DECLARE(Player);
ECS_COMPONENT_DECLARE(DropsExperience);
ECS_COMPONENT_DECLARE(Experience);

ECS_SYSTEM_DECLARE(PickUpExperience);
ECS_SYSTEM_DECLARE(DrawLevel);

static int ExperienceRequiredToLevelUp(int level) { return 100 * level; }

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
  static ecs_entity_t experiencePrefrab = 0;

  if ((ecs_should_quit(it->world))) return;

  if (!experiencePrefrab) {
    experiencePrefrab = ecs_new(it->world);
    ecs_set(it->world, experiencePrefrab, RectShape,
            {
                .offset = {0, 0},
                .size = {10, 10},
                .color = GOLD,
            });
    ecs_set(it->world, experiencePrefrab, Rotation, {45 * DEG2RAD});
    ecs_set(it->world, experiencePrefrab, Collidable, {.radius = 7});
  }
  DropsExperience *droppedExperience = ecs_field(it, DropsExperience, 0);
  Position *position = ecs_field(it, Position, 1);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t experience =
        ecs_new_w_pair(it->world, EcsIsA, experiencePrefrab);
    ecs_set(it->world, experience, Position, {position[i].x, position[i].y});
    ecs_set(it->world, experience, Despawn, DESPAWN_IN(15));
    ecs_set(it->world, experience, Experience, {droppedExperience[i]});
  }
}

static void DrawLevel(ecs_iter_t *it) {
  Player *player = ecs_field(it, Player, 0);
  Position *pos = ecs_field(it, Position, 1);

  for (int i = 0; i < it->count; i++) {
    DrawText(TextFormat("Level: %d", player[i].level), pos[i].x, pos[i].y - 20,
             20, BLACK);
    DrawText(TextFormat("Experience: %0.f", player[i].experience), pos[i].x,
             pos[i].y - 40, 20, BLACK);
  }
}

void PlayersImport(ecs_world_t *world) {
  ECS_MODULE(world, Players);
  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Collisions);

  ECS_TAG_DEFINE(world, PlayerTeam);

  ECS_COMPONENT_DEFINE(world, Player);
  ECS_COMPONENT_DEFINE(world, DropsExperience);
  ECS_COMPONENT_DEFINE(world, Experience);
  ecs_add_pair(world, ecs_id(Player), EcsWith, PlayerTeam);

  ECS_SYSTEM_DEFINE(
      world, PickUpExperience, EcsOnUpdate, [out] Player($this),
      collisions.CollidesWith($this, $other), [out] Experience($other));
  ECS_SYSTEM_DEFINE(world, DrawLevel, EcsOnStore, [in] Player,
                    movement.Position);

  ECS_OBSERVER(world, SpawnExperienceOnEnemyDeath, EcsOnRemove,
               DropsExperience, [in] movement.Position);
}