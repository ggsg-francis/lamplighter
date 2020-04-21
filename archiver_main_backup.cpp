#include "archive_read_write.h"
#include "archive.hpp"

// SDL
#define SDL_MAIN_HANDLED
#include "SDL2\SDL.h"

//-------------------------------- WINDOWING GLOBAL VARIABLES

SDL_Window* sdl_window;
SDL_GLContext sdl_glcontext;
bool focus = true;

void RunSerializer()
{
	serializer::convert_files_src("0000gameassets.uwu");
	serializer::InterpretArchiveContents("0000gamecontents.uwu");
	serializer::SaveArchive("res/archive.UwUa");
	acv::ClearMemory();
}

int main(int argc, char * argv[])
{
	RunSerializer();
	return 0; // Goodbye
}