// RootNode.h

#include "Node.h"
#include "Config.h"
#include "MenuScreen.h" 
#include "SimpleButton.h" 
#include "HighscoresNode.h"
#include "GameMenuNode.h"
#include "OptionsNode.h"
 
class RootNode : public Node {      // RootNode inherits from Node
private:
    // Before creating pointers to the children nodes, define them first
    HighscoresNode* highscoresNode; 
    GameMenuNode* gameMenuNode;
    OptionsNode* optionsNode;
 
public:
    
    // RootNode constructor
    RootNode(SDL_Renderer* renderer_in = nullptr, Node* parentNode_in = nullptr): Node(renderer_in, parentNode_in) {

        // First we create the children nodes on the heap
        highscoresNode = new HighscoresNode(getRenderer(), this);
        gameMenuNode = new GameMenuNode(getRenderer(), this);
        optionsNode = new OptionsNode(getRenderer(), this);
        // After the constructors for the three nodes above are executed, the rest of the code below is executed

        // Every node has a list of pointers to its children
        children.push_back(highscoresNode);
        children.push_back(gameMenuNode);
        children.push_back(optionsNode);

        // Create a screen for the RootNode
        MenuScreen* rootNodeScreen = createMenuScreen();
 
        // Create and add a background image to the screen of the RootNode
        ArcadeTexture* rootNodeScreenBackground = createImage(renderer_in, "rootNodeImages/rootNodeScreenBackground.png", true);
        rootNodeScreen->addTextureToScreen(rootNodeScreenBackground);
 
        // Create a title and put it on the screen
        ArcadeTexture* mainMenuText = createSimpleText(renderer_in, "fonts/retro/italic.ttf", 100, "MAIN MENU", 255, 255, 0);
        mainMenuText->setPosition(windowWidth / 2 - mainMenuText->getWidth() / 2, 25);
        rootNodeScreen->addTextureToScreen(mainMenuText);

        // Create a button that takes us to the HighscoresNode
        SimpleButton* highscoresButton = createSimpleTextButton(renderer_in, "fonts/pixel/classic.ttf", 30, "HIGHSCORES", 255, 0, 0);
        highscoresButton->setButtonPosition(windowWidth / 2 - highscoresButton->getWidth() / 2,
                                            mainMenuText->getY() + mainMenuText->getHeight() + 50);
        // Link this button to the highscoresNode by creating an action and passing the highscoresNode pointer as the action parameter
        highscoresButton->setButtonAction(createAction(MOVE_NODES, highscoresNode));
        
        // Create a button that takes us to the GameMenuNode
        SimpleButton* gameMenuButton = createSimpleTextButton(renderer_in, "fonts/pixel/classic.ttf", 30, "GAME MENU", 255, 0, 0);
        gameMenuButton->setButtonPosition(windowWidth / 2 - gameMenuButton->getWidth() / 2,
                                          highscoresButton->getY() + highscoresButton->getHeight() + 25);
        // Link this button to the gameMenuNode by creating an action and passing the gameMenuNode pointer as the action parameter
        gameMenuButton->setButtonAction(createAction(MOVE_NODES, gameMenuNode));
 
        // Create a button that will take us to the OptionsNode
        SimpleButton* optionsButton = createSimpleTextButton(renderer_in, "fonts/pixel/classic.ttf", 30, "OPTIONS", 255, 0, 0);
        optionsButton->setButtonPosition(windowWidth / 2 - optionsButton->getWidth() / 2, 
                                         gameMenuButton->getY() + gameMenuButton->getHeight() + 25);
        // link this button to the optionsNode by creating an action and passing the optionsNode pointer as the action parameter
        optionsButton->setButtonAction(createAction(MOVE_NODES, optionsNode));

        // Add the buttons that we previously created to the RootNode screen
        rootNodeScreen->addButtonToScreen(highscoresButton);
        rootNodeScreen->addButtonToScreen(gameMenuButton);
        rootNodeScreen->addButtonToScreen(optionsButton);
        
        // Add the screen to the RootNode and set it as the current
        this->addScreen(rootNodeScreen);
        this->setCurrentScreen(rootNodeScreen);
    }
};