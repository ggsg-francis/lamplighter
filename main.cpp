// ctrl-r, ctrl-w for showing line indentation

// memory leak detection
#ifdef _DEBUG
//#include "vld.h"
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||| STYLE GUIDE |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// FOR BROAD CATEGORIZATION
//________________________________________________________________________________________________________________________________
// BIG COMMENT -------------------------------------------------------------------------------------------------------------------

// ANYTHING SMALLER / HEADER ZONING
//-------------------------------- LITTLE COMMENT

// Littler comment

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#ifndef DEF_NMP
#include <time.h> // For the random seed
#endif

// OpenGL
#include "glad/glad.h"
#include "glm/glm.hpp"
//gtx is experimental, gtc is stable
#include "glm/gtc\matrix_transform.hpp"
#include "glm/gtc\type_ptr.hpp"
//for transform rotation
#include "glm/gtc\quaternion.hpp"

// SDL
#ifdef __cplusplus
extern "C" {
	#endif
	#include <SDL2\SDL.h>
	#include <SDL2\SDL_syswm.h>
	#ifdef __cplusplus
}
#endif

//engine
//try not to include so many things!
#include "graphics.hpp"
#include "maths.hpp"
#include "archive.hpp"
#include "input.h"
#include "cfg.h"
#include "network.h"
#include "audio.hpp"
#include "core.h"
#include "weather.h"
#include "test_zone.h"

//-------------------------------- TEMP

btf32 wipe_time = 0.f;

//-------------------------------- WINDOWING GLOBAL VARIABLES

SDL_Window* sdl_window;
SDL_GLContext sdl_glcontext;
bool focus = true;

//-------------------------------- GAME GLOBAL VARIABLES (SHOULD GO ELSEWHERE, PROBABLY)

GLuint quadVAO; // Quad vertex array object, used for framebuffer drawing
GLuint quadVBO; // Quad vertex buffer object, used for framebuffer drawing

GLuint framebuffer_1; // Framebuffer
graphics::Texture rendertexture_1; // Framebuffer render texture, can be sampled
graphics::Texture depthbuffer_1; // Framebuffer depth buffer, can be sampled

GLuint framebuffer_2; // Framebuffer
graphics::Texture rendertexture_2; // Framebuffer render texture, can be sampled
graphics::Texture depthbuffer_2; // Framebuffer depth buffer, can be sampled

GLuint framebuffer_intermediate; // not really sure, 'Second post-processing framebuffer'
graphics::Texture screenTexture; // used by intermediate
GLuint framebuffer_intermediate_downsample; // Downsampled framebuffer for post effects

GLuint framebuffer_shadow; // Shadowmap framebuffer
//GLuint rendertexture_shadow; // Shadowmap rendertexture
graphics::Texture rendertexture_shadow; // Shadowmap rendertexture

//________________________________________________________________________________________________________________________________
// TRANSLATE INPUTS --------------------------------------------------------------------------------------------------------------

bool step_pause = false;

