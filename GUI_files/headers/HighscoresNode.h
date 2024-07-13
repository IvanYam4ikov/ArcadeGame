// HighscoresNode.h

#include "Node.h"
#include "MenuScreen.h"
#include "SimpleButton.h"
#include "Config.h"

class HighscoresNode : public Node {        // HighscoresNode inherits from Node
public:
    
    // Constructor
    HighscoresNode(SDL_Renderer* renderer_in = nullptr, Node* parentNode_in = nullptr) : Node(renderer_in, parentNode_in) {
        
        // Create a screen for the HighscoresNode and add the background picture to the created screen
        MenuScreen* highscoresScreen = createMenuScreen();
        ArcadeTexture* highscoresScreenBackground = createImage(renderer_in, "highscoresNodeImages/highscoresNodeScreenBackground.png", true);
        highscoresScreen->addTextureToScreen(highscoresScreenBackground);

        // Add the title that says "Highschore" to the screen
        ArcadeTexture* highscoresText = createSimpleText(renderer_in, "fonts/retro/italic.ttf", 100, "HIGHSCORES", 255, 255, 0);
        highscoresText->setPosition(windowWidth / 2 - highscoresText->getWidth() / 2, 20);
        highscoresScreen->addTextureToScreen(highscoresText);

        // Add the button that takes us to the main menu
        SimpleButton* mainMenuButton = createSimpleTextButton(renderer_in, "fonts/pixel/classic.ttf", 30, "MAIN MENU", 255, 0, 0);
        mainMenuButton->setButtonPosition(windowWidth / 2 - mainMenuButton->getWidth() / 2, windowHeight - 50);
        mainMenuButton->setButtonAction(createAction(MOVE_NODES, getParentNode()));
        highscoresScreen->addButtonToScreen(mainMenuButton);

        // Now add the screen to the Node and set it as a curren screen
        this->addScreen(highscoresScreen);
        this->setCurrentScreen(highscoresScreen);
    }
};
