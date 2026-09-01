#ifndef TETRIS_NODE_H
#define TETRIS_NODE_H

#include "Node.h"
#include "TetrisGame.h"

class TetrisNode : public Node {
public:
    TetrisNode(SDL_Renderer* renderer = nullptr, Node* parent = nullptr)
        : Node(renderer, parent)
    {
        TetrisGame* game = new TetrisGame;
        game->setParentNode(parent);

        ArcadeTexture* title = createSimpleText(renderer, "fonts/retro/italic.ttf",
            48, "TETRIS", 255, 255, 0);
        title->setPosition(18, 45);
        game->addTextureToScreen(title);

        ArcadeTexture* linesHud = createSimpleText(renderer,
            "fonts/pixel/classic.ttf", 20, "LINES 0", 255, 255, 255);
        linesHud->setPosition(22, 145);
        game->addTextureToScreen(linesHud);
        game->setLinesHud(linesHud);

        ArcadeTexture* nextLabel = createSimpleText(renderer,
            "fonts/pixel/classic.ttf", 18, "NEXT", 255, 255, 255);
        nextLabel->setPosition(500, 105);
        game->addTextureToScreen(nextLabel);

        ArcadeTexture* status = createSimpleText(renderer, "fonts/pixel/classic.ttf",
            15, "ARROWS MOVE", 255, 255, 0);
        game->addTextureToScreen(status);
        game->setStatus(status);
        game->setisNewGame(true);
        game->setGameState(true);
        addScreen(game);
        setCurrentScreen(game);
    }
};

#endif
