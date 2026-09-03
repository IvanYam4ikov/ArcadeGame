#ifndef SIMPLE_GAME_H
#define SIMPLE_GAME_H

#include "Ball.h"
#include "Config.h"
#include "GameScreen.h"
#include "Tile.h"

#include <random>
#include <string>
#include <vector>

class SimpleGame : public GameScreen {
public:
    SimpleGame();
    ~SimpleGame();
    Action update(SDL_Event* event);
    void render(SDL_Renderer* renderer);
    void newGame();

    void setBall(Ball* value) { ball = value; }
    Ball* getBall() { return ball; }
    void addExtraBall(Ball* value) { extraBalls.push_back(value); }
    void setPaddle(Tile* value) { paddle = value; }
    void addBrick(Tile* value) { bricks.push_back(value); }
    void setHud(ArcadeTexture* value) { hud = value; }
    void setMessage(ArcadeTexture* value) { message = value; }

private:
    enum PowerUpType { WIDE_PADDLE, MULTIBALL, EXTRA_LIFE, SLOW_BALL, PIERCE_BALL };
    struct PowerUp { PowerUpType type; SDL_Rect bounds; bool active; };

    bool intersects(const Entity* first, const Entity* second) const;
    bool intersects(const SDL_Rect& rect, const Entity* entity) const;
    void processBall(Ball* currentBall);
    void updatePowerUps();
    void spawnPowerUp(int x, int y);
    void activatePowerUp(PowerUpType type);
    void resetEffects();
    void setPaddleWidth(int width);
    void advanceRowsIfNeeded(int clearedRowY);
    void refillBoard();
    void parkBall();
    void resetBall();
    void applySpeedForScore();
    void stabilizeBall(Ball* currentBall);
    int verticalSpeed() const;
    int speedLevel() const;
    void updateHud();
    void showMessage(const std::string& text, int preferredSize);
    bool hasActiveBalls() const;

    Ball* ball;
    std::vector<Ball*> extraBalls;
    Tile* paddle;
    std::vector<Tile*> bricks;
    std::vector<PowerUp> powerUps;
    ArcadeTexture* hud;
    ArcadeTexture* message;
    int score;
    int lives;
    int bricksDestroyed;
    bool roundActive;
    Uint32 wideUntil;
    Uint32 slowUntil;
    Uint32 pierceUntil;
    SDL_Texture* powerUpTextures[5];
    SDL_Texture* pauseTexture;
    bool paused;
    Uint32 pauseStarted;
    Uint32 resumeAt;
    int countdownValue;
    std::mt19937 randomGenerator;
};

#endif
