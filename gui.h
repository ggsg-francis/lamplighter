#ifndef GUI_H
#define GUI_H

#include "global.h"

btID GUIAddButton(void(*callback)(void*), void* callbackData, char* string, btID texture, bti32 xstart, bti32 xend, bti32 ystart, bti32 yend);
void GUIUpdatNeighbors();
void GUITick();
void GUIDraw();
void GUIClear();

#endif
