#include "archiver_loop.h"

//-------------------------------- WINDOWING GLOBAL VARIABLES

//SDL_Window* sdl_window;
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
	//-------------------------------- INITIALIZE SDL2

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) != 0)
		return 0;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // Request OpenGL 3.3 'context'
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // I don't know what this does
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Turn on double buffering

	sdl_window = SDL_CreateWindow("TSOA ARCHIVE EDITOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		ARCHIVER_WINDOW_W, ARCHIVER_WINDOW_H, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); // Create window
	if (!sdl_window) return -1; // Die if creation failed

	sdl_glcontext = SDL_GL_CreateContext(sdl_window); // Create our opengl context and attach it to our window

	SDL_GL_SetSwapInterval(1); // This makes our buffer swap syncronized with the monitor's vertical refresh

	//-------------------------------- INITIALIZE GLAD

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) // Load all OpenGL function pointers
	{
		std::cout << "ERROR: gladLoadGLLoader failed!" << std::endl;
		return -1;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, ARCHIVER_WINDOW_W, ARCHIVER_WINDOW_H); // Set opengl viewport size to window size X, Y, W, H

	//-------------------------------- MAIN LOOP

	//*

	graphics::Init();
	res::Init();

	while (true)
	{
		if (!ArchiverGUITick())
			break;
		//ArchiverGUITick();
	}

	res::End();
	graphics::End();

	//*/

	//-------------------------------- OTHER SHIT

	//RunSerializer();

	//-------------------------------- END PROGRAM

exit:
	SDL_GL_DeleteContext(sdl_glcontext);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();

	return 0; // Goodbye
}