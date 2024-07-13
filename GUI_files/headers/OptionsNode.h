// OptionsNode.h

#include "Node.h"
#include "MenuScreen.h"
#include "OptionsButton.h"
#include "SDL_mixer.h"
 
class OptionsNode : public Node {       // OptionsNode inherits from Node
public:
    
    // Declare the music pointers and initialize them to nullptr
    Mix_Music* music0 = nullptr;
    Mix_Music* music1 = nullptr;
    Mix_Music* music2 = nullptr; 

    // Constructor for OptionsNode
    OptionsNode(SDL_Renderer* renderer_in = nullptr, Node* parentNode_in = nullptr) : Node(renderer_in, parentNode_in)
    {   
        // Create a screen for the OptionsNode and add the background image to the screen
        MenuScreen* optionsNodeScreen = createMenuScreen();
        ArcadeTexture* optionsNodeScreenBackground = createImage(renderer_in, "optionsNodeImages/optionsNodeScreen.png", true);
        optionsNodeScreen->addTextureToScreen(optionsNodeScreenBackground);

        // Create a title that says "Options" and add it to the screen
        ArcadeTexture* optionsText = createSimpleText(renderer_in, "fonts/retro/italic.ttf", 100, "OPTIONS", 255, 255, 0);
        optionsText->setPosition(windowWidth / 2 - optionsText->getWidth()/2, 20);
        optionsNodeScreen->addTextureToScreen(optionsText);

        // Create an option button that will control sound on/off
        OptionsButton* soundOptionButton = createOptionsButton(renderer_in, "fonts/retro/italic.ttf", 60, "SOUND", 255, 255, 0);
        soundOptionButton->setButtonPosition(50, 150);
        
        // Create an action that will turn off the sound and add it to the button
        Action* soundOff = createAction(CHANGE_SOUND, 0);
        ArcadeTexture* soundOffText = createSimpleText(renderer_in, "fonts/pixel/classic.ttf", 30, "OFF", 0, 255, 0);
        soundOffText->setPosition(300, 170);
        soundOptionButton->addOption(soundOff, soundOffText);
        
        // Create an action that turns on the sound and add it to the button
        Action* soundOn = createAction(CHANGE_SOUND, 1);
        ArcadeTexture* soundOnText = createSimpleText(renderer_in, "fonts/pixel/classic.ttf", 30, "ON", 0, 255, 0);
        soundOnText->setPosition(300, 170);
        soundOptionButton->addOption(soundOn, soundOnText);

        // Create an option button that allows us to set the music
        OptionsButton* musicOptionButton = createOptionsButton(renderer_in, "fonts/retro/italic.ttf", 60, "MUSIC", 255, 255, 0);
        musicOptionButton->setButtonPosition(50, 250);
        
        // Create an action that turns off the music, create an image that shows this action, and add this action to the button
        music0 = nullptr;
        Action* noMusic = createAction(CHANGE_MUSIC, music0);
        ArcadeTexture* noMusicImage = createImage(renderer_in, "optionsNodeImages/noMusic.png");
        noMusicImage->setPosition(300, 270);
        musicOptionButton->addOption(noMusic, noMusicImage);
        
        // Create an action that turns on music1, create and image that represents it, and add the action to the button
        music1 = Mix_LoadMUS("optionsNodeImages/Classy-8-Bit.wav");
        Action* intenseMusic = createAction(CHANGE_MUSIC, music1);
        ArcadeTexture* intenseMusicImage = createImage(renderer_in, "optionsNodeImages/intenseMusicOptionTexture.png");
        intenseMusicImage->setPosition(300, 270);
        musicOptionButton->addOption(intenseMusic, intenseMusicImage);

        // Create and action that turns on music2, create an image that represents it, and add the action to the button
        music2 = Mix_LoadMUS("optionsNodeImages/Retro-80s-Technology.wav");
        Action* calmMusic = createAction(CHANGE_MUSIC, music2);
        ArcadeTexture* calmMusicImage = createImage(renderer_in, "optionsNodeImages/calmMusicOptionTexture.png", false);
        calmMusicImage->setPosition(300, 270);
        musicOptionButton->addOption(calmMusic, calmMusicImage);

        // Create a button that takes us to the main menu
        SimpleButton* mainMenuButton = createSimpleTextButton(renderer_in,"fonts/pixel/classic.ttf", 30, "MAIN MENU", 255,0,0);
        mainMenuButton->setButtonPosition(windowWidth / 2 - mainMenuButton->getWidth() / 2, windowHeight - 50);
        mainMenuButton->setButtonAction(createAction(MOVE_NODES, getParentNode())); // Here we are using a shortcut.
        // createAction returns a pointer to the action and we can put it as the argument straightaway

        // Add the three buttons that we created earlier to the options screen
        optionsNodeScreen->addButtonToScreen(mainMenuButton);
        optionsNodeScreen->addButtonToScreen(soundOptionButton);
        optionsNodeScreen->addButtonToScreen(musicOptionButton);

        // Add the screen to the options node and set it as current
        this->addScreen(optionsNodeScreen);
        this->setCurrentScreen(optionsNodeScreen);
    }
    
    // Destructor to free memory allocated for music
    virtual ~OptionsNode() 
    {
        printf("FREEING MUSIC FROM OPTIONS DESTRUCTOR=============\n");
        if (music1 != nullptr)
        {
            Mix_FreeMusic(music1);
            music1 = nullptr;
        }
        if (music2 != nullptr)
        {
            Mix_FreeMusic(music2);
            music2 = nullptr;
        }
    }
};