// Convert device input to chara input
inline void TranslateInput()
{
	#if DEF_3PP
	#ifdef DEF_NMP
	for (btID i = 0u; i < NUM_PLAYERS; ++i)
	{
		// Set input
		core::SetPlayerInput((btID)i,
			m::Vector2((btf32)(input::GetHeld(i, input::key::DIR_L) - input::GetHeld(i, input::key::DIR_R)),
			(btf32)(input::GetHeld(i, input::key::DIR_F) - input::GetHeld(i, input::key::DIR_B))),
			input::buf[i][INPUT_BUF_GET].mouse_x * 0.25f, input::buf[i][INPUT_BUF_GET].mouse_y * 0.25f,
			input::GetHeld(i, input::key::USE),
			input::GetHit(i, input::key::USE),
			input::GetHit(i, input::key::USE_ALT),
			input::GetHeld(i, input::key::RUN),
			false,
			input::GetHit(i, input::key::ACTION_A),
			input::GetHit(i, input::key::ACTION_B),
			input::GetHit(i, input::key::ACTION_C),
			input::GetHit(i, input::key::CROUCH),
			input::GetHeld(i, input::key::JUMP));
	}
	#else
	// Set input for player 0
	core::SetPlayerInput(0u,
		m::Vector2(0.f, (btf32)(input::GetHeld(input::key::DIR_F) - input::GetHeld(input::key::DIR_B))),
		(btf32)(input::GetHeld(input::key::DIR_R) - input::GetHeld(input::key::DIR_L)) * 5.f, 0.f,
		input::GetHeld(input::key::USE),
		input::GetHit(input::key::USE),
		input::GetHit(input::key::USE_ALT),
		input::GetHeld(input::key::RUN),
		false,
		input::GetHit(input::key::ACTION_A),
		input::GetHit(input::key::ACTION_B),
		input::GetHit(input::key::ACTION_C),
		input::GetHit(input::key::CROUCH),
		input::GetHeld(input::key::JUMP));
	if (cfg::bSplitScreen)
	{
		// Set input for player 1
		core::SetPlayerInput(1u,
			m::Vector2(-input::BUF_LOCALGET.joy_x_a, -input::BUF_LOCALGET.joy_y_a),
			input::BUF_LOCALGET.joy_x_b * 8.f, input::BUF_LOCALGET.joy_y_b * 8.f,
			input::GetHeld(input::key::C_USE),
			input::GetHit(input::key::C_USE),
			input::GetHit(input::key::C_USE_ALT),
			input::GetHeld(input::key::C_RUN),
			false,
			input::GetHit(input::key::C_ACTION_A),
			input::GetHit(input::key::C_ACTION_B),
			input::GetHit(input::key::C_ACTION_C),
			input::GetHit(input::key::C_CROUCH),
			input::GetHeld(input::key::C_JUMP));
	}
	#endif
	#else
	#ifdef DEF_NMP
	for (btID i = 0u; i < NUM_PLAYERS; ++i)
	{
		// Set input
		core::SetPlayerInput((btID)i,
			m::Vector2((btf32)(input::GetHeld(i, input::key::DIR_L) - input::GetHeld(i, input::key::DIR_R)),
			(btf32)(input::GetHeld(i, input::key::DIR_F) - input::GetHeld(i, input::key::DIR_B))),
			input::buf[i][INPUT_BUF_GET].mouse_x * 0.25f, input::buf[i][INPUT_BUF_GET].mouse_y * 0.25f,
			input::GetHeld(i, input::key::USE),
			input::GetHit(i, input::key::USE),
			input::GetHit(i, input::key::USE_ALT),
			input::GetHeld(i, input::key::RUN),
			false,
			input::GetHit(i, input::key::ACTION_A),
			input::GetHit(i, input::key::ACTION_B),
			input::GetHit(i, input::key::ACTION_C),
			input::GetHit(i, input::key::CROUCH),
			input::GetHeld(i, input::key::JUMP));
	}
	#else
	// Set input for player 0
	core::SetPlayerInput(0u,
		m::Vector2((btf32)(input::GetHeld(input::key::DIR_L) - input::GetHeld(input::key::DIR_R)),
		(btf32)(input::GetHeld(input::key::DIR_F) - input::GetHeld(input::key::DIR_B))),
		input::BUF_LOCALGET.mouse_x * 0.25f, input::BUF_LOCALGET.mouse_y * 0.25f,
		input::GetHeld(input::key::USE),
		input::GetHit(input::key::USE),
		input::GetHit(input::key::USE_ALT),
		input::GetHeld(input::key::RUN),
		false,
		input::GetHit(input::key::ACTION_A),
		input::GetHit(input::key::ACTION_B),
		input::GetHit(input::key::ACTION_C),
		input::GetHit(input::key::CROUCH),
		input::GetHeld(input::key::JUMP));
	if (cfg::bSplitScreen)
	{
		// Set input for player 1
		core::SetPlayerInput(1u,
			m::Vector2(-input::BUF_LOCALGET.joy_x_a, -input::BUF_LOCALGET.joy_y_a),
			input::BUF_LOCALGET.joy_x_b * 8.f, input::BUF_LOCALGET.joy_y_b * 8.f,
			input::GetHeld(input::key::C_USE),
			input::GetHit(input::key::C_USE),
			input::GetHit(input::key::C_USE_ALT),
			input::GetHeld(input::key::C_RUN),
			false,
			input::GetHit(input::key::C_ACTION_A),
			input::GetHit(input::key::C_ACTION_B),
			input::GetHit(input::key::C_ACTION_C),
			input::GetHit(input::key::C_CROUCH),
			input::GetHeld(input::key::C_JUMP));
	}
	#endif
	#endif
}

