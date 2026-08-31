#ifndef GAME_NODE_H
#define GAME_NODE_H

#include "Node.h"
#include "Config.h"
#include "SimpleGame.h"
#include "Tile.h"

class GameNode : public Node {

public:
    GameNode(SDL_Renderer* renderer_in, Node* parentNode_in) : Node (renderer_in, parentNode_in) {
        SimpleGame* game = new SimpleGame;
        game->addTextureToScreen(createImage(renderer_in, "brickBreakerNodeImages/brickBreakerScreen.png", true));

        Ball* ball = new Ball;
        ArcadeTexture* ballTexture = createImage(renderer_in, "brickBreakerNodeImages/ballTexture.png");

        ball->setGameObjectTexture(ballTexture);
        ball->setState(true);

        game->addEntity(ball);
        game->addMovingEntity(ball);
        game->setBall(ball);
        game->setParentNode(parentNode_in);

        for (int index = 0; index < 2; ++index) {
            Ball* extraBall = new Ball;
            extraBall->setGameObjectTexture(createImage(renderer_in,
                "brickBreakerNodeImages/ballTexture.png"));
            extraBall->setState(false);
            game->addEntity(extraBall);
            game->addExtraBall(extraBall);
        }

        Tile* paddle = new Tile;
        paddle->setGameObjectTexture(createImage(renderer_in, "brickBreakerNodeImages/paddleTexture.png"));
        paddle->setState(true);
        game->addEntity(paddle);
        game->setPaddle(paddle);

        const char* brickTextures[] = {
            "brickBreakerNodeImages/redBrickTexture.png",
            "brickBreakerNodeImages/yellowBrickTexture.png",
            "brickBreakerNodeImages/greenBrickTexture.png",
            "brickBreakerNodeImages/blueBrickTexture.png"
        };
        for (int row = 0; row < 4; ++row) {
            for (int column = 0; column < 10; ++column) {
                Tile* brick = new Tile;
                brick->setGameObjectTexture(createImage(renderer_in, brickTextures[row]));
                brick->setXPos(45 + column * 55);
                brick->setYPos(235 + row * 25);
                brick->setState(true);
                game->addEntity(brick);
                game->addBrick(brick);
            }
        }

        ArcadeTexture* hud = createSimpleText(renderer_in, "fonts/pixel/classic.ttf", 20, "SCORE 0     LIVES 3", 255, 255, 255);
        game->addTextureToScreen(hud);
        game->setHud(hud);

        ArcadeTexture* message = createSimpleText(renderer_in, "fonts/pixel/classic.ttf", 18, "ARROWS/A-D TO MOVE  |  Q TO EXIT", 255, 255, 0);
        game->addTextureToScreen(message);
        game->setMessage(message);

        game->setisNewGame(true);
        game->setGameState(true);

        this->addScreen(game);
        this->setCurrentScreen(game);
    }
};

#endif
