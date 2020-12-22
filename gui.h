#ifndef GUI_H
#define GUI_H

#include "global.h"

ID16 GUIAddButton(void(*callback)(void*), void* callbackData, char* string, ID16 texture, li32 xstart, li32 xend, li32 ystart, li32 yend);
void GUIUpdatNeighbors();
void GUITick();
void GUIDraw();
void GUIClear();

#endif
