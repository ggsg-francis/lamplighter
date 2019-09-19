// ctrl-r, ctrl-w for showing line indentation

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||| STYLE GUIDE |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// FOR BROAD CATEGORIZATION
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//--------------------------- BIG COMMENT ----------------------------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// FOR WITHIN FUNCTION DEFINITIONS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//------------- MEDIUM COMMENT -----------------------------------

// ANYTHING SMALLER / HEADER ZONING
//******************************** LITTLE COMMENT

// Littler comment

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//sdl
#include <SDL2\SDL.h>
#include <SDL2\SDL_syswm.h>

//opengl stuff
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
//gtx is experimental, gtc is stable
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//for transform rotation
#include <glm\gtc\quaternion.hpp>

#include "tga_reader.h"

//std stuff
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//engine
//try not to include so many things!
#include "graphics.hpp"
#include "maths.hpp"
#include "resources.h"
#include "input.h"
#include "cfg.h"
#include "audio.h"	//temp
#include "index.h"
#include "index_util.h"
#include "weather.h"
#include "time.hpp"
#include "test_zone.h"

//******************************** WINDOWING GLOBAL VARIABLES

SDL_Window* sdl_window;
SDL_GLContext sdl_glcontext;
bool quit = false;
bool focus = true;

//******************************** GAME GLOBAL VARIABLES (SHOULD GO ELSEWHERE, PROBABLY)

float mDeltaX; // Mouse delta X
float mDeltaY; // Mouse delta Y

GLuint quadVAO; // Quad vertex array object, used for framebuffer drawing
GLuint quadVBO; // Quad vertex buffer object, used for framebuffer drawing

GLuint framebuffer_1; // Framebuffer
graphics::Texture rendertexture_1; // Framebuffer render texture, can be sampled
graphics::Texture depthbuffer_1; // Framebuffer depth buffer, can be sampled

GLuint framebuffer_2; // Framebuffer
graphics::Texture rendertexture_2; // Framebuffer render texture, can be sampled
graphics::Texture depthbuffer_2; // Framebuffer depth buffer, can be sampled

GLuint framebuffer_intermediate; // not really sure what this does actually!!!!!!!!!!!!!!

GLuint depthMapFBO;
GLuint depthMap;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//--------------------------- STEP TICK ------------------------------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

SDL_Event sdl_event; // Input event

double step_accumulator = 0.f;
bool step_pause = false;

