#ifndef MODELS_H
#define MODELS_H

#include "game.h"
#include "material.h"

void initGround(Backend*, Material*);
void initTree(Backend*, Material*);
void initWolf(Backend*, Material*);
void initSheep(Backend*, Material*);
void initTable(Backend*, Material*, Material*);
void initTorch(Backend*, Material*, Material*);
void initSign(Backend*, Material*);
void initTrap(Backend*, Material*);
void initSafeZone(Backend*, Material*);

#endif
