#include "systems/gui.h"

#include "flecs.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "raylib.h"
#include "raymath.h"

void DrawExperienceBar(ecs_iter_t *it) {
  static int centerX;
  static int barWidth;
  static float width;
  if (centerX == 0 || IsWindowResized()) {
    centerX = GetScreenWidth() / 2;
    barWidth = GetScreenWidth() * 0.4f;
  }
  Player *player = ecs_field(it, Player, 0);

  Rectangle backgroundRec = {
      centerX - barWidth / 2,
      20,
      barWidth,
      20,
  };
  float desiredWidth = barWidth * (player->experience /
                                   ExperienceRequiredToLevelUp(player->level));
  if (width != desiredWidth) {
    if (desiredWidth < width) width = 0;
    width += fminf(desiredWidth - width, barWidth * 0.2f * it->delta_time);
  }
  Rectangle filledRec = {
      centerX - barWidth / 2,
      20,
      width,
      20,
  };
  DrawRectangleRounded(backgroundRec, 2.0f, 8, BLACK);
  DrawRectangleRounded(filledRec, 2.0f, 8, DARKGREEN);

  char *levelText = TextFormat("Level %d", player->level);
  DrawText(levelText, centerX - MeasureText(levelText, 20) / 2, 20, 20, BLUE);
}
