#ifndef CONTROLS_NODE_H
#define CONTROLS_NODE_H

#include "Config.h"
#include "MenuScreen.h"
#include "Node.h"
#include "SimpleButton.h"

class ControlsNode : public Node {
public:
    ControlsNode(SDL_Renderer* renderer = nullptr, Node* parent = nullptr)
        : Node(renderer, parent)
    {
        MenuScreen* screen = createMenuScreen();
        screen->addTextureToScreen(createImage(renderer,
            "gameMenuNodeImages/gameMenuNodeScreenBackgroundTexture.png", true));

        addCenteredText(screen, renderer, "CONTROLS", 58, 24,
            255, 255, 0, "fonts/retro/italic.ttf");

        addCenteredText(screen, renderer, "BRICK BREAKER", 25, 100,
            255, 65, 65);
        addCenteredText(screen, renderer,
            "LEFT/RIGHT OR A/D - MOVE PADDLE", 16, 133, 255, 255, 255);
        addCenteredText(screen, renderer,
            "SPACE - SERVE     P - PAUSE", 16, 156, 255, 255, 255);

        addCenteredText(screen, renderer, "TETRIS", 25, 202,
            65, 235, 255);
        addCenteredText(screen, renderer,
            "LEFT/RIGHT - MOVE     DOWN - SOFT DROP", 16, 235, 255, 255, 255);
        addCenteredText(screen, renderer,
            "UP/Z - ROTATE     SPACE - HARD DROP", 16, 258, 255, 255, 255);

        addCenteredText(screen, renderer, "FROGGER", 25, 304,
            90, 255, 90);
        addCenteredText(screen, renderer,
            "ARROW KEYS OR WASD - HOP", 16, 337, 255, 255, 255);
        addCenteredText(screen, renderer,
            "REACH ALL 5 HOMES BEFORE TIME RUNS OUT", 16, 360, 255, 255, 255);

        addCenteredText(screen, renderer,
            "ALL GAMES: Q/ESC - EXIT     N - RESTART", 14, 399,
            255, 255, 0);

        SimpleButton* back = createSimpleTextButton(renderer,
            "fonts/pixel/classic.ttf", 23, "MAIN MENU", 255, 0, 0);
        back->setButtonPosition(windowWidth / 2 - back->getWidth() / 2, 442);
        back->setButtonAction(createAction(MOVE_NODES, getParentNode()));
        screen->addButtonToScreen(back);

        addScreen(screen);
        setCurrentScreen(screen);
    }

private:
    void addCenteredText(MenuScreen* screen, SDL_Renderer* renderer,
        const std::string& text, int size, int y, int red, int green, int blue,
        const std::string& font = "fonts/pixel/classic.ttf")
    {
        ArcadeTexture* line = createSimpleText(renderer, font, size, text,
            red, green, blue);
        line->setPosition(CENTER_X, y);
        screen->addTextureToScreen(line);
    }
};

#endif
