//B TYPE ENGINE / SPIES VS MERCS 2019

//sdl
#include <SDL2\SDL.h>
//#include <SDL2\SDL_ttf.h>

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
//#include "global.h"

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

#include "env.h"
//#include "autonomy.h"

#include "time.h"

//....................................... WINDOWING GLOBAL VARIABLES

SDL_Window* sdl_window;
//SDL_Renderer * m_renderer;
SDL_GLContext sdl_glcontext;

SDL_Surface* surf;
//TTF_Font* font;

bool quit;

//....................................... GAME GLOBAL VARIABLES (SHOULD GO ELSEWHERE)

//network id
unsigned int nid = 0;

//for tga importing
extern TGA_ORDER *TGA_READER_ARGB;
extern TGA_ORDER *TGA_READER_ABGR;

//hm??
bool firstMouse = true;
//player inputs
float inputX;
float inputY;
//mouse delta
float mDeltaX;
float mDeltaY;

float networkTimerTemp;

//for drawing framebuffer
unsigned int quadVAO;
unsigned int quadVBO;
//framebuffer
GLuint framebuffer_1;
graphics::Texture rendertexture_1;
graphics::Texture depthbuffer_1;

GLuint framebuffer_intermediate;
//Texture rendertexture_intermediate;

//Texture shadowbuffer_2;

class BTError : public std::exception
{
	std::string msg;
public:
	BTError();
	BTError(const std::string &);
	virtual ~BTError() throw();
	virtual const char * what() const throw();
};

BTError::BTError() : exception(), msg(SDL_GetError())
{
}

BTError::BTError(const std::string & m) : exception(), msg(m)
{
}

BTError::~BTError() throw()
{
}

const char * BTError::what() const throw()
{
	return msg.c_str();
}

/*
void SDLInit(Uint32 flags)
{
if (SDL_Init(flags) != 0)
throw BTError();

//request gl 3.3 'context'
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

// Turn on double buffering with a 24bit Z buffer.
// You may need to change this to 16 or 32 for your system
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

m_window = SDL_CreateWindow("WelCUM home master Nya~", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
winx, winy, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
if (!m_window) // Die if creation failed
throw BTError();

// Create our opengl context and attach it to our window
maincontext = SDL_GL_CreateContext(m_window);

// This makes our buffer swap syncronized with the monitor's vertical refresh
SDL_GL_SetSwapInterval(1);
}
*/

void SDLClose()
{
	SDL_GL_DeleteContext(sdl_glcontext);
	SDL_DestroyWindow(sdl_window);
	//renderer used for drawing text and such using SDL
	//SDL_DestroyRenderer(m_renderer);
	SDL_Quit();
}

