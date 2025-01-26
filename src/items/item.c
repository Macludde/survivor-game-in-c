#include "item.h"

#include "lib/stb_ds.h"

PlayerItem *InitializeFlameGrenade();
void FreeFlameGrenade();

Items items;

void InitializeItems() { items.flameGrenade = InitializeFlameGrenade(); }

void FreeItems() { FreeFlameGrenade(); }

void ItemsCallBeforeTick(Player *player) {
  for (int i = 0; i < arrlen(player->items); ++i) {
    if (player->items[i]->beforeTick == NULL) continue;
    player->items[i]->beforeTick(player);
  }
}
void ItemsCallAfterTick(Player *player) {
  for (int i = 0; i < arrlen(player->items); ++i) {
    if (player->items[i]->afterTick == NULL) continue;
    player->items[i]->afterTick(player);
  }
}
void ItemsCallDrawBackground(Player *player) {
  for (int i = 0; i < arrlen(player->items); ++i) {
    if (player->items[i]->drawBackground == NULL) continue;
    player->items[i]->drawBackground(player);
  }
}
void ItemsCallDrawForeground(Player *player) {
  for (int i = 0; i < arrlen(player->items); ++i) {
    if (player->items[i]->drawForeground == NULL) continue;
    player->items[i]->drawForeground(player);
  }
}