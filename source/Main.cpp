// main.cpp

#include "SDL.h"
#include "SDL_mixer.h"
#include "RootNode.h"
#include "Config.h"

// Pointers to objects needed to initialize our program
SDL_Window* arcadeSystemWindow = nullptr;
SDL_Renderer* arcadeSystemRenderer = nullptr;
TTF_Font* font = nullptr;
Node* currentNode = nullptr;

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

// Starting point of program execution
int main(int argc, char* argv[])
{	
	// first call initialization
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	// If initializes successfully then continue
	else
	{
		// First create the root node which triggers creation of all nodes through constructor
 		RootNode rootNode(arcadeSystemRenderer, nullptr); 
		currentNode = &rootNode;
		
		// Pointer to a music object, holds the music currently being played in the progrm
		Mix_Music* currentMusic = nullptr;
		
		// Is the sound on or off
		bool soundState = false; 

		// Flag for main loop, is our program running? 
		bool quit = false;

		// GUI loop
		while (!quit)
		{	
			// Handle user events until none detected
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				// User requests quit by clicking window X
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				
				// Start update process by handling the next event on the event queue
				// Returns an action to execute in this top tier of the menu system
				Action newAction = currentNode->update(&e);
				
				// Check to see what the action returned by the node's update is
				switch (newAction.actionName)
				{
				case(MOVE_NODES):
				{
					// When moving nodes, first exit, set the current, then enter
					printf("\nMOVING NODES\n");
					currentNode->exitNode();
					currentNode = (Node*)(newAction.actionParameter);
					currentNode->enter();
					break;
				}
				case(CHANGE_SOUND):
				{
					// When changing sound to off, stop the music
					printf("\n changing sound\n\n");
					if (newAction.actionParameter == 0)
					{
						printf("\n sound turning off\n\n");
						Mix_HaltMusic();
						soundState = false;
					}
					else
					{
						soundState = true;
						printf("\n sound turning on\n\n");
						Mix_PlayMusic(currentMusic, -1);
					}
					break;
				}
				case(CHANGE_MUSIC):
				{
					// When changing music, check whether sound is on or not
					printf("\n changing music\n\n");
					printf("\n music pointer: %p\n\n", (Mix_Music*)(newAction.actionParameter));
					currentMusic = (Mix_Music*)(newAction.actionParameter);
					if (soundState == true)
					{
						if (currentMusic == nullptr)
						{
							Mix_HaltMusic();
						}
						else
						{
							Mix_PlayMusic(currentMusic, -1);
						}
					}
					break;
				}
				default:
					break;
				}
			}
			// After handling all events, update all graphics for animation
			currentNode->update(nullptr);

			// After updating, render
			currentNode->render(arcadeSystemRenderer);
			SDL_RenderPresent(arcadeSystemRenderer);
		}
	}
	// After quitting, free up SDL_Resources -- Note: rootNode was created on the stack so after quitting it will go
	// Out of scope and its destructor will be called which triggers the destructors of all other nodes
	Mix_CloseAudio();
	TTF_CloseFont(font);
	SDL_DestroyRenderer(arcadeSystemRenderer);
	SDL_DestroyWindow(arcadeSystemWindow);

	Mix_Quit();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	return 0;
}