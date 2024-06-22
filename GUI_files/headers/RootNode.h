// Root node

// include the files we need
#include "Node.h"
#include "Config.h"
#include "MenuScreen.h"
#include "SimpleButton.h"

// Root node publicly inherits from Node
class RootNode : public Node {
public:
    // Constructor                                              // initializer list
    RootNode(SDL_Renderer* renderer_in, Node* parentNode_in) : Node(renderer_in, parentNode_in) {
        
        // creating screens for the node
        MenuScreen* screen1 = createMenuScreen();
        MenuScreen* screen2 = createMenuScreen();

        // create images and text to put on the screen using an ArcadeTexture object
        ArcadeTexture* screen1Background = createImage(renderer_in, "rootNodeImages/rootNodeScreenBackground.png", true);
        ArcadeTexture* screen2Background = createImage(renderer_in, "rootNodeImages/rootNodeScreenBackground.png", true);

        ArcadeTexture* screen1Text = createSimpleText(renderer_in, "fonts/retro/italic.ttf", 100, "screen 1", 255, 255, 0);
        screen1Text->setPosition(windowWidth / 2 - screen1Text->getWidth() / 2, 25);

        ArcadeTexture* screen2Text = createSimpleText(renderer_in, "fonts/retro/italic.ttf", 100, "screen 2", 255, 255, 0);
        screen2Text->setPosition(windowWidth / 2 - screen2Text->getWidth() / 2, 25);

        // add the images and text to the screen after creating them
        screen1->addTextureToScreen(screen1Background);
        screen1->addTextureToScreen(screen1Text);

        screen2->addTextureToScreen(screen2Background);
        screen2->addTextureToScreen(screen2Text);

        // make a text button to put on the screen1
        SimpleButton* button1 = createSimpleTextButton(renderer_in, "fonts/pixel/classic.ttf", 30, "gotoscreen2", 255, 0, 0);
        button1->setButtonPosition(windowWidth / 2 - button1->getWidth() / 2, screen1Text->getY() + screen1Text->getHeight() + 50);

        // give this button an action
        button1->setButtonAction(createAction(MOVE_SCREENS, screen2));

        // make a text button to put on the screen2
        SimpleButton* button2 = createSimpleTextButton(renderer_in, "fonts/pixel/classic.ttf", 30, "gotoscreen1", 255, 0, 0);
        button2->setButtonPosition(windowWidth / 2 - button2->getWidth() / 2, screen2Text->getY() + screen2Text->getHeight() + 50);

        // give this button an action
        button2->setButtonAction(createAction(MOVE_SCREENS, screen1));

        // add the button to their screens
        screen1->addButtonToScreen(button1);
        screen2->addButtonToScreen(button2);

        // add the screens to the Node
        this->addScreen(screen1);
        this->addScreen(screen2);

        // tell the node the current screen
        this->setCurrentScreen(screen1);
    }
};