bool StepTick(double dt) // Fixed timestep tick function
{
	if (!focus) // If this window is out of focus, wait for an event to say that we are back in focus
		if (SDL_WaitEvent(&sdl_event))
		{
			if (sdl_event.type == SDL_WINDOWEVENT)
			{
				if (sdl_event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
				{
					SDL_Log("Window %d gained keyboard focus", sdl_event.window.windowID);
					focus = true;
					return false;
				}
			}
		}
	// e is an SDL_Event variable we've declared globally
	while (SDL_PollEvent(&sdl_event))
	{
		//If user closes the window
		if (sdl_event.type == SDL_QUIT) {
			quit = true;
		}
		else if (sdl_event.type == SDL_WINDOWEVENT)
		{
			if (sdl_event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
			{
				SDL_Log("Window %d lost keyboard focus", sdl_event.window.windowID);
				focus = false;
				return false;
			}
		}
		else if (focus)
		{
			input::UpdateInput(sdl_event);
		}
	}

	if (!step_pause && focus)
	{
		step_accumulator += dt;
		if (step_accumulator < FRAME_TIME)
			return false;
		step_accumulator = 0.f;

		//******************************** CONVERT DEVICE INPUT TO CHARA INPUT

		// Generate analogue input from directional keys
		m::Vector2 input_p1(0.f, 0.f);
		if (input::Get(input::key::fw_held)) // Forward
			input_p1.y = 1.f;
		if (input::Get(input::key::bk_held)) // Back
			input_p1.y = -1.f;
		if (input::Get(input::key::rt_held)) // Right
			input_p1.x += 1.f;
		if (input::Get(input::key::lt_held)) // Left
			input_p1.x -= 1.f;
		// Set input
		index::SetInput(0ui16, input_p1, input::mouse_x * 0.25f, input::mouse_y * 0.25f,
			input::Get(input::key::atk_held),
			input::Get(input::key::sprint_held),
			input::Get(input::key::aim_held));

		// Generate analogue input from joystick input
		m::Vector2 input_p2(0.f, 0.f);
		input_p2.x = input::joy_x_a;
		input_p2.y = -input::joy_y_a;
		// Set input
		index::SetInput(1ui16, input_p2, input::joy_x_b * 8.f, input::joy_y_b * 8.f,
			false,
			input::Get(input::key::c_sprint_held),
			false);

		//do stuff
		index::Tick((btf32)(FRAME_TIME));
		weather::Tick((btf32)(FRAME_TIME));

		return true;
	}
	else
		return false;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//--------------------------- SET WINDOW TITLE -----------------------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void SetTitle(btui8 id)
{
	if (id == 0)
		SDL_SetWindowTitle(sdl_window, "Client Player 0");
	if (id == 1)
		SDL_SetWindowTitle(sdl_window, "Client Player 1");
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//--------------------------- MAIN -----------------------------------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int main(int argc, char * argv[])
{
	//******************************** TEST ZONE

	InitTest();

	//******************************** LOAD CONFIG
	
	cfg::LoadCfg();

	//******************************** INITIALIZE SDL2

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
	#ifdef DEF_SDL_USE_ZBUF
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // With a 24 bit Z buffer (unneeded, our own framebuffer has one)
	#endif

	sdl_window = SDL_CreateWindow("WelCUM home master Nya~", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		cfg::iWinX, cfg::iWinY, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); // Create window
	if (!sdl_window) return -1; // Die if creation failed

	sdl_glcontext = SDL_GL_CreateContext(sdl_window); // Create our opengl context and attach it to our window

	SDL_GL_SetSwapInterval(1); // This makes our buffer swap syncronized with the monitor's vertical refresh

	#ifdef DEF_INPUT_MOUSE_HIDDEN
	SDL_ShowCursor(false); // Set mouse cursor invisible
	#endif
	#ifdef DEF_INPUT_MOUSE_1ST_PERSON
	SDL_SetRelativeMouseMode((SDL_bool)true); // Set mouse input to use raw input
	#endif

	//******************************** INITIALIZE GLAD
	
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) // Load all OpenGL function pointers
	{
		std::cout << "ERROR: gladLoadGLLoader failed!" << std::endl;
		return -1;
	}
	glViewport(0, 0, cfg::iWinX, cfg::iWinY); // Set opengl viewport size to window size X, Y, W, H
	
	//******************************** CONFIGURE GLOBAL OPENGL STATE

	glEnable(GL_CULL_FACE); // Enable face culling	
	glCullFace(GL_FRONT); // Set culling mode
	glFrontFace(GL_CCW); // Set front face

	//******************************** SCREEN QUAD (SHOULD GO IN GRAPHICS) FOR DRAWING FRAMEBUFFER

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
	//unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//******************************** CREATE EACH SCREEN FRAMEBUFFER

	//create framebuffer
	glGenFramebuffers(1, &framebuffer_1);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);
	rendertexture_1.InitRenderTexture(cfg::iWinX / 2, cfg::iWinY, true);
	depthbuffer_1.InitRenderBuffer(cfg::iWinX / 2, cfg::iWinY, false);
	//depthbuffer_1.InitDepthTexture(cfg::iWinX / 2, cfg::iWinY, false);

	glGenFramebuffers(1, &framebuffer_2);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_2);
	rendertexture_2.InitRenderTexture(cfg::iWinX / 2, cfg::iWinY, true);
	depthbuffer_2.InitRenderBuffer(cfg::iWinX / 2, cfg::iWinY, false);
	//depthbuffer_2.InitDepthTexture(cfg::iWinX / 2, cfg::iWinY, false);

	//******************************** CREATE SHADOWBUFFER

	glGenFramebuffers(1, &depthMapFBO);
	#define SHADOW_WIDTH 1024
	#define SHADOW_HEIGHT 1024
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_COMPARE_FUNC, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//#undef SHADOW_WIDTH
	//#undef SHADOW_HEIGHT
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//******************************** CONFIGURE SECOND POST-PROCESSING FRAMEBUFFER

	glGenFramebuffers(1, &framebuffer_intermediate);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_intermediate);
	// create a color attachment texture
	unsigned int screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	#ifdef DEF_HDR // HDR Texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, cfg::iWinX / 2, cfg::iWinY, 0, GL_RGBA, GL_FLOAT, NULL); //create a blank image
	#else // Not HDR Texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cfg::iWinX / 2, cfg::iWinY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer
	#ifdef DEF_MULTISAMPLE
	glEnable(GL_MULTISAMPLE); // Enable multisampling for anti-aliasing
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE); // Enable ATOC for texture alpha
	#endif // DEF_MULTISAMPLE

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- INITIALIZATION -------------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	graphics::Init();
	res::Init();

	// sprite test
	graphics::GUIBitmap guibmp;
	guibmp.Init();
	guibmp.SetShader(&graphics::shader_gui);

	// Actual initialization

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(sdl_window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;

	aud::Init(hwnd);
	aud::PlaySnd(); // sound test

	index::Init();

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- MAIN LOOP ------------------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	while (!quit)
	{
		// per-frame time delta calculation
		time = (btf64)(SDL_GetTicks() / 1000.);
		time_delta = time - time_last;
		time_last = time;

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		//------------- READY INPUT --------------------------------------

		mDeltaX = 0; mDeltaY = 0;
		//inputX = 0; inputY = 0;

		if (input::Get(input::key::quit))
		{
			quit = true;
		}

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		//------------- PER-FRAME GAME LOGIC -----------------------------

		if (StepTick(time_delta)) // Run simulation at a fixed step, if step proceed to render
		{
			//******************************** RENDER ENTITIES

			//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

			//glm::vec3 colour = *(glm::vec3*)weather::FogColour();
			glm::vec3 colour = *(glm::vec3*)weather::SunColour();

			// BUFFER 1 (LEFT SCREEN)
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);

			glViewport(0, 0, cfg::iWinX / 2, cfg::iWinY);
			//double size
			//glViewport(0, 0, cfg::iWinX, cfg::iWinY * 2);
			//glClearColor(colour.x, colour.y, colour.z, 1.0f);
			//glClearColor(0.f, 0.f, 1.f, 1.0f);
			glClearColor(0.f, 0.f, 0.f, 1.0f);

			//******************************** BUFFER 1 (LEFT SCREEN)

			// Set GL properties for solid rendering
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);

			// Test
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			index::SetViewFocus(0x0u);
			index::Draw(false);
			index::SetShadowTexture(depthMap);

			glViewport(0, 0, cfg::iWinX / 2, cfg::iWinY);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			index::SetViewFocus(0x0u);
			index::Draw();
			// Read pixel
			unsigned char pixel[4];
			glReadPixels(320, 240, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);

			if (pixel[2] == 0ui8)
				//index::SetViewTargetID((btID)pixel[0]);
				index::SetViewTargetID((btID)pixel[0] + ((btID)pixel[1] << 8u));
			else
				index::SetViewTargetID(ID_NULL);

			index::DrawGUI();
			index::TickGUI(); // causes a crash if before drawgui

			//******************************** BUFFER 1 GUI

			// Player 1, X Start
			int p1_x_start = -(int)cfg::iWinX / 4;
			int p1_y_start = -(int)cfg::iWinY / 2;

			//******************************** BUFFER 2 (RIGHT SCREEN)

			// Set GL properties for solid rendering
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_2);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			index::SetViewFocus(0x1u);
			index::Draw();

			//******************************** BUFFER 2 GUI

			//draw ui
			graphics::shader_gui.Use();
			guibmp.SetTexture(res::GetBTT(res::t_gui_bar_red).glID);

			guibmp.Draw(p1_x_start + 32, p1_y_start + 16, (int)(index::GetHP(index::players[1ui16]) * 64.f), 32);

			//******************************** DRAW FRAMEBUFFER

			#ifdef DEF_BLIT_FRAME

			// Now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_1);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate);
			glBlitFramebuffer(0, 0, cfg::iWinX / 2, cfg::iWinY, 0, 0, cfg::iWinX / 2, cfg::iWinY, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, cfg::iWinX, cfg::iWinY);

			glClearColor(1.f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			graphics::shader_post.Use();

			glDisable(GL_DEPTH_TEST);
			glFrontFace(GL_CW);

			glm::mat4 mat_fb = glm::mat4(1.0f);
			mat_fb = glm::translate(mat_fb, glm::vec3(-0.5f, 0.f, 0.f));
			mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));
			// get matrix's uniform location and set matrix
			graphics::shader_post.setMat4("transform", mat_fb);

			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
			glUniform1i(glGetUniformLocation(graphics::shader_post.ID, "screenTexture"), 0);
			//glUniform1i(glGetUniformLocation(pRes->screen_post.ID, "screenTexture"), 0);
			glBindTexture(GL_TEXTURE_2D, screenTexture);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// BUFFER 2

			// Now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_2);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate);
			glBlitFramebuffer(0, 0, cfg::iWinX / 2, cfg::iWinY, 0, 0, cfg::iWinX / 2, cfg::iWinY, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			mat_fb = glm::mat4(1.0f);
			mat_fb = glm::translate(mat_fb, glm::vec3(0.5f, 0.f, 0.f));
			mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));
			// get matrix's uniform location and set matrix
			graphics::shader_post.setMat4("transform", mat_fb);

			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
			glUniform1i(glGetUniformLocation(graphics::shader_post.ID, "screenTexture"), 0);
			//glUniform1i(glGetUniformLocation(pRes->screen_post.ID, "screenTexture"), 0);
			glBindTexture(GL_TEXTURE_2D, screenTexture);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			#endif

			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

			glFrontFace(GL_CCW);

			//******************************** SWAP BUFFERS

			SDL_GL_SwapWindow(sdl_window);

			//******************************** CLEAR INPUT

			input::ResetInput();
		} // End if StepTick
	}

	//******************************** END PROGRAM

	index::End();
	aud::End();
	res::End();

	SDL_GL_DeleteContext(sdl_glcontext);
	SDL_DestroyWindow(sdl_window);
	//renderer used for drawing text and such using SDL
	//SDL_DestroyRenderer(m_renderer);
	SDL_Quit();

	return 0; // Goodbye
}