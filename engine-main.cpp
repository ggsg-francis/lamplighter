// ctrl-r, ctrl-w for showing line indentation

// CRT memory leak detection

#ifdef _DEBUG
//#include "vld.h"
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
#endif // _DEBUG

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

//opengl stuff
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
//gtx is experimental, gtc is stable
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//for transform rotation
#include <glm\gtc\quaternion.hpp>

//engine
//try not to include so many things!
#include "graphics.hpp"
#include "maths.hpp"
#include "archive.hpp"
#include "input.h"
#include "cfg.h"
#include "network_client.h"
#include "audio.hpp"
#include "index.h"
#include "weather.h"
#include "time.hpp"
#include "test_zone.h"

//-------------------------------- WINDOWING GLOBAL VARIABLES

GLFWwindow* window;
bool quit = false;
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

void FocusCallback(GLFWwindow* win, int focus2)
{
	//focus = focus2 == 1;
}
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	cfg::iWinX = width; cfg::iWinY = height;
	if (!cfg::bEditMode)
	{
		graphics::SetFrameSize(cfg::iWinX, cfg::iWinY);
		RegenFramebuffers();
	}
}

//________________________________________________________________________________________________________________________________
//--------------------------- STEP TICK ------------------------------------------------------------------------------------------

double step_accumulator = 0.f;
bool step_pause = false;

bool StepTick(double dt) // Fixed timestep tick function
{
	if (!step_pause && focus)
	{
		step_accumulator += dt;
		if (step_accumulator < FRAME_TIME)
			return false;
		step_accumulator = 0.f;

		Time::Step();

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

		//do stuff
		index::Tick((btf32)(FRAME_TIME));
		weather::Tick((btf32)(FRAME_TIME));

		return true;
	}
	else
		return false;
}

