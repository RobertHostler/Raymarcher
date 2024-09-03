#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "./Shader.hpp"
#include "./Camera.hpp"
#include "./Model.hpp"


// global variables
int screenHeight = 720;
int screenWidth = 1280;
SDL_Window *graphicsApplicationWindow = nullptr;
SDL_GLContext openGLContext = nullptr;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *screenSurface;

const int FrameRate = 60;
const int FrameDelay = 1000/FrameRate;
Uint32 FrameStartTime;
int FrameUpdateDuration;
float Time = 0.0f;
float deltaT = (float)FrameDelay/1000.0f;
bool Quit = false;

glm::vec2 mouseDelta;
float mouseSensitivity = 0.075;
bool mouseIsFree = false;


 
void startWindow(){
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0){
		window = SDL_CreateWindow(
			"SDF and Raymarch Rendering",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			screenWidth,
			screenHeight,
			SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL
		);

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED && SDL_RENDERER_PRESENTVSYNC);

		screenSurface = SDL_GetWindowSurface(window);
	} else if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Could not load SDL Subsystems" << std::endl;
		exit(1);
	}

	if (window == nullptr){
		std::cout << "window is null" << std::endl;
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	openGLContext = SDL_GL_CreateContext(window);
	if (openGLContext == nullptr){
		std::cout << "Could not load OpenGL" << std::endl;
		exit(1);
	}

	// initializes the GLAD openGL loader
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)){
		std::cout << "GLAD failed to start" << std::endl;
		exit(1);
	}

	glViewport(0, 0, screenWidth, screenHeight);

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void closeWindow(){
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(screenSurface);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


int main(){
	startWindow();

	// Scene scene;
	Camera camera;
	float camFOV = 75.0f;
	float camZoomRate = 1.0f;
	float maxFOV = 150.0f;
	float minFOV = 10.0f;
	float zNear = 0.1f;
	float zFar = 100.0f;
	// these should go in the camera class later

	// the fullscreen quad used as a framebuffer for Raymarching and SDFs
	Model quad("./Assets/Models/quad.obj");

	//Shader quadShader("./Shaders/sdfVert.glsl", "./Shaders/sdfFrag.glsl");
	Shader quadShader("./Shaders/raymarchVert.glsl", "./Shaders/raymarchFrag.glsl");

	glm::mat4 projMat = glm::mat4(1.0f);

	// convenient initial position for SDF rendering
	camera.position = glm::vec3(0.0f, 0.1f, -2.0f);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// the rendering loop
	while(Quit == false){
		
		// input polling
		SDL_Event event;
		while(SDL_PollEvent(&event) != 0){
			if (event.type == SDL_QUIT){
				Quit = true;
			}

			if(event.type == SDL_MOUSEMOTION && mouseIsFree == false){
				int mouse_x, mouse_y;
				SDL_PumpEvents();
				SDL_GetMouseState(&mouse_x, &mouse_y);

				// here the mouse is always reset to the centre of the screen every frame,
				// so the current mouse_x - screenWidth/2 is, in effect, a mouseDelta
				mouseDelta = glm::vec2(
					mouseSensitivity * (float)(mouse_x - screenWidth/2),
					mouseSensitivity * (float)(mouse_y - screenHeight/2)
				);
				camera.mouseLook(mouseDelta);
			}

			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
				mouseIsFree = !mouseIsFree;
			}

			if(event.type == SDL_MOUSEWHEEL){
				if(event.wheel.y > 0){
					camFOV -= camZoomRate;
				}
				if(event.wheel.y < 0){
					camFOV += camZoomRate;
				}
				if(camFOV > maxFOV){
					camFOV = maxFOV;
				}
				if(camFOV < minFOV){
					camFOV = minFOV;
				}

				projMat = glm::perspective(
					glm::radians(camFOV),
					(float)screenWidth/(float)screenHeight, zNear, zFar
				);
			}
		}

		const Uint8 *state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_Q]){
			Quit = true;
		}

		if(mouseIsFree){
			SDL_SetRelativeMouseMode(SDL_FALSE);
		} else {
			// sets the mouse coords back to the centre of the screen; you won't see this,
			// because the mouse is hidden.
			SDL_WarpMouseInWindow(graphicsApplicationWindow, screenWidth/2, screenHeight/2);
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}

		if (state[SDL_SCANCODE_LSHIFT]){
			camera.speed = 3.0f*camera.defaultSpeed;
		} else {
			camera.speed = camera.defaultSpeed;
		}

		if (state[SDL_SCANCODE_W]){
			camera.moveForward();
		}
		if (state[SDL_SCANCODE_S]){
			camera.moveBackward();
		}
		if (state[SDL_SCANCODE_A]){
			camera.moveLeft();
		}
		if (state[SDL_SCANCODE_D]){
			camera.moveRight();
		}

		float time = ((float)SDL_GetTicks()/1000.0);
		FrameStartTime = SDL_GetTicks();

		// SDF rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		quadShader.UseShader();
		
		// the time uniform is not currently used in the raymarcher, but in future
		// updates it might be.
		//quadShader.SetUniform1f("time", Time);
		
		quadShader.SetUniform3fv("camView", camera.view);
		glm::vec3 camRight = glm::cross(camera.view, camera.up);
		quadShader.SetUniform3fv("camRight", camRight);
		quadShader.SetUniform3fv("camUp", camera.up);
		quadShader.SetUniform3fv("camPos", camera.position);
		quad.Draw(quadShader);

		SDL_GL_SwapWindow(window);
		FrameUpdateDuration = SDL_GetTicks() - FrameStartTime;
		deltaT = (float)FrameDelay/1000.0f;
		Time += deltaT;
		if (FrameDelay > FrameUpdateDuration){
			SDL_Delay(FrameDelay - FrameUpdateDuration);
		}
	}

	closeWindow();
	return 0;
}
