#include "gui.h"

#include "memory.hpp"
#include "maths.hpp"
#include "graphics.hpp"
#include "archive.hpp"
#include "input.h"
#include "audio.hpp"
#include "cfg.h"

struct Button {
	// Position
	li32 xs, xe, ys, ye;
	// Directional neighbors
	lid nbUp = ID_NULL;
	lid nbDn = ID_NULL;
	lid nbLt = ID_NULL;
	lid nbRt = ID_NULL;
	// Callback function to use when the button is activated
	void(*callback)(void*) = nullptr;
	void* callbackData = nullptr;
	graphics::GUIBox box;
	graphics::GUIText text;
	lid boxTxtr;
	lid textTxtr;
};
mem::Buffer32<Button> buttons;
lid buttonSelected = 0;

lid GUIAddButton(void(*callback)(void*), void* callbackData, char* string, lid texture, li32 xstart, li32 xend, li32 ystart, li32 yend) {
	Button button;
	button.box.Init();
	button.box.ReGen(xstart, xend, ystart, yend, 4, 2);
	button.text.Init();
	button.text.ReGen(string, (xstart + xend) / 2, xend + 512, yend - 7, graphics::eTEXTALIGN_MID);
	button.callback = callback;
	button.callbackData = callbackData;
	button.xs = xstart;
	button.xe = xend;
	button.ys = ystart;
	button.ye = yend;
	button.boxTxtr = texture;
	button.textTxtr = acv::t_gui_font;
	return buttons.Add(button);
}
void GetCenter(Button* b, li32* out_x, li32* out_y) {
	*out_x = (b->xs + b->xe) / 2;
	*out_y = (b->ys + b->ye) / 2;
}
void NbTest(lui32 i, lui32 j, lf32 distO, li32 x, li32 y, lid* nb) {
	// If neighbor already seen
	if (*nb != ID_NULL) {
		// Get existing neighbor's center
		li32 xN, yN; GetCenter(&buttons[*nb], &xN, &yN);
		// Assign new neighbor if closer
		if (distO < m::Length(m::Vector2(abs(xN - x), abs(yN - y)))) *nb = j;
	}
	else *nb = j; // Otherwise just assign neighbor
}
void GUIUpdatNeighbors() {
	// For every button
	for (int i = 0; i < buttons.Size(); ++i) {
		if (!buttons.Used(i)) continue;
		// Get this button's center
		li32 x, y; GetCenter(&buttons[i], &x, &y);
		// For every other button
		for (int j = 0; j < buttons.Size(); ++j) {
			if (j == i || !buttons.Used(j)) continue;
			// Get other button's center
			li32 xO, yO; GetCenter(&buttons[j], &xO, &yO);
			lf32 distO = m::Length(m::Vector2(abs(xO - x), abs(yO - y)));
			// if more to the UD direction
			if (abs(yO - y) > abs(xO - x)) {
				if (yO > y) NbTest(i, j, distO, x, y, &buttons[i].nbUp);
				if (yO < y) NbTest(i, j, distO, x, y, &buttons[i].nbDn);
			}
			// if more to the LR direction
			else {
				if (xO > x) NbTest(i, j, distO, x, y, &buttons[i].nbRt);
				if (xO < x) NbTest(i, j, distO, x, y, &buttons[i].nbLt);
			}
		}
	}
}
void GUITick() {
	#if DEF_NMP
	// Allow any player to use the gui
	for (lui32 i = 0; i < config.iNumNWPlayers; ++i) {
		// Check for button activations and use callbacks
		if (input::GetHit(i, input::key::ACTIVATE) || input::GetHit(i, input::key::JUMP)) {
			buttons[buttonSelected].callback(buttons[buttonSelected].callbackData);
			aud::PlaySnd(aud::FILE_GUI_ACCEPT);
		}
		// Check for directional inputs
		if (input::GetHit(i, input::key::DIR_F) && buttons[buttonSelected].nbUp != ID_NULL) {
			buttonSelected = buttons[buttonSelected].nbUp;
			aud::PlaySnd(aud::FILE_GUI_BEEP);
		}
		if (input::GetHit(i, input::key::DIR_B) && buttons[buttonSelected].nbDn != ID_NULL) {
			buttonSelected = buttons[buttonSelected].nbDn;
			aud::PlaySnd(aud::FILE_GUI_BEEP);
		}
		if (input::GetHit(i, input::key::DIR_R) && buttons[buttonSelected].nbRt != ID_NULL) {
			buttonSelected = buttons[buttonSelected].nbRt;
			aud::PlaySnd(aud::FILE_GUI_BEEP);
		}
		if (input::GetHit(i, input::key::DIR_L) && buttons[buttonSelected].nbLt != ID_NULL) {
			buttonSelected = buttons[buttonSelected].nbLt;
			aud::PlaySnd(aud::FILE_GUI_BEEP);
		}
	}
	#else
	// Check for button activations and use callbacks
	if (input::GetHit(input::key::ACTIVATE) || input::GetHit(input::key::JUMP)) {
		buttons[buttonSelected].callback(buttons[buttonSelected].callbackData);
		aud::PlaySnd(aud::FILE_GUI_ACCEPT);
	}
	// Check for directional inputs
	if (input::GetHit(input::key::DIR_F) && buttons[buttonSelected].nbUp != ID_NULL) {
		buttonSelected = buttons[buttonSelected].nbUp;
		aud::PlaySnd(aud::FILE_GUI_BEEP);
	}
	if (input::GetHit(input::key::DIR_B) && buttons[buttonSelected].nbDn != ID_NULL) {
		buttonSelected = buttons[buttonSelected].nbDn;
		aud::PlaySnd(aud::FILE_GUI_BEEP);
	}
	if (input::GetHit(input::key::DIR_R) && buttons[buttonSelected].nbRt != ID_NULL) {
		buttonSelected = buttons[buttonSelected].nbRt;
		aud::PlaySnd(aud::FILE_GUI_BEEP);
	}
	if (input::GetHit(input::key::DIR_L) && buttons[buttonSelected].nbLt != ID_NULL) {
		buttonSelected = buttons[buttonSelected].nbLt;
		aud::PlaySnd(aud::FILE_GUI_BEEP);
	}
	#endif
}
void GUIDraw() {
	for (int i = 0; i < buttons.Size(); ++i) {
		if (!buttons.Used(i)) continue;
		buttons[i].box.Draw(&acv::GetT(buttons[i].boxTxtr));
		buttons[i].text.Draw(&acv::GetT(buttons[i].textTxtr));
		if (buttonSelected == i)
			graphics::DrawGUIBox(&acv::GetT(acv::t_gui_select_box), buttons[i].xs, buttons[i].xe, buttons[i].ys, buttons[i].ye, 8, 6);
	}
}
void GUIClear() {
	for (int i = 0; i < buttons.Size(); ++i) {
		if (!buttons.Used(i)) continue;
		buttons[i].box.End();
		buttons[i].text.End();
		buttons.Remove(i);
	}
}
