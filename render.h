#ifndef LTRREN_H
#define LTRREN_H

#include "global.h"

#if DEF_SWR

#include "maths.hpp"
#include "graphics.hpp"

#define TILE_P_SIZE 128
//#define TILE_P_SIZE 64

#define Y_MULT 0.75
#define Z_OFFSET_MULT 0.661437809

void RenderDrawMesh(graphics::Mesh* mesh, graphics::Texture* txtr, graphics::Matrix4x4 matr);

void RenderInit();

void RenderClear();

void RenderTexture(li32 x, li32 y, graphics::Texture* tx);
void RenderSprite(li32 x, li32 y, graphics::Texture* tx, lui32 spritesize, lui32 num);

void RenderPrint();

#endif

#endif // !LTRREN_H
