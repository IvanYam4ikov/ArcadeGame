// GameMenuNode.h

#include "Node.h"
#include "MenuScreen.h"
#include "SimpleButton.h"
#include "Config.h"

class GameMenuNode : public Node {		// GameMenuNode inherits from Node
public:
	GameMenuNode(SDL_Renderer* renderer_in = nullptr, Node* parentNode_in = nullptr) : Node(renderer_in, parentNode_in) {
		
		// Declare and initialize screens for three games
    	MenuScreen* tetrisScreen = createMenuScreen();
		MenuScreen* froggerScreen = createMenuScreen();
		MenuScreen* brickBreakerScreen = createMenuScreen();

		// Set the backgroud for the tetris screen
		ArcadeTexture* tetrisBackgroundImage = createImage(renderer_in, "gameMenuNodeImages/gameMenuNodeScreenBackgroundTexture.png", true);
		tetrisScreen->addTextureToScreen(tetrisBackgroundImage);

		// Add the title to the tetris screen
		ArcadeTexture* tetrisGameMenuText = createSimpleText(renderer_in, "fonts/retro/italic.ttf",100, "TETRIS", 255, 255, 0);
		tetrisGameMenuText->setPosition(windowWidth / 2 - tetrisGameMenuText->getWidth() / 2, 20);
		tetrisScreen->addTextureToScreen(tetrisGameMenuText);

		// Add the button that takes us to the frogger screen
		SimpleButton* tetrisPreviousButton = createSimpleButton(renderer_in, "gameMenuNodeImages/previousScreenButton.png");
		tetrisPreviousButton->setButtonPosition(25, windowHeight / 2 - tetrisPreviousButton->getHeight() / 2);
		tetrisPreviousButton->setButtonAction(createAction(MOVE_SCREENS, froggerScreen));
		tetrisScreen->addButtonToScreen(tetrisPreviousButton);

		// Add the button that takes us to the tetris game
		SimpleButton* tetrisButton = createSimpleButton(renderer_in, "gameMenuNodeImages/tetrisGameButton.png");
		tetrisButton->setButtonPosition(windowWidth / 2 - tetrisButton->getButtonTexture()->getWidth() / 2,
		windowHeight / 2 - tetrisButton->getHeight() / 2);
		tetrisButton->setButtonAction(createAction(MOVE_NODES, getParentNode()));
		tetrisScreen->addButtonToScreen(tetrisButton);

		// Add the button that takes us to the brickbreaker screen
		SimpleButton* tetrisNextButton = createSimpleButton(renderer_in, "gameMenuNodeImages/nextScreenButton.png");
		tetrisNextButton->setButtonPosition(windowWidth - tetrisNextButton->getWidth() - 25, windowHeight / 2 - tetrisNextButton->getWidth() / 2);
		tetrisNextButton->setButtonAction(createAction(MOVE_SCREENS, brickBreakerScreen));
		tetrisScreen->addButtonToScreen(tetrisNextButton);

		// Add the button that takes us to the main menu
		SimpleButton* tetrisMainMenuButton = createSimpleTextButton(renderer_in, "fonts/pixel/classic.ttf", 30, "MAIN MENU", 255, 0, 0);
		tetrisMainMenuButton->setButtonPosition(windowWidth / 2 - tetrisMainMenuButton->getWidth() / 2, windowHeight - 50);
		tetrisScreen->addButtonToScreen(tetrisMainMenuButton);
		tetrisMainMenuButton->setButtonAction(createAction(MOVE_NODES, getParentNode()));

		// Add the screen to the current node
		this->addScreen(tetrisScreen);


		// Set the background for the brickbreaker screen
		ArcadeTexture* brickBreakerBackgroundImage = createImage(renderer_in, "gameMenuNodeImages/gameMenuNodeScreenBackgroundTexture.png", true);
		brickBreakerScreen->addTextureToScreen(brickBreakerBackgroundImage);

		// Add the title to the brickbreaker screen
		ArcadeTexture* brickBreakerGameMenuText = createSimpleText(renderer_in, "fonts/retro/italic.ttf", 100, "BREAKOUT", 255, 255, 0);
		brickBreakerGameMenuText->setPosition(windowWidth / 2 - brickBreakerGameMenuText->getWidth() / 2, 20);
		brickBreakerScreen->addTextureToScreen(brickBreakerGameMenuText);

		// Add the button that takes us to the tetris screen
		SimpleButton* brickBreakerPreviousButton = createSimpleButton(renderer_in, "gameMenuNodeImages/previousScreenButton.png");
		brickBreakerPreviousButton->setButtonPosition(25, windowHeight / 2 - brickBreakerPreviousButton->getHeight() / 2);
		brickBreakerPreviousButton->setButtonAction(createAction(MOVE_SCREENS, tetrisScreen));
		brickBreakerScreen->addButtonToScreen(brickBreakerPreviousButton);

		// Add the button that takes us to the brickbreaker game		
		SimpleButton* brickBreakerButton = createSimpleButton(renderer_in, "gameMenuNodeImages/brickBreakerGameButton.png");
		brickBreakerButton->setButtonPosition(windowWidth / 2 - brickBreakerButton->getWidth() / 2, windowHeight / 2 - brickBreakerButton->getHeight() / 2);
		brickBreakerButton->setButtonAction(createAction(MOVE_NODES, parentNode_in));
		brickBreakerScreen->addButtonToScreen(brickBreakerButton);

		// Add the button that takes us to the frogger screen
		SimpleButton* brickBreakerNextButton = createSimpleButton(renderer_in, "gameMenuNodeImages/nextScreenButton.png");
		brickBreakerNextButton->setButtonPosition(windowWidth - brickBreakerNextButton->getWidth() - 25, windowHeight / 2 - brickBreakerNextButton->getWidth() / 2);
		brickBreakerNextButton->setButtonAction(createAction(MOVE_SCREENS, froggerScreen));
		brickBreakerScreen->addButtonToScreen(brickBreakerNextButton);

		// Add the button that takes us to the main menu
		SimpleButton* brickBreakerMainMenuButton = createSimpleTextButton(renderer_in, "fonts/pixel/classic.ttf", 30, "MAIN MENU", 255, 0, 0);
		brickBreakerMainMenuButton->setButtonPosition(windowWidth / 2 - brickBreakerMainMenuButton->getWidth() / 2, windowHeight - 50);
		brickBreakerMainMenuButton->setButtonAction(createAction(MOVE_NODES, getParentNode()));
		brickBreakerScreen->addButtonToScreen(brickBreakerMainMenuButton);

		// Add the brickbreaker screen to the GameMenuNode
		this->addScreen(brickBreakerScreen);

		
		// Same actions as we did before for the frogger screen
		ArcadeTexture* froggerBackgroundImage = createImage(renderer_in, "gameMenuNodeImages/gameMenuNodeScreenBackgroundTexture.png", true);
		froggerScreen->addTextureToScreen(froggerBackgroundImage);

		ArcadeTexture* froggerGameMenuText = createSimpleText(renderer_in, "fonts/retro/italic.ttf", 100, "FROGGER", 255, 255, 0);
		froggerGameMenuText->setPosition(windowWidth / 2 - froggerGameMenuText->getWidth() / 2, 20);
		froggerScreen->addTextureToScreen(froggerGameMenuText);

		SimpleButton* froggerPreviousButton = createSimpleButton(renderer_in, "gameMenuNodeImages/previousScreenButton.png");
		froggerPreviousButton->setButtonPosition(25, windowHeight / 2 - froggerPreviousButton->getHeight() / 2);
		froggerPreviousButton->setButtonAction(createAction(MOVE_SCREENS, brickBreakerScreen));
		froggerScreen->addButtonToScreen(froggerPreviousButton);

		SimpleButton* froggerButton = createSimpleButton(renderer_in, "gameMenuNodeImages/froggerGameButton.png");
		froggerButton->setButtonPosition(windowWidth / 2 - froggerButton->getWidth() / 2, windowHeight / 2 - froggerButton->getHeight() / 2);
		froggerButton->setButtonAction(createAction(MOVE_NODES, getParentNode()));
		froggerScreen->addButtonToScreen(froggerButton);
		
		SimpleButton* froggerNextButton = createSimpleButton(renderer_in, "gameMenuNodeImages/nextScreenButton.png");
		froggerNextButton->setButtonPosition(windowWidth - froggerNextButton->getWidth() - 25, windowHeight / 2 - froggerNextButton->getWidth() / 2);
		froggerNextButton->setButtonAction(createAction(MOVE_SCREENS, tetrisScreen));
		froggerScreen->addButtonToScreen(froggerNextButton);

		SimpleButton* froggerMainMenuButton = createSimpleTextButton(renderer_in, "fonts/pixel/classic.ttf", 30, "MAIN MENU", 255, 0, 0);
		froggerMainMenuButton->setButtonPosition(windowWidth / 2 - froggerMainMenuButton->getWidth() / 2, windowHeight - 50);
		froggerMainMenuButton->setButtonAction(createAction(MOVE_NODES, getParentNode()));
		froggerScreen->addButtonToScreen(froggerMainMenuButton);
		
		this->addScreen(froggerScreen);

		// set the tetris screen as the current screen
		this->setCurrentScreen(tetrisScreen);
	}
};