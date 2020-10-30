#ifndef GUI_H
#define GUI_H

#include "global.h"

lid GUIAddButton(void(*callback)(void*), void* callbackData, char* string, lid texture, li32 xstart, li32 xend, li32 ystart, li32 yend);
void GUIUpdatNeighbors();
void GUITick();
void GUIDraw();
void GUIClear();

#endif
