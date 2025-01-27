#include "enemy.h"
#include "level.h"
#include "player.h"

// has some elastic collisions
void HandleAllEnemyCollisions(Enemy *allEnemies, int enemyCount,
                              OLD_Player *player);