// Fixed timestep editor tick function
inline void TranslateInputEditor()
{
	if (!step_pause && focus)
	{
		// Generate analogue input from directional keys
		m::Vector2 input_p1(0.f, 0.f);
		btf32 mouserot_mult = 0.25f;
		if (!input::GetHeld(input::key::RUN))
		{
			input_p1.x = input::BUF_LOCALGET.mouse_x * -0.125f;
			input_p1.y = input::BUF_LOCALGET.mouse_y * -0.125f;
			mouserot_mult = 0.f;
		}
		// Set input
		core::SetPlayerInput(0u, input_p1, input::BUF_LOCALGET.mouse_x * mouserot_mult, input::BUF_LOCALGET.mouse_y * mouserot_mult,
			input::GetHit(input::key::USE),
			input::GetHit(input::key::USE),
			input::GetHit(input::key::USE_ALT),
			input::GetHeld(input::key::RUN),
			false,
			input::GetHit(input::key::ACTION_A),
			input::GetHit(input::key::ACTION_B),
			input::GetHit(input::key::ACTION_C),
			input::GetHeld(input::key::CROUCH),
			input::GetHit(input::key::JUMP));

		//do stuff
		core::Tick(FRAME_TIME);
		weather::Tick(FRAME_TIME);
	}
}

//________________________________________________________________________________________________________________________________
// UTIL FUNCTIONS ----------------------------------------------------------------------------------------------------------------

bool MainTick(SDL_Event* e)
{
	aud::Update(FRAME_TIME);

	input::ClearHitsAndDelta();
	while (SDL_PollEvent(e)) input::UpdateInput(e);

	// TODO: think this over, there must be a better way to handle this
	#ifdef DEF_NMP
	if (cfg::bHost)
	{
		network::RecvTCP();
		network::SendInputBuffer();
	}
	else
	{
		network::SendInputBuffer();
		network::RecvTCP();
	}
	#endif

	// If we reach this point, its time to run the tick
	#ifdef DEF_NMP
	input::CycleBuffers();
	// Quit if any player quits
	for (btui32 i = 0; i < NUM_PLAYERS; ++i)
		if (input::GetHit(i, input::key::QUIT)) return false;
	#else
	if (input::GetHit(input::key::QUIT)) return false;
	#endif
	TranslateInput();
	core::Tick((btf32)(FRAME_TIME));
	weather::Tick((btf32)(FRAME_TIME));
	return true;
}

void MainDraw()
{
	// BUFFER 1 (LEFT SCREEN)
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);

	glViewport(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY());
	glm::vec3* fogC = (glm::vec3*)weather::FogColour();
	glClearColor(fogC->r, fogC->g, fogC->b, 1.0f);

	#ifndef DEF_NMP
	//-------------------------------- BUFFER 1 (LEFT SCREEN)
	// Set GL properties for solid rendering
	glEnable(GL_DEPTH_TEST); glDisable(GL_BLEND); glBlendFunc(GL_ONE, GL_ZERO);
	//-------------------------------- RENDER SHADOWMAP
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_shadow);
	glClear(GL_DEPTH_BUFFER_BIT);
	core::SetViewFocus(0u); // Set render POV
	core::Draw(false);
	//-------------------------------- RENDER VIEW
	glViewport(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY());
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	core::SetViewFocus(0u); // Set render POV
	core::Draw(); core::DrawGUI();
	core::TickGUI(); // causes a crash if before drawgui (does it still?)

	//-------------------------------- BUFFER 2 (RIGHT SCREEN)
	if (cfg::bSplitScreen) {
		// Set GL properties for solid rendering (again....)
		glEnable(GL_DEPTH_TEST); glDisable(GL_BLEND); glBlendFunc(GL_ONE, GL_ZERO);
		//-------------------------------- RENDER SHADOWMAP
		glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_shadow);
		glClear(GL_DEPTH_BUFFER_BIT);
		core::SetViewFocus(1u); // Set render POV to second player
		core::Draw(false);
		//-------------------------------- RENDER VIEW
		glViewport(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY());
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_2);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		core::SetViewFocus(1u); // Set render POV to second player
		core::Draw(); core::DrawGUI();
		core::TickGUI(); // causes a crash if before drawgui (does it still?)
	}
	#else
	// Set GL properties for solid rendering
	glEnable(GL_DEPTH_TEST); glDisable(GL_BLEND); glBlendFunc(GL_ONE, GL_ZERO);
	//-------------------------------- RENDER SHADOWMAP
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_shadow);
	glClear(GL_DEPTH_BUFFER_BIT);
	core::SetViewFocus(network::nid); // Set render POV
	core::Draw(false);
	//-------------------------------- RENDER VIEW
	glViewport(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY());
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	core::SetViewFocus(network::nid); // Set render POV
	core::Draw(); core::DrawGUI();
	core::TickGUI(); // causes a crash if before drawgui (does it still?)
	#endif // MP
}

void RegenFramebuffers()
{
	// Left screen
	glGenFramebuffers(1, &framebuffer_1);
	rendertexture_1.InitRenderBuffer(framebuffer_1, graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	#if !DEF_DEPTH_BUFFER_RW
	depthbuffer_1.InitDepthBufferW(framebuffer_1, graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	#else
	depthbuffer_1.InitDepthBufferRW(framebuffer_1, graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	#endif

	// Right screen
	glGenFramebuffers(1, &framebuffer_2);
	rendertexture_2.InitRenderBuffer(framebuffer_2, graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	#if !DEF_DEPTH_BUFFER_RW
	depthbuffer_2.InitDepthBufferW(framebuffer_2, graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	#else
	depthbuffer_2.InitDepthBufferRW(framebuffer_2, graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	#endif
}

void ScreenWipeIn()
{
	// screenwipe stuff (temp)
	/*wipe_time += FRAME_TIME * 1.6f;
	if (wipe_time > 1.f) wipe_time = 1.f;
	mat_fb = glm::rotate(mat_fb, glm::radians(wipe_time * 360.f * 2.f), glm::vec3(0.f, 0.f, 1.f));
	mat_fb = glm::scale(mat_fb, glm::vec3(wipe_time, wipe_time, wipe_time));*/
}

void ScreenWipeOut()
{

}

//________________________________________________________________________________________________________________________________
// MAIN --------------------------------------------------------------------------------------------------------------------------

// handle pre-game loop initialization
bool MainBoiler()
{
	// Test stuff
	InitTest();

	// Load config
	cfg::LoadCfg();

	#ifdef DEF_NMP
	// Connect to server
	if (!network::Init()) return false;
	#endif

	//-------------------------------- INITIALIZE SDL2

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) != 0)
		return false;

	SDL_Joystick* gGameController = NULL; // Game Controller 1 handler

	if (SDL_NumJoysticks() < 1) // Check for joysticks
		printf("Warning: No joysticks connected!\n");
	else { // If there is one
		gGameController = SDL_JoystickOpen(0); // Load joystick
		if (gGameController == NULL) { printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError()); }
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // Request OpenGL 3.3 'context'
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // I don't know what this does
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Turn on double buffering

	sdl_window = SDL_CreateWindow(DEF_PROJECTNAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		cfg::iWinX, cfg::iWinY, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); // Create window
	if (!sdl_window) return false; // Die if creation failed

	sdl_glcontext = SDL_GL_CreateContext(sdl_window); // Create our opengl context and attach it to our window

	switch (cfg::iFullscreen) {
	case 1:
		SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);
		break;
	case 2:
		SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		break;
	}

	SDL_GL_SetSwapInterval(1); // This makes our buffer swap syncronized with the monitor's vertical refresh

	#if defined DEF_INPUT_MOUSE_HIDDEN
	SDL_ShowCursor(false); // Set mouse cursor invisible
	#elif defined DEF_INPUT_MOUSE_1ST_PERSON
	SDL_SetRelativeMouseMode((SDL_bool)true); // Set mouse input to use raw input
	#endif

	//-------------------------------- INITIALIZE GLAD

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) // Load all OpenGL function pointers
	{
		std::cout << "ERROR: gladLoadGLLoader failed!" << std::endl;
		return false;
	}

	glEnable(GL_CULL_FACE); // Enable face culling
	glCullFace(GL_FRONT); // Set culling mode

	return true;
}

int main(int argc, char * argv[]) // SDL Main
{
	if (!MainBoiler()) goto exitnoinit;

	//-------------------------------- INITIALIZE GRAPHICS

	graphics::Init();

	//-------------------------------- WHATEVER THIS IS MY HANDS ARE REALLY COLD RIGHT NOW

	SDL_Event e; // Input event

	//-------------------------------- CREATE EACH SCREEN FRAMEBUFFER

	if (!cfg::bEditMode) RegenFramebuffers();

	// Create shadowbuffer

	glGenFramebuffers(1, &framebuffer_shadow);
	rendertexture_shadow.InitShadowBuffer(framebuffer_shadow);

	// Create second post-processing framebuffer

	glGenFramebuffers(1, &framebuffer_intermediate);
	screenTexture.InitIntermediateTest(framebuffer_intermediate);

	//-------------------------------- SCREEN QUAD (SHOULD GO IN GRAPHICS) FOR DRAWING FRAMEBUFFER

	{
		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions    // texCoords
			-1.f,	1.f,	0.f,	1.f,
			-1.f,	-1.f,	0.f,	0.f,
			1.f,	-1.f,	1.f,	0.f,

			-1.f,	1.f,	0.f,	1.f,
			1.f,	-1.f,	1.f,	0.f,
			1.f,	1.f,	1.f,	1.f
		};
		// screen quad VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}

	//-------------------------------- INITIALIZATION

	#ifndef DEF_NMP
	srand(time(NULL)); // Initialize the random seed
	#endif

	acv::Init();
	#ifdef DEF_USE_CS
	#ifdef WIN32
	{ // (these variables not needed elsewhere)
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(sdl_window, &wmInfo);
		HWND hwnd = wmInfo.info.win.window;
		aud::Init(hwnd);
	}
	#else
	aud::Init(nullptr);
	#endif
	#else
	aud::Init(nullptr);
	#endif
	core::Init();
	input::Init();

	core::SetShadowTexture(rendertexture_shadow.glID);

	//________________________________________________________________________________________________________________________________
	// ENTER GAME LOOP ---------------------------------------------------------------------------------------------------------------

	btf64 current_frame_time = 0.f;
	btf64 next_frame_time = 0.f;

	// Variable declarations (so that they are not crossed by the label jumps)
	glm::mat4 mat_fb;
	btf64 test2;

	// Skip to the editor loop if the game is running in edit mode
	if (cfg::bEditMode) goto loop_editor;

	printf("Entered Game Loop\n");
updtime:
	// Check to make sure the game isnt paused, and the window is in focus
	if (!step_pause && focus)
	{
		#ifndef _DEBUG
		_controlfp(_PC_24, _MCW_PC); _controlfp(_RC_NEAR, _MCW_RC);
		//gpAssert((_controlfp(0, 0) & _MCW_PC) == _PC_24); gpAssert((_controlfp(0, 0) & _MCW_RC) == _RC_NEAR);
		#endif

		#ifdef DEF_SMOOTH_FRAMERATE
		// Wait until the exact right time to run a new frame
		while (current_frame_time <= next_frame_time)
			current_frame_time = (btf64)SDL_GetTicks() / 1000.;
		next_frame_time = current_frame_time + FRAME_TIME;
		#else
		// Just run the new frame now
		current_frame_time = (btf64)SDL_GetTicks() / 1000.;
		next_frame_time = current_frame_time + FRAME_TIME;
		Time::Update(current_frame_time);
		#endif

		// Run the frame
		if (!MainTick(&e)) goto exit;
	}
	// If the program is out of focus, or paused
	else
	{
		// Wait for a second then restart the function
		Sleep(1000u);
		goto updtime;
	}

	//________________________________________________________________________________________________________________________________
	// RENDER ------------------------------------------------------------------------------------------------------------------------

render:
	MainDraw();

	//-------------------------------- DRAW FRAMEBUFFER (TODO: try and clean this up a bit!)

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);

	// Now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_1);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate);
	glBlitFramebuffer(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(), 0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, cfg::iWinX, cfg::iWinY);

	// WIPE TEST
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// END TEST

	graphics::GetShader(graphics::S_POST).Use();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CW);

	mat_fb = glm::mat4(1.0f);

	#ifndef DEF_NMP
	// Set frame matrix to half-screen
	if (cfg::bSplitScreen)
	{
		mat_fb = glm::translate(mat_fb, glm::vec3(-0.5f, 0.f, 0.f));
		mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));
	}
	#endif // MP

	// screenwipe stuff (temp)
	wipe_time += FRAME_TIME * 1.6f;
	if (wipe_time > 1.f) wipe_time = 1.f;
	mat_fb = glm::rotate(mat_fb, glm::radians(wipe_time * 360.f * 2.f), glm::vec3(0.f, 0.f, 1.f));
	mat_fb = glm::scale(mat_fb, glm::vec3(wipe_time, wipe_time, wipe_time));

	graphics::GetShader(graphics::S_POST).setMat4(graphics::Shader::matTransform, *(graphics::Matrix4x4*)&mat_fb);

	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glUniform1i(glGetUniformLocation(graphics::GetShader(graphics::S_POST).ID, "screenTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, screenTexture.glID);
	#if DEF_DEPTH_BUFFER_RW
	glActiveTexture(GL_TEXTURE7); // activate the texture unit first before binding texture
	glUniform1i(glGetUniformLocation(graphics::GetShader(graphics::S_POST).ID, "depthTexture"), 7);
	glBindTexture(GL_TEXTURE_2D, depthbuffer_1.glID);
	#endif
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// BUFFER 2

	#ifndef DEF_NMP
	if (cfg::bSplitScreen)
	{
		// Now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_2);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate);
		glBlitFramebuffer(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(), 0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		mat_fb = glm::mat4(1.0f);
		mat_fb = glm::translate(mat_fb, glm::vec3(0.5f, 0.f, 0.f));
		mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));

		// screenwipe stuff (temp)
		mat_fb = glm::rotate(mat_fb, glm::radians(wipe_time * 360.f * 2.f), glm::vec3(0.f, 0.f, 1.f));
		mat_fb = glm::scale(mat_fb, glm::vec3(wipe_time, wipe_time, wipe_time));

		graphics::GetShader(graphics::S_POST).setMat4(graphics::Shader::matTransform, *(graphics::Matrix4x4*)&mat_fb);

		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glUniform1i(glGetUniformLocation(graphics::GetShader(graphics::S_POST).ID, "screenTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, screenTexture.glID);
		#if DEF_DEPTH_BUFFER_RW
		glActiveTexture(GL_TEXTURE7); // activate the texture unit first before binding texture
		glUniform1i(glGetUniformLocation(graphics::GetShader(graphics::S_POST).ID, "depthTexture"), 7);
		glBindTexture(GL_TEXTURE_2D, depthbuffer_1.glID);
		#endif
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	#endif // MP

	// TODO check up on this later!!
	glActiveTexture(GL_TEXTURE0); // For some reason or other, the texture must be reset (probably forgotten elsewhere)

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

	core::DrawPostDraw(FRAME_TIME);

	glFrontFace(GL_CCW);

	//-------------------------------- SWAP BUFFERS

	SDL_GL_SwapWindow(sdl_window);

	//-------------------------------- SLEEP FOR THE REMAINDER OF THE FRAME

	// Get the time now that we've rendered the frame
	current_frame_time = (btf64)SDL_GetTicks() / 1000.;
	// Get the frame time remainder that we need to sleep for (rounded down)
	test2 = (next_frame_time - current_frame_time) * 1000.;
	if (test2 > 0.)
	{
		DWORD test = (DWORD)test2;
		Sleep(test);
	}
	goto updtime; // Return to the beginning of the loop

	//--------------------------------------------------------------------------------------------------------------------------------

	//-------------------------------- ENTER EDITOR LOOP (messy, make it like the game loop)

loop_editor:
	printf("Entered Editor Loop\n");
	while (true)
	{
		input::ClearHitsAndDelta();
		while (SDL_PollEvent(&e)) input::UpdateInput(&e);

		if (input::GetHit(input::key::QUIT)) break;

		TranslateInputEditor();

		// Set GL properties for solid rendering
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);

		// Test
		glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_shadow);
		glClear(GL_DEPTH_BUFFER_BIT);
		core::SetViewFocus(0u);
		core::Draw(false);
		core::SetShadowTexture(rendertexture_shadow.glID);

		glViewport(0, 0, cfg::iWinX, cfg::iWinY);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.5f, 0.f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		core::SetViewFocus(0u);
		core::Draw();

		glFrontFace(GL_CCW);

		//-------------------------------- SWAP BUFFERS

		SDL_GL_SwapWindow(sdl_window);
		input::ClearHitsAndDelta();
	}

	//-------------------------------- END PROGRAM

exit:
	core::End();
	aud::End();
	acv::End();
	graphics::End();
	#ifdef DEF_NMP
	network::End();
	#endif

	SDL_GL_DeleteContext(sdl_glcontext);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();

	#ifdef _DEBUG
	// CRT memory leak report
	//_CrtDumpMemoryLeaks();
	#endif // _DEBUG

exitnoinit:
	return 0; // Goodbye
}
