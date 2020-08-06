// SDL
#define SDL_MAIN_HANDLED
#include "SDL2\SDL.h"

#include "archive_read_write.h"
#include "archive.hpp"
#include "graphics.hpp"

// OpenGL
#include "glad/glad.h"

btui64 tickCount;

SDL_Event e; // Input event

bool redraw = false;

SDL_Window* sdl_window;

struct AGUIList
{
	int i;
};

bool ArchiverGUIHandleInput(SDL_Event* e)
{
	switch (e->type)
	{
	case SDL_QUIT:
	case SDL_APP_LOWMEMORY:
	case SDL_APP_TERMINATING:
		return false;
		break;
	}

	redraw = true;

	return true;
}

void ArchiverGUIRedraw()
{
	//printf("redrawing!\n");

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	graphics::DrawGUIBox(&acv::GetT(acv::t_gui_box), -64, 64, -32, 32, 4, 0);
	//graphics::DrawGUIText("hewwo world", &acv::GetT(acv::t_gui_font), -64, 64, 32);

	char string[64];

	for (int i = 0; i < acv::prop_index; ++i)
	{
		//itoa(acv::props[i].idMesh, string);
		snprintf(string, 64, "ID mesh: %i\nID txtr: %i", acv::props[i].idMesh, acv::props[i].idTxtr);
		graphics::DrawGUIText(string, &acv::GetT(acv::t_gui_font), -64, 64, (ARCHIVER_WINDOW_H / 2) - (i * 24));
	}

	SDL_GL_SwapWindow(sdl_window);
}

bool ArchiverGUITick()
{
	bool rt = true;
	while (SDL_PollEvent(&e)) rt = ArchiverGUIHandleInput(&e);

	if (redraw)
	{
		ArchiverGUIRedraw();
		redraw = false;
	}

	return rt;
}