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
//-------------------------------- BIG COMMENT

// ANYTHING SMALLER / HEADER ZONING
//-------------------------------- LITTLE COMMENT

// Littler comment

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// OpenGL
#include <glad\glad.h>
#include <glm\glm.hpp>
//gtx is experimental, gtc is stable
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//for transform rotation
#include <glm\gtc\quaternion.hpp>

// SDL
#include <SDL2\SDL.h>
#include <SDL2\SDL_syswm.h>

//#ifdef DEF_USE_CS
//// For getting the window handle
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW\glfw3native.h>
//#endif

//engine
//try not to include so many things!
#include "graphics.hpp"
#include "maths.hpp"
#include "archive.hpp"
#include "input.h"
#include "cfg.h"
#include "network_client.h"
#include "audio.hpp"
#include "core.h"
#include "weather.h"
#include "time.hpp"
#include "test_zone.h"

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

GLuint framebuffer_intermediate; // not really sure what this does actually!!!!!!!!!!!!!!

GLuint framebuffer_shadow; // Shadowmap framebuffer
GLuint rendertexture_shadow; // Shadowmap rendertexture

//________________________________________________________________________________________________________________________________
//--------------------------- UTIL FUNCTIONS -------------------------------------------------------------------------------------

void RegenFramebuffers()
{
	// Left screen
	glGenFramebuffers(1, &framebuffer_1);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);
	rendertexture_1.InitRenderTexture(graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	depthbuffer_1.InitDepthBuffer(graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	//depthbuffer_1.InitDepthTexture(cfg::iWinX / 2, cfg::iWinY, false);

	// Right screen
	glGenFramebuffers(1, &framebuffer_2);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_2);
	rendertexture_2.InitRenderTexture(graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	depthbuffer_2.InitDepthBuffer(graphics::FrameSizeX(), graphics::FrameSizeY(), false);
	//depthbuffer_2.InitDepthTexture(cfg::iWinX / 2, cfg::iWinY, false);
}

//void FocusCallback(GLFWwindow* win, int focus2)
//{
//	//focus = focus2 == 1;
//}
//void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
//{
//	cfg::iWinX = width; cfg::iWinY = height;
//	if (!cfg::bEditMode)
//	{
//		graphics::SetFrameSize(cfg::iWinX, cfg::iWinY);
//		RegenFramebuffers();
//	}
//}

//________________________________________________________________________________________________________________________________
//--------------------------- STEP TICK ------------------------------------------------------------------------------------------

bool step_pause = false;

inline void UpdateInput() // Fixed timestep tick function
{
	//-------------------------------- CONVERT DEVICE INPUT TO CHARA INPUT

	// Generate analogue input from directional keys
	m::Vector2 input_p1(0.f, 0.f);
	if (input::GetHeld(input::key::DIR_F)) // Forward
		input_p1.y = 1.f;
	if (input::GetHeld(input::key::DIR_B)) // Back
		input_p1.y = -1.f;
	if (input::GetHeld(input::key::DIR_R)) // Right
		input_p1.x += 1.f;
	if (input::GetHeld(input::key::DIR_L)) // Left
		input_p1.x -= 1.f;
	// Set input
	index::SetInput(0ui16, input_p1, input::mouse_x * 0.25f, input::mouse_y * 0.25f,
		input::GetHeld(input::key::USE),
		input::GetHit(input::key::USE),
		input::GetHit(input::key::USE_ALT),
		input::GetHeld(input::key::RUN),
		false,
		input::GetHit(input::key::ACTION_A),
		input::GetHit(input::key::ACTION_B),
		input::GetHit(input::key::ACTION_C)); // 3rd 'aim' variable was here

	// Generate analogue input from joystick input
	m::Vector2 input_p2(0.f, 0.f);
	input_p2.x = input::joy_x_a;
	input_p2.y = -input::joy_y_a;
	// Set input
	index::SetInput(1ui16, input_p2, input::joy_x_b * 8.f, input::joy_y_b * 8.f,
		input::GetHeld(input::key::C_USE),
		input::GetHit(input::key::C_USE),
		input::GetHit(input::key::C_USE_ALT),
		input::GetHeld(input::key::C_RUN),
		false,
		input::GetHit(input::key::C_ACTION_A),
		input::GetHit(input::key::C_ACTION_B),
		input::GetHit(input::key::C_ACTION_C)); // 3rd 'aim' variable was here
}

//fixed timestep tick function
bool StepTickEditor(double dt)
{
	if (!step_pause && focus)
	{
		/*step_accumulator += dt;
		if (step_accumulator < FRAME_TIME)
			return false;
		step_accumulator = 0.f;*/

		//-------------------------------- CONVERT DEVICE INPUT TO CHARA INPUT

		// Generate analogue input from directional keys
		m::Vector2 input_p1(0.f, 0.f);
		if (input::GetHeld(input::key::DIR_F)) // Forward
			input_p1.y = 1.f;
		if (input::GetHeld(input::key::DIR_B)) // Back
			input_p1.y = -1.f;
		if (input::GetHeld(input::key::DIR_R)) // Right
			input_p1.x += 1.f;
		if (input::GetHeld(input::key::DIR_L)) // Left
			input_p1.x -= 1.f;
		// Set input
		index::SetInput(0ui16, input_p1, input::mouse_x * 0.25f, input::mouse_y * 0.25f,
			input::GetHit(input::key::USE),
			input::GetHit(input::key::USE),
			input::GetHit(input::key::USE_ALT),
			input::GetHeld(input::key::RUN),
			false,
			input::GetHit(input::key::ACTION_A),
			input::GetHit(input::key::ACTION_B),
			input::GetHit(input::key::ACTION_C)); // 3rd 'aim' variable was here

		//do stuff
		index::Tick(FRAME_TIME);
		weather::Tick(FRAME_TIME);

		return true;
	}
	else
	{
		return false;
	}
}

//________________________________________________________________________________________________________________________________
//--------------------------- MAIN -----------------------------------------------------------------------------------------------

int main(int argc, char * argv[]) // SDL Main
{
	// Test stuff
	InitTest();

	// Load config
	cfg::LoadCfg();

	#ifdef DEF_NMP
	// Connect to server
	network::Init();
	network::Connect();
	#endif

	//-------------------------------- INITIALIZE SDL2

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) != 0)
		return 0;

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

	sdl_window = SDL_CreateWindow("TSOA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		cfg::iWinX, cfg::iWinY, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); // Create window
	if (!sdl_window) return -1; // Die if creation failed

	sdl_glcontext = SDL_GL_CreateContext(sdl_window); // Create our opengl context and attach it to our window

	SDL_GL_SetSwapInterval(1); // This makes our buffer swap syncronized with the monitor's vertical refresh

	SDL_Event e; // Input event

	#if defined DEF_INPUT_MOUSE_HIDDEN
	SDL_ShowCursor(false); // Set mouse cursor invisible
	#elif defined DEF_INPUT_MOUSE_1ST_PERSON
	SDL_SetRelativeMouseMode((SDL_bool)true); // Set mouse input to use raw input
	#else
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	#endif

	//-------------------------------- INITIALIZE GLAD
	
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) // Load all OpenGL function pointers
	{
		std::cout << "ERROR: gladLoadGLLoader failed!" << std::endl;
		return -1;
	}
	glViewport(0, 0, cfg::iWinX, cfg::iWinY); // Set opengl viewport size to window size X, Y, W, H
	
	//-------------------------------- INITIALIZE GRAPHICS

	graphics::Init();

	//-------------------------------- CONFIGURE GLOBAL OPENGL STATE

	glEnable(GL_CULL_FACE); // Enable face culling	
	glCullFace(GL_FRONT); // Set culling mode
	glFrontFace(GL_CCW); // Set front face

	//-------------------------------- SCREEN QUAD (SHOULD GO IN GRAPHICS) FOR DRAWING FRAMEBUFFER

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
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

	//-------------------------------- CREATE EACH SCREEN FRAMEBUFFER

	if (!cfg::bEditMode) RegenFramebuffers();

	//-------------------------------- CREATE SHADOWBUFFER

	glGenFramebuffers(1, &framebuffer_shadow);
	glGenTextures(1, &rendertexture_shadow);
	glBindTexture(GL_TEXTURE_2D, rendertexture_shadow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_COMPARE_FUNC, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_shadow);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rendertexture_shadow, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//-------------------------------- CONFIGURE SECOND POST-PROCESSING FRAMEBUFFER

	glGenFramebuffers(1, &framebuffer_intermediate);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_intermediate);
	// create a color attachment texture
	unsigned int screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	#ifdef DEF_HDR // HDR Texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, graphics::FrameSizeX(), graphics::FrameSizeY(), 0, GL_RGBA, GL_FLOAT, NULL); //create a blank image
	#else // Not HDR Texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, graphics::FrameSizeX(), graphics::FrameSizeY(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	#ifdef DEF_LINEAR_FB
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	#endif
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer
	#ifdef DEF_MULTISAMPLE
	glEnable(GL_MULTISAMPLE); // Enable multisampling for anti-aliasing
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE); // Enable ATOC for texture alpha
	#endif // DEF_MULTISAMPLE

	//-------------------------------- INITIALIZATION

	res::Init();
	#ifdef DEF_USE_CS
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(sdl_window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;
	aud::Init(hwnd);
	#else
	aud::Init();
	#endif
	index::Init();
	input::Init();

	index::SetShadowTexture(rendertexture_shadow);

	//________________________________________________________________________________________________________________________________
	//-------------------------------- ENTER GAME LOOP

	btf64 current_frame_time = 0.f;
	btf64 next_frame_time = 0.f;

	// Skip to the editor loop if the game is running in edit mode
	if (cfg::bEditMode) goto loop_editor;

	printf("Entered Game Loop\n");
updtime:
	// Check to make sure the game isnt paused, and the window is in focus
	if (!step_pause && focus)
	{
		#ifdef DEF_SMOOTH_FRAMERATE
		// Wait until the exact right time to run a new frame
		while (current_frame_time <= next_frame_time)
		{
			current_frame_time = (btf64)SDL_GetTicks() / 1000.;
		}
		next_frame_time = current_frame_time + FRAME_TIME;
		Time::Update(current_frame_time);

		aud::Update(Time::deltaTime);

		#else
		// Just run the new frame now
		current_frame_time = (btf64)glfwGetTime();
		next_frame_time = current_frame_time + FRAME_TIME;
		Time::Update(current_frame_time);
		#endif

		if (input::GetHit(input::key::QUIT)) goto exit;

		input::ClearHitsAndDelta();
		while (SDL_PollEvent(&e))
		{
			//If user closes the window
			if (e.type == SDL_QUIT) {
				goto exit;
			}
			else
			{
				input::UpdateInput(&e);
				//#error
			}
		}
		//input::UpdateControllerInput();

		// If we reach this point, its time to run the tick
		Time::Step();
		UpdateInput();
		index::Tick((btf32)(FRAME_TIME));
		weather::Tick((btf32)(FRAME_TIME));
	}
	// If the program is out of focus, or paused
	else
	{
		// Wait for a second then restart the function
		Sleep(1000u);
		goto updtime;
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- RENDER

render:

	// BUFFER 1 (LEFT SCREEN)
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);

	glViewport(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY());
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	//-------------------------------- BUFFER 1 (LEFT SCREEN)

	// Set GL properties for solid rendering
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	//-------------------------------- RENDER SHADOWMAP
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_shadow);
	glClear(GL_DEPTH_BUFFER_BIT);
	index::SetViewFocus(0u); // Set render POV
	index::Draw(false);
	//-------------------------------- RENDER VIEW
	glViewport(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY());
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	index::SetViewFocus(0u); // Set render POV
	index::Draw(); index::DrawGUI();
	index::TickGUI(); // causes a crash if before drawgui (does it still?)

	//-------------------------------- BUFFER 2 (RIGHT SCREEN)

	#ifndef DEF_NMP
	if (cfg::bSplitScreen) {
		// Set GL properties for solid rendering (again....)
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);
		//-------------------------------- RENDER SHADOWMAP
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_shadow);
		glClear(GL_DEPTH_BUFFER_BIT);
		index::SetViewFocus(1u); // Set render POV to second player
		index::Draw(false);
		//-------------------------------- RENDER VIEW
		glViewport(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY());
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_2);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		index::SetViewFocus(1u); // Set render POV to second player
		index::Draw(); index::DrawGUI();
		index::TickGUI(); // causes a crash if before drawgui (does it still?)
	}
	#endif // MP

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

	graphics::GetShader(graphics::S_POST).Use();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CW);

	glm::mat4 mat_fb = glm::mat4(1.0f);
	#ifndef DEF_NMP
	// Set frame matrix to half-screen
	if (cfg::bSplitScreen)
	{
		mat_fb = glm::translate(mat_fb, glm::vec3(-0.5f, 0.f, 0.f));
		mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));
	}
	#endif // MP
	// get matrix's uniform location and set matrix
	graphics::GetShader(graphics::S_POST).setMat4(graphics::Shader::matTransform, *(graphics::Matrix4x4*)&mat_fb);

	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glUniform1i(glGetUniformLocation(graphics::GetShader(graphics::S_POST).ID, "screenTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
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
		// get matrix's uniform location and set matrix
		graphics::GetShader(graphics::S_POST).setMat4(graphics::Shader::matTransform, *(graphics::Matrix4x4*)&mat_fb);

		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glUniform1i(glGetUniformLocation(graphics::GetShader(graphics::S_POST).ID, "screenTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	#endif // MP

	// to do check up on this later!!
	glActiveTexture(GL_TEXTURE0); // For some reason or other, the texture must be reset (probably forgotten elsewhere)

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

	index::DrawPostDraw();

	glFrontFace(GL_CCW);

	//-------------------------------- SWAP BUFFERS

	SDL_GL_SwapWindow(sdl_window);

	//-------------------------------- SLEEP FOR THE REMAINDER OF THE FRAME

	// Get the time now that we've rendered the frame
	current_frame_time = (btf64)SDL_GetTicks() / 1000.;
	// Get the frame time remainder that we need to sleep for (rounded down)
	btf64 test2 = (next_frame_time - current_frame_time) * 1000.;
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
		Time::Update((btf64)SDL_GetTicks() / 1000.);

		if (input::GetHit(input::key::QUIT)) break;

		if (StepTickEditor(Time::deltaTime)) // Run simulation at a fixed step, if step proceed to render
		{
			// Set GL properties for solid rendering
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);

			// Test
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_shadow);
			glClear(GL_DEPTH_BUFFER_BIT);
			index::SetViewFocus(0u);
			index::Draw(false);
			index::SetShadowTexture(rendertexture_shadow);

			glViewport(0, 0, cfg::iWinX, cfg::iWinY);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0.5f, 1.f, 1.f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			index::SetViewFocus(0u);
			index::Draw();
			//// Read pixel
			//unsigned char pixel[4];
			//glReadPixels(320, 240, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);

			//if (pixel[2] == 0ui8)
			//	//index::SetViewTargetID((btID)pixel[0]);
			//	index::SetViewTargetID((btID)pixel[0] + ((btID)pixel[1] << 8u));
			//else
			//	index::SetViewTargetID(ID_NULL);

			//index::DrawGUI();
			//index::TickGUI(); // causes a crash if before drawgui

			glFrontFace(GL_CCW);

			//-------------------------------- SWAP BUFFERS

			SDL_GL_SwapWindow(sdl_window);
			input::ClearHitsAndDelta();
			//glfwPollEvents();
			//#error
		} // End if StepTick
	}

	//-------------------------------- END PROGRAM

exit:
	index::End();
	aud::End();
	res::End();
	graphics::End();

	SDL_GL_DeleteContext(sdl_glcontext);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();

	#ifdef _DEBUG
	// CRT memory leak report
	//_CrtDumpMemoryLeaks();
	#endif // _DEBUG

	return 0; // Goodbye
}