//fixed timestep tick function
bool StepTickEditor(double dt)
{
	if (!step_pause && focus)
	{
		step_accumulator += dt;
		if (step_accumulator < FRAME_TIME)
			return false;
		step_accumulator = 0.f;

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

//int main(int argc, char * argv[])
int main()
{
	//-------------------------------- TEST ZONE

	InitTest();

	//-------------------------------- LOAD CONFIG
	
	cfg::LoadCfg();

	//-------------------------------- CONNECT TO SERVER

	network::Init();
	network::Connect();

	//-------------------------------- INITIALIZE GLFW

	glfwInit();
	// Set to OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_DECORATED, false);

	if (cfg::bFullscreen) window = glfwCreateWindow(cfg::iWinX, cfg::iWinY, "TSOA", glfwGetPrimaryMonitor(), NULL);
	else window = glfwCreateWindow(cfg::iWinX, cfg::iWinY, "TSOA", NULL, NULL);
	if (window == NULL) // If the window creation failed, error & close
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Sets this window to be in focus, I think				
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback); //set glfw to call resize gl viewport function
	glfwSetKeyCallback(window, input::KeyCallback);
	glfwSetCharCallback(window, input::CharCallback);
	glfwSetCursorPosCallback(window, input::CursorCallback);
	glfwSetScrollCallback(window, input::ScrollCallback);
	glfwSetMouseButtonCallback(window, input::MouseButtonCallback);
	// window focus callbacks
	glfwSetWindowFocusCallback(window, FocusCallback);

	#if defined DEF_INPUT_MOUSE_HIDDEN
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	#elif defined DEF_INPUT_MOUSE_1ST_PERSON
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	#else
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	#endif

	/*
	SDL_Joystick* gGameController = NULL; // Game Controller 1 handler

	if (SDL_NumJoysticks() < 1) // Check for joysticks
		printf("Warning: No joysticks connected!\n");
	else { // If there is one
		gGameController = SDL_JoystickOpen(0); // Load joystick
		if (gGameController == NULL) { printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError()); }
	}*/

	//-------------------------------- INITIALIZE GLAD
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // Load all OpenGL function pointers
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

	//-------------------------------- ENTER GAME LOOP (ITS A MESS)

	if (!cfg::bEditMode)
	{
		printf("Entered Game Loop\n");
		while (!quit)
		{
			Time::Update((btf64)(glfwGetTime()));

			if (input::GetHit(input::key::QUIT)) quit = true;

			if (StepTick(Time::deltaTime)) // Run simulation at a fixed step, if step proceed to render
			{
				//-------------------------------- RENDER ENTITIES

				// BUFFER 1 (LEFT SCREEN)
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);

				glViewport(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY());
				glClearColor(0.f, 0.f, 0.f, 1.0f);

				//-------------------------------- BUFFER 1 (LEFT SCREEN)

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

				glViewport(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY());

				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				index::SetViewFocus(0u);
				index::Draw();
				index::DrawGUI();
				index::TickGUI(); // causes a crash if before drawgui (does it still?)

				//-------------------------------- BUFFER 2 (RIGHT SCREEN)

				// Set GL properties for solid rendering
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ZERO);

				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_2);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				index::SetViewFocus(1u);
				index::Draw();
				index::DrawGUI();
				index::TickGUI(); // causes a crash if before drawgui (does it still?)

				//-------------------------------- DRAW FRAMEBUFFER

				glEnable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ZERO);
				//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_2);

				#ifdef DEF_BLIT_FRAME

				// Now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
				glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_1);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate);
				glBlitFramebuffer(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(), 0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, cfg::iWinX, cfg::iWinY);

				glClearColor(1.f, 0.5f, 0.5f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				graphics::GetShader(graphics::S_POST).Use();

				glDisable(GL_DEPTH_TEST);
				glFrontFace(GL_CW);

				glm::mat4 mat_fb = glm::mat4(1.0f);
				mat_fb = glm::translate(mat_fb, glm::vec3(-0.5f, 0.f, 0.f));
				mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));
				// get matrix's uniform location and set matrix
				graphics::GetShader(graphics::S_POST).setMat4("transform", mat_fb);

				glBindVertexArray(quadVAO);
				glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
				glUniform1i(glGetUniformLocation(graphics::GetShader(graphics::S_POST).ID, "screenTexture"), 0);
				glBindTexture(GL_TEXTURE_2D, screenTexture);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// BUFFER 2

				// Now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
				glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_2);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate);
				glBlitFramebuffer(0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(), 0, 0, graphics::FrameSizeX(), graphics::FrameSizeY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				mat_fb = glm::mat4(1.0f);
				mat_fb = glm::translate(mat_fb, glm::vec3(0.5f, 0.f, 0.f));
				mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));
				// get matrix's uniform location and set matrix
				graphics::GetShader(graphics::S_POST).setMat4("transform", mat_fb);

				glBindVertexArray(quadVAO);
				glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
				glUniform1i(glGetUniformLocation(graphics::GetShader(graphics::S_POST).ID, "screenTexture"), 0);
				glBindTexture(GL_TEXTURE_2D, screenTexture);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				// to do check up on this later!!
				glActiveTexture(GL_TEXTURE0); // For some reason or other, the texture must be reset (probably forgotten elsewhere)

				#endif

				glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

				if (cfg::bShowConsole) index::DrawPostDraw();

				glFrontFace(GL_CCW);

				//-------------------------------- SWAP BUFFERS

				glfwSwapBuffers(window);
				input::ClearHitsAndDelta();
				glfwPollEvents();
				input::UpdateControllerInput();
			} // End if StepTick
		}
	}
	
	//-------------------------------- ENTER EDITOR LOOP

	else
	{
		printf("Entered Editor Loop\n");
		while (!quit)
		{
			Time::Update((btf64)(glfwGetTime()));

			if (input::GetHit(input::key::QUIT)) quit = true;

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
				glClearColor(128.f, 255.f, 255.f, 1.0f);
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

				glfwSwapBuffers(window);
				input::ClearHitsAndDelta();
				glfwPollEvents();
			} // End if StepTick
		}
	}
	
	//-------------------------------- END PROGRAM

	index::End();
	aud::End();
	res::End();
	graphics::End();

	glfwTerminate();

	#ifdef _DEBUG
	// CRT memory leak report
	//_CrtDumpMemoryLeaks();
	#endif // _DEBUG

	return 0; // Goodbye
}