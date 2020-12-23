#ifndef GUI_H
#define GUI_H

#include "global.h"

enum GUIButtonType : lui8 {
	eGUI_BUTTON,
	eGUI_BUTTON_GETKEY, // Returns a key input
};
ID16 GUIAddButton(GUIButtonType type, void(*callback)(void*, void*), void* callbackData, char* string,
	ID16 texture, li32 xstart, li32 xend, li32 ystart, li32 yend);
void GUISetButtonText(ID16 button, char* string);
void GUIUpdatNeighbors();
void GUITick();
void GUIDraw();
void GUIClear();

#endif
