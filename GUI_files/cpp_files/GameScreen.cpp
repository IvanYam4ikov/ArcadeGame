// GameScreen.cpp

#include "GameScreen.h"
#include <string>
#include <iostream>
#include <fstream>
#include <list>
using namespace std;

GameScreen:: ~GameScreen()
{
	for (int i = 0; i < getArcadeTextureList().size(); i++)
	{
		if (getArcadeTextureList()[i] != nullptr)
		{
			delete getArcadeTextureList()[i];
			getArcadeTextureList()[i] = nullptr;
		}
	}
	for (int i = 0; i < entities.size(); i++)
	{
		delete entities[i];
	}
}

void GameScreen::restartGame()
{
	gameState = true;
	isNewGame = true;
}

Action GameScreen::update(SDL_Event* event)
{
	Action newAction = { DO_NOTHING, nullptr };
	for (int i = 0; i < movingEntities.size(); i++)
	{
		movingEntities[i]->update(event);
	}
	if (!event)
	{
		return newAction;
	}
	else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_q)
	{
		newAction = { MOVE_NODES, parentNode };
		return newAction;
	}
	return newAction;
}

// method to render the current Screen
void GameScreen::render(SDL_Renderer* renderer)
{
	// the screen first renders all its textures
	for (int i = 0; i < getArcadeTextureList().size(); i++)
	{
		getArcadeTextureList()[i]->render(renderer);
	}

	// the screen then renders all its buttons
	for (int i = 0; i < getEntities().size(); i++)
	{
		if (getEntities()[i]->getState() == false)
			continue;
		getEntities()[i]->render(renderer);
	}
}

void GameScreen::submitScore(SDL_Renderer* ren)
{
	ArcadeTexture promptTexture;
	ArcadeTexture inputTexture;

	promptTexture.setRenderer(ren);
	inputTexture.setRenderer(ren);

	std::string inputText = "'NAME'";
	inputTexture.loadFromRenderedText("fonts/pixel/classic.ttf", 25, inputText, { 255,0,0 });
	promptTexture.loadFromRenderedText("fonts/pixel/classic.ttf", 25, "ENTER TEXT:", { 255,0,0 });
	SDL_StartTextInput();
	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		//The rerender text flag
		bool renderText = false;

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				//Handle backspace
				if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
				{
					//pop off character
					inputText.pop_back();
					renderText = true;
				}
			}
			else if (e.type == SDL_TEXTINPUT)
			{
					//Append character
				if (inputText.length() < 3)
				{
					inputText += e.text.text;
					renderText = true;
				}

			}
		}
		if (renderText)
		{
			//Text is not empty
			if (inputText != "")
			{
				//Render new text
				inputTexture.loadFromRenderedText("fonts/pixel/classic.ttf", 25, inputText, { 255,0,0 });
			}
			//Text is empty
			else
			{
				//Render space texture
				inputTexture.loadFromRenderedText("fonts/pixel/classic.ttf", 25, " ", { 255,0,0 });
			}
		}
		inputTexture.setPosition(CENTER_X, 300);
		inputTexture.setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL);
		promptTexture.setPosition(CENTER_X, 200);
		promptTexture.setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL);

		SDL_RenderClear(ren);
		promptTexture.render(ren);
		inputTexture.render(ren);
		SDL_RenderPresent(ren);
	}

	// write name and score to according file
	ifstream fileIn;
	string fileName = gameName + ".txt";
	fileIn.open(fileName);
	std::string currScore = "";
	std::list<playerScore> playerScores;
	playerScore ps;

	// first read all the information from file to a list
	getline(fileIn, currScore); // label
	while (getline(fileIn, currScore))
	{
		ps.name = currScore.substr(currScore.size() - 8, 3);
		cout << "Name: " << ps.name << endl;
		ps.score = stoi(currScore.substr(currScore.size() - 3, 3));
		playerScores.push_back(ps);
	}

	// add latest score to the list and sort
	ps = { gameName, inputText, gameScore };
	playerScores.push_back(ps);
	playerScores.sort(CompareEntries);

	if (playerScores.size() > 10)
	{
		playerScores.pop_back();
	}
	for (std::list<playerScore>::const_iterator iterator = playerScores.begin(), end = playerScores.end(); iterator != end; ++iterator) {
		printf("\n\nscore: %i", (*iterator).score);
	}
	fileIn.close();

	// now write the list back to the file (will overwrite)
	ofstream fileOut;
	fileOut.open(fileName);
	fileOut << gameName;
	int i = 1;
	std::string fileString = "";
	for (std::list<playerScore>::const_iterator iterator = playerScores.begin(), end = playerScores.end(); iterator != end; ++iterator)
	{
		if ((*iterator).score == 0)
		{
			fileString = "\n " + to_string(i) + " " + (*iterator).name + "  000";
		}
		else if ((*iterator).score < 10)
		{
			fileString = "\n " + to_string(i) + " " + (*iterator).name + "  00" + to_string((*iterator).score);
		}
		else if ((*iterator).score < 100)
		{
			fileString = "\n " + to_string(i) + " " + (*iterator).name + "  0" + to_string((*iterator).score);
		}
		else
		{
			fileString = "\n " + to_string(i) + " " + (*iterator).name + "  " + to_string((*iterator).score);
		}
		// this just formats because the number ten is two digits
		if (i == 10)
		{
			fileString = fileString.substr(0, 1) + fileString.substr(2, fileString.size() - 2);
		}
		fileOut << fileString;
		i++;
	}
	fileOut.close();
	SDL_StopTextInput();
}
