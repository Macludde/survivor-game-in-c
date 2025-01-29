#include "player.h"

#include "enemy.h"
#include "helpers.h"
#include "level.h"
#include "physics_math.h"
#include "raylib.h"
#include "raymath.h"

extern EnemySpawner enemySpawner;
extern Level level;

OLD_Player InitialPlayer() {
  return (OLD_Player){
      .entity =
          (Entity){
              .body =
                  (PhysicsBody){
                      .pos = Vector2Zero(),
                      .velocity = Vector2Zero(),
                      .acceleration = Vector2Zero(),
                      .mass = 400,
                      .radius = 20,
                  },
              .health = 100,
              .type = ENTITY_TYPE_PLAYER,
          },
      .stats =
          (PlayerStats){
              .damage = 1.0f,
              .bulletSpeed = 1.0f,
              .movementSpeed = 1.0f,
              .range = 1.0f,
              .attackSpeed = 1.0f,
          },
  };
}

Vector2 GetDesiredVeloctiy() {
  Vector2 delta = Vector2Zero();
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) delta.y -= 1;
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) delta.y += 1;

  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) delta.x -= 1;
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) delta.x += 1;
  // we cast float to int. Because length is either 0 (0,0), 1 (vertical or
  // horizontal) or 2 (diagonal)
  int lengthSquared = Vector2LengthSqr(delta);  // faster than length, no sqrt
  if (lengthSquared == 0) return delta;
  if (lengthSquared == 1) return delta;
  return Vector2Normalize(delta);
}

#define DEFAULT_PLAYER_MOVEMENT_SPEED 200
void MovePlayer(OLD_Player *player) {
  Vector2 force =
      Vector2Scale(GetDesiredVeloctiy(),
                   DEFAULT_PLAYER_MOVEMENT_SPEED * player->stats.movementSpeed);
  if (Vector2LengthSqr(force) != 0)
    ApplyAcceleration(&player->entity.body, force);
  // MoveBodyWithWeights(&player->entity.body, ACCELERATION_SPEED,
  //                     RETARDATION_SPEED);
}

void TickPlayer(OLD_Player *player) { MovePlayer(player); }

void DrawPlayer(OLD_Player *player) {
  DrawCircleV(player->entity.body.pos, player->entity.body.radius,
              PLAYER_COLOR);
}