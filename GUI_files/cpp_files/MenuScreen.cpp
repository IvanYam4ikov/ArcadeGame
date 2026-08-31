// MenuScreen.cpp

#include "MenuScreen.h"

MenuScreen::MenuScreen(){}

MenuScreen::~MenuScreen() 
{
	// delete all the textures and all the buttons
	for (int i = 0; i < getArcadeTextureList().size(); i++)
	{
		if (getArcadeTextureList()[i] != nullptr)
		{
			delete getArcadeTextureList()[i];
			getArcadeTextureList()[i] = nullptr;
		}
	}
	for (int i = 0; i < buttonList.size(); i++)
	{
		if (buttonList[i] != nullptr)
		{
			delete buttonList[i];
			buttonList[i] = nullptr;
		}
	}
}

Action MenuScreen::update(SDL_Event* event)
{
	Action newAction = { DO_NOTHING, nullptr };
	for (int i = 0; i < buttonList.size(); i++)
	{
		newAction = buttonList[i]->update(event);
		if (newAction.actionName != DO_NOTHING)
		{
			break;
		}
	}
	return newAction;
}

void MenuScreen::render(SDL_Renderer* renderer)
{   
	// the screen first renders all its textures
	for (int i = 0; i < getArcadeTextureList().size(); i++)
	{
		getArcadeTextureList()[i]->render(renderer);
	}
	
	// the screen then renders all its buttons
	for (int i = 0; i < getButtonList().size(); i++)
	{
		getButtonList()[i]->render(renderer);
	}
}