void Draw()
{
	/* Clear our buffer with a red background */
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	/* Swap our back buffer to the front */
	SDL_GL_SwapWindow(sdl_window);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||| STEP TICK ||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//variables
//as per 'the simulation loop' (rigid body overview)
float step_accumulator = 0.f;
//step size currently set to 60 frames per second
//to do: add this to config when config is added here
//float btpxStepSize = 1.f / 30.f;
//float btpxStepSize = 1.f / 60.f;
bool step_pause = false;

//fixed timestep tick function
//to do: do the timer logic *before* calling the function, as opposed to within it as is done currently
bool StepTick(double dt)
{
	if (!step_pause)
	{
		// timing
		step_accumulator += dt;
		if (step_accumulator < FRAME_TIME)
			return false;
		//while (btpxAccumulator > FRAME_TIME)
		//	btpxAccumulator -= FRAME_TIME;
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
			input::Get(input::key::atk_hit),
			input::Get(input::key::sprint_held),
			input::Get(input::key::aim_held));

		//do stuff
		index::Tick(FRAME_TIME);
		weather::Tick(FRAME_TIME);

		input::ResetInput();

		return true;
	}
	else
	{
		return false;
	}
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
	cfg::LoadCfg();

	//....................................... CONNECT TO SERVER

	//network::Init();

	//....................................... INITIALIZE SDL2

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) != 0)
		return 0;

	//Game Controller 1 handler
	SDL_Joystick* gGameController = NULL;

	//Check for joysticks
	if (SDL_NumJoysticks() < 1)
	{
		printf("Warning: No joysticks connected!\n");
	}
	else {
		//Load joystick
		gGameController = SDL_JoystickOpen(0);
		if (gGameController == NULL) { printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError()); }
	}

	//request gl 3.3 'context'
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	/* Turn on double buffering with a 24bit Z buffer.
	* You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	//turn on antialiasing
	#ifndef DEF_OLDSKOOL
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	#endif

	sdl_window = SDL_CreateWindow("WelCUM home master Nya~", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		cfg::iWinX, cfg::iWinY, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!sdl_window) /* Die if creation failed */
		throw BTError();

	/* Create our opengl context and attach it to our window */
	sdl_glcontext = SDL_GL_CreateContext(sdl_window);

	/* This makes our buffer swap syncronized with the monitor's vertical refresh */
	SDL_GL_SetSwapInterval(1);

	//input event
	SDL_Event e;

	//SDL_ShowCursor(false);
	//toggle cursor
	SDL_SetRelativeMouseMode((SDL_bool)true);

	//....................................... INITIALIZE GLAD

	//load all OpenGL function pointers
	//if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "ERROR: gladLoadGLLoader failed!" << std::endl;
		return -1;
	}
	//set opengl viewport size to window size x,y,w,h
	glViewport(0, 0, cfg::iWinX, cfg::iWinY);

	//....................................... CONFIGURE GLOBAL OPENGL STATE

	//enable face culling
	glEnable(GL_CULL_FACE);
	//set culling mode
	glCullFace(GL_FRONT);
	//set front faces to clockwise
	//glFrontFace(GL_CW);
	glFrontFace(GL_CCW);

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//||||||||||||||||||||| ENGINE STARTS HERE |||||||||||||||||||||
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	//....................................... SCREEN QUAD (SHOULD GO IN GRAPHICS)
	//FOR DRAWING FRAMEBUFFER

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

	//create framebuffer
	glGenFramebuffers(1, &framebuffer_1);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);
	rendertexture_1.InitRenderTexture(cfg::iWinX / 2, cfg::iWinY, true);
	depthbuffer_1.InitRenderBuffer(cfg::iWinX / 2, cfg::iWinY, false);
	//depthbuffer_1.InitDepthTexture(cfg::iWinX / 2, cfg::iWinY, false);

	// configure second post-processing framebuffer
	glGenFramebuffers(1, &framebuffer_intermediate);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_intermediate);
	// create a color attachment texture
	unsigned int screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cfg::iWinX / 2, cfg::iWinY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer

	#ifdef DEF_BLIT_FRAME
	glEnable(GL_MULTISAMPLE); // Enable multisampling for anti-aliasing
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE); // Enable ATOC for texture alpha
	#endif

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- INITIALIZATION -------------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	graphics::Init();
	res::Init();
	index::Init();

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//--------------------------- MAIN LOOP ------------------------------------------------------------------------------------------
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	while (!quit)
	{
		//network::Recv2(PAK_TYPE_SETPOS);

		// per-frame time delta calculation
		time = (double)(SDL_GetTicks() / 1000.);
		time_delta = (float)time - time_last;
		time_last = time;

		//....................................... INPUT

		mDeltaX = 0; mDeltaY = 0;
		inputX = 0; inputY = 0;

		//e is an SDL_Event variable we've declared before entering the main loop
		while (SDL_PollEvent(&e))
		{
			//If user closes the window
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else
			{
				input::UpdateInput(e);
			}
		}

		//if (input::Get(input::key::atkhit))
		//{
		//	std::cout << "atkhit" << std::endl; // y no work
		//}

		if (input::Get(input::key::quit))
		{
			quit = true;
		}

		/*
		networkTimerTemp += delta_time;

		//put in network file uwu
		if (networkTimerTemp > 1.f / cfg::fSyncTimer)
		{
		while (networkTimerTemp > 1.f / cfg::fSyncTimer)
		networkTimerTemp -= 1.f / cfg::fSyncTimer;
		msg::set_unit_pose ptemp;
		ptemp.id = 0;
		//set positions
		//ptemp.px = actor_sphere->getGlobalPose().p.x;
		ptemp.px = unitP1.body.position.x;
		ptemp.py = unitP1.body.position.y;
		ptemp.pz = unitP1.body.position.z;
		//set rotations
		ptemp.vh = unitP1.v_yaw;
		ptemp.vv = unitP1.v_ptc;
		//network::SndMsg(PAK_TYPE_SETPOS, ptemp);
		network::SendMsg(MSG_SET_UNIT_POSE, &ptemp);
		}*/

		//....................................... PER-FRAME GAME LOGIC

		StepTick(time_delta);

		//....................................... BIND FRAMEBUFFER

		//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

		//....................................... CLEAR SCREEN

		//enable depth testing for game objects
		glEnable(GL_DEPTH_TEST);

		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);

		//....................................... RENDER ENTITIES

		//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

		glViewport(0, 0, cfg::iWinX / 2, cfg::iWinY);
		//double size
		//glViewport(0, 0, cfg::iWinX, cfg::iWinY * 2);

		// BUFFER 1 (LEFT SCREEN)
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		index::SetViewFocus(0x0u);
		graphics::SetFrontFace();
		index::Draw();
		//index::DrawPostShadow();

		// BUFFER 2 (RIGHT SCREEN)
		//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_2);
		//glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//index::SetViewFocus(0x1u);
		//index::Draw();
		//index::DrawPostShadow();

		//....................................... DRAW FRAMEBUFFER

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
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_2);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate);
		//glBlitFramebuffer(0, 0, cfg::iWinX / 2, cfg::iWinY, 0, 0, cfg::iWinX / 2, cfg::iWinY, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//mat_fb = glm::mat4(1.0f);
		//mat_fb = glm::translate(mat_fb, glm::vec3(0.5f, 0.f, 0.f));
		//mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));
		//// get matrix's uniform location and set matrix
		//pRes->screenshadercomplex.setMat4("transform", mat_fb);

		//glBindVertexArray(quadVAO);
		//glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		//glUniform1i(glGetUniformLocation(pRes->screenshadercomplex.ID, "screenTexture"), 0);
		////glUniform1i(glGetUniformLocation(pRes->screen_post.ID, "screenTexture"), 0);
		//glBindTexture(GL_TEXTURE_2D, screenTexture);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		#else
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		glViewport(0, 0, cfg::iWinX, cfg::iWinY);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default


		glFrontFace(GL_CW);

		//....................................... BUFFER 1

		pRes->screenshadercomplex.Use();
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_1);

		//set screen matrix
		glm::mat4 mat_fb = glm::mat4(1.0f);
		mat_fb = glm::translate(mat_fb, glm::vec3(-0.5f, 0.f, 0.f));
		mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));

		// get matrix's uniform location and set matrix
		pRes->screenshadercomplex.setMat4("transform", mat_fb);
		//pRes->screen_post.setMat4("transform", mat_fb);

		//pRes->screenshadercomplex.setVec3("csun", *(glm::vec3*)weather::SunColour());
		//pRes->screenshadercomplex.setVec3("camb", *(glm::vec3*)weather::AmbientColour());
		pRes->screenshadercomplex.setVec3("cfog", *(glm::vec3*)weather::FogColour());
		pRes->screenshadercomplex.SetFloat("ffog", *(float*)weather::FogDensity());

		//index::SetViewFocus(0u);
		//pRes->screenshadercomplex.setVec2("pointLights[0].pos", glm::vec2((index::player1->t.position.x + index::viewpos.x) * 0.1f, (index::player1->t.position.y + index::viewpos.y) * 0.1f * 0.75f));
		//pRes->screenshadercomplex.setVec3("pointLights[0].col", glm::vec3(1.f * 0.4f, 0.7f * 0.4f, 0.4f * 0.4f));
		//pRes->screenshadercomplex.setVec2("pointLights[1].pos", glm::vec2((index::player2->t.position.x + index::viewpos.x) * 0.1f, (index::player2->t.position.y + index::viewpos.y) * 0.1f * 0.75f));
		//pRes->screenshadercomplex.setVec3("pointLights[1].col", glm::vec3(1.f * 0.4f, 0.7f * 0.4f, 0.4f * 0.4f));

		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glUniform1i(glGetUniformLocation(pRes->screenshadercomplex.ID, "screenTexture"), 0);
		//glUniform1i(glGetUniformLocation(pRes->screen_post.ID, "screenTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, rendertexture_1.id);
		glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
		glUniform1i(glGetUniformLocation(pRes->screenshadercomplex.ID, "depthTexture"), 1);
		glBindTexture(GL_TEXTURE_2D, depthbuffer_1.id);
		//glActiveTexture(GL_TEXTURE2); // activate the texture unit first before binding texture
		//glUniform1i(glGetUniformLocation(pRes->screenshadercomplex.ID, "shadowTexture"), 2);
		//glBindTexture(GL_TEXTURE_2D, shadowbuffer_1.id);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//index::SetViewFocus(0u);
		//index::DrawPostShadow();

		// DRAW SECOND TIME
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

		pRes->shader_screen_post.Use();

		mat_fb = glm::mat4(1.0f);
		mat_fb = glm::translate(mat_fb, glm::vec3(-0.5f, 0.f, 0.f));
		mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));
		pRes->shader_screen_post.setMat4("transform", mat_fb);

		glUniform1i(glGetUniformLocation(pRes->shader_screen_post.ID, "screenTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, rendertexture_1.id);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//....................................... BUFFER 2

		pRes->screenshadercomplex.Use();
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_2);

		mat_fb = glm::mat4(1.0f);
		mat_fb = glm::translate(mat_fb, glm::vec3(-0.5f, 0.f, 0.f));
		mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));

		// get matrix's uniform location and set matrix
		pRes->screenshadercomplex.setMat4("transform", mat_fb);

		index::SetViewFocus(1u);
		pRes->screenshadercomplex.setVec2("pointLights[0].pos", glm::vec2((index::player1->t.position.x + index::viewpos.x) * 0.1, (index::player1->t.position.y + index::viewpos.y) * 0.1));
		pRes->screenshadercomplex.setVec2("pointLights[1].pos", glm::vec2((index::player2->t.position.x + index::viewpos.x) * 0.1, (index::player2->t.position.y + index::viewpos.y) * 0.1));

		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glUniform1i(glGetUniformLocation(pRes->screenshadercomplex.ID, "screenTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, rendertexture_2.id);
		glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
		glUniform1i(glGetUniformLocation(pRes->screenshadercomplex.ID, "depthTexture"), 1);
		glBindTexture(GL_TEXTURE_2D, depthbuffer_2.id);
		//glActiveTexture(GL_TEXTURE2); // activate the texture unit first before binding texture
		//glUniform1i(glGetUniformLocation(pRes->screenshadercomplex.ID, "shadowTexture"), 2);
		//glBindTexture(GL_TEXTURE_2D, shadowbuffer_2.id);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//index::SetViewFocus(0u);
		//index::DrawPostShadow();

		// DRAW SECOND TIME
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		pRes->shader_screen_post.Use();
		mat_fb = glm::mat4(1.0f);
		mat_fb = glm::translate(mat_fb, glm::vec3(0.5f, 0.f, 0.f));
		mat_fb = glm::scale(mat_fb, glm::vec3(0.5f, 1.f, 1.f));
		pRes->shader_screen_post.setMat4("transform", mat_fb);

		glUniform1i(glGetUniformLocation(pRes->shader_screen_post.ID, "screenTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, rendertexture_2.id);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		#endif

		//....................................... RENDER GUI

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

											  // Player 1, X Start
		float p1_x_start = -(int)cfg::iWinX / 2;
		float p1_y_start = -(int)cfg::iWinY / 2;
		float p2_x_start = 0;
		float p2_y_start = -(int)cfg::iWinY / 2;

		glFrontFace(GL_CCW);

		//....................................... SWAP BUFFERS

		SDL_GL_SwapWindow(sdl_window);
	}

	//....................................... END PROGRAM

	index::End();
	res::End();

	SDLClose();

	//GOODBYE
	return 0;
}