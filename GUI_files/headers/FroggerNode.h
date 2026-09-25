#ifndef FROGGER_NODE_H
#define FROGGER_NODE_H

#include "FroggerGame.h"
#include "Node.h"

class FroggerNode : public Node {
public:
    FroggerNode(SDL_Renderer* renderer = nullptr, Node* parent = nullptr)
        : Node(renderer, parent)
    {
        FroggerGame* game = new FroggerGame;
        game->setParentNode(parent);

        ArcadeTexture* hud = createSimpleText(renderer,
            "fonts/pixel/classic.ttf", 17,
            "SCORE 0   LIVES 3   LEVEL 1", 255, 255, 255);
        game->addTextureToScreen(hud);
        game->setHud(hud);

        ArcadeTexture* message = createSimpleText(renderer,
            "fonts/pixel/classic.ttf", 14,
            "ARROWS/WASD MOVE", 255, 255, 0);
        game->addTextureToScreen(message);
        game->setMessage(message);

        game->setisNewGame(true);
        game->setGameState(true);
        addScreen(game);
        setCurrentScreen(game);
    }
};

#endif
