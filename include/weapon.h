#pragma once
#include <stdlib.h>

#include "level.h"
#include "player.h"
#include "raylib.h"
#include "weapon_type.h"

#define DEFAULT_BULLET_CAPACITY 100

Weapon InitialWeapon();

void TickWeapon(Weapon *weapon, Player *player);
void DrawWeapon(Weapon *weapon);