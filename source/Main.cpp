// main.cpp
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "Config.h"
#include <stdio.h>
#include "RootNode.h"

// declaring pointer to objects that main needs, initialize to null
// these objects are created in main because they are needed for the outermost loop
SDL_Window* arcadeSystemWindow = nullptr;
SDL_Renderer* arcadeSystemRenderer = nullptr;
TTF_Font* font = nullptr;

// initialization function, initializes above objects and calls some SDL initialization functions
bool init()
{
	// Initialization flag
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		// Create window in the center of the screen
		arcadeSystemWindow = SDL_CreateWindow("Arcade System", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
		if (arcadeSystemWindow == NULL)	
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// initialize renderer, set the background as white for the wwindow, sync the renderer with the monitor refresh rate
			arcadeSystemRenderer = SDL_CreateRenderer(arcadeSystemWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			SDL_SetRenderDrawColor(arcadeSystemRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		}
		// Initialize SDL_mixer
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			success = false;
		}
		// initialize true type font 
		if (TTF_Init() == -1)
		{
			printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
			success = false;
		}

	}
	return success;
}

int main(int argc, char* argv[])
{	
	// first call initialization
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	// if initializes successfully then continue
	else
	{
		bool quit = false;
		RootNode currentNode(arcadeSystemRenderer, nullptr);
		while (!quit)
		{	
			// handle events on queue until empty
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				currentNode.update(&e);
				// user requests quit by clicking window X
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}
			currentNode.update(nullptr);
			currentNode.render(arcadeSystemRenderer);
			SDL_RenderPresent(arcadeSystemRenderer);
		}
	}
	
	return 0;
}