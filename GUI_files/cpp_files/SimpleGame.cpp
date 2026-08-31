#include "SimpleGame.h"
#include "ScoreStore.h"
#include "SoundEffects.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

namespace {
const char* fontPath = "fonts/pixel/classic.ttf";
const SDL_Color hudColor = {255, 255, 255, 255};
const SDL_Color messageColor = {255, 255, 0, 255};
const int brickColumns = 10;
const int brickStartX = 45;
const int brickStartY = 235;
const int brickStepX = 55;
const int brickStepY = 25;
const int speedThreshold = 1000;
const int messageY = 180;
const int normalPaddleWidth = 78;
const int widePaddleWidth = 130;
const Uint32 effectDuration = 10000;
}

SimpleGame::SimpleGame()
    : ball(nullptr), paddle(nullptr), hud(nullptr), message(nullptr), score(0),
      lives(3), bricksDestroyed(0), nextPowerUp(0), roundActive(false),
      wideUntil(0), slowUntil(0), pierceUntil(0), pauseTexture(nullptr),
      paused(false), pauseStarted(0), resumeAt(0), countdownValue(0)
{
    for (int index = 0; index < 5; ++index) powerUpTextures[index] = nullptr;
}

SimpleGame::~SimpleGame()
{
    for (int index = 0; index < 5; ++index) SDL_DestroyTexture(powerUpTextures[index]);
    SDL_DestroyTexture(pauseTexture);
}

Action SimpleGame::update(SDL_Event* event)
{
    Action action = {DO_NOTHING, nullptr};
    if (getIsNewGame()) newGame();

    if (event && event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_q || event->key.keysym.sym == SDLK_ESCAPE) {
            setisNewGame(true);
            return {MOVE_NODES, getParentNode()};
        }
        if (event->key.keysym.sym == SDLK_n) newGame();
        if (event->key.keysym.sym == SDLK_SPACE && !roundActive && lives > 0) resetBall();
        if (event->key.keysym.sym == SDLK_p && event->key.repeat == 0 && roundActive) {
            if (paused) {
                paused = false;
                resumeAt = SDL_GetTicks() + 3000;
                countdownValue = 0;
            } else {
                paused = true;
                resumeAt = 0;
                if (!pauseStarted) pauseStarted = SDL_GetTicks();
                showMessage("PAUSED\nPRESS P TO RESUME", 24);
            }
        }
    }

    if (!event) {
        const Uint32 now = SDL_GetTicks();
        if (paused) return action;
        if (resumeAt) {
            if (now < resumeAt) {
                const int remaining = static_cast<int>((resumeAt - now + 999) / 1000);
                if (remaining != countdownValue) {
                    countdownValue = remaining;
                    showMessage(std::to_string(remaining), 48);
                }
                return action;
            }
            const Uint32 pausedFor = now - pauseStarted;
            if (wideUntil) wideUntil += pausedFor;
            if (slowUntil) slowUntil += pausedFor;
            if (pierceUntil) pierceUntil += pausedFor;
            resumeAt = pauseStarted = 0;
            countdownValue = 0;
            showMessage("GO!", 28);
        }
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        int paddleX = paddle->getXPos();
        const int paddleSpeed = std::max(7, speedLevel() + 6);
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) paddleX -= paddleSpeed;
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) paddleX += paddleSpeed;
        paddle->setXPos(std::max(0, std::min(windowWidth - paddle->getWidth(), paddleX)));

        updatePowerUps();
        if (roundActive) {
            ball->update(nullptr);
            for (std::vector<Ball*>::iterator extra = extraBalls.begin(); extra != extraBalls.end(); ++extra) {
                if ((*extra)->getState()) (*extra)->update(nullptr);
            }
            processBall(ball);
            for (std::vector<Ball*>::iterator extra = extraBalls.begin(); extra != extraBalls.end(); ++extra) {
                if ((*extra)->getState()) processBall(*extra);
            }

            if (roundActive && !hasActiveBalls()) {
                SoundEffects::playWall();
                --lives;
                roundActive = false;
                powerUps.clear();
                resetEffects();
                updateHud();
                if (lives > 0) {
                    showMessage("BALL LOST - PRESS SPACE", 22);
                } else {
                    setGameState(false);
                    ScoreStore::submit("brick_breaker", score);
                    showMessage("GAME OVER\nSCORE SAVED - PRESS N", 20);
                }
            }
        } else if (ball->getState()) {
            ball->setXPos(paddle->getXPos() + paddle->getWidth() / 2 - ball->getWidth() / 2);
            ball->setYPos(paddle->getYPos() - ball->getHeight() - 2);
        }
    }
    return action;
}

void SimpleGame::render(SDL_Renderer* renderer)
{
    GameScreen::render(renderer);
    const char* paths[] = {
        "brickBreakerNodeImages/powerups/widePaddle.png",
        "brickBreakerNodeImages/powerups/multiball.png",
        "brickBreakerNodeImages/powerups/extraLife.png",
        "brickBreakerNodeImages/powerups/slowBall.png",
        "brickBreakerNodeImages/powerups/pierceBall.png"
    };
    for (int index = 0; index < 5; ++index) {
        if (!powerUpTextures[index]) powerUpTextures[index] = IMG_LoadTexture(renderer, paths[index]);
    }
    for (std::vector<PowerUp>::const_iterator power = powerUps.begin(); power != powerUps.end(); ++power) {
        if (!power->active) continue;
        SDL_Texture* texture = powerUpTextures[static_cast<int>(power->type)];
        if (texture) {
            SDL_RenderCopy(renderer, texture, nullptr, &power->bounds);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &power->bounds);
        }
    }
    if (paused || resumeAt) {
        if (!pauseTexture) pauseTexture = IMG_LoadTexture(renderer,
            "brickBreakerNodeImages/pauseScreenBackgroundTexture.png");
        SDL_Rect screen = {0, 0, windowWidth, windowHeight};
        if (pauseTexture) SDL_RenderCopy(renderer, pauseTexture, nullptr, &screen);
        message->render(renderer);
    }
}

void SimpleGame::newGame()
{
    score = 0;
    lives = 3;
    bricksDestroyed = 0;
    nextPowerUp = 0;
    paused = false;
    pauseStarted = resumeAt = 0;
    countdownValue = 0;
    powerUps.clear();
    resetEffects();
    setGameState(true);
    setisNewGame(false);
    paddle->setXPos(windowWidth / 2 - paddle->getWidth() / 2);
    paddle->setYPos(windowHeight - paddle->getHeight() - 12);
    paddle->setState(true);
    refillBoard();
    parkBall();
    showMessage("PRESS SPACE TO SERVE  |  Q TO EXIT", 18);
    updateHud();
}

void SimpleGame::resetBall()
{
    for (std::vector<Ball*>::iterator extra = extraBalls.begin(); extra != extraBalls.end(); ++extra)
        (*extra)->setState(false);
    ball->setXPos(paddle->getXPos() + paddle->getWidth() / 2 - ball->getWidth() / 2);
    ball->setYPos(paddle->getYPos() - ball->getHeight() - 2);
    const int level = speedLevel();
    ball->setXVelocity((lives % 2 == 0) ? -(level + 2) : level + 2);
    ball->setYVelocity(-verticalSpeed());
    ball->setState(true);
    roundActive = true;
}

void SimpleGame::parkBall()
{
    for (std::vector<Ball*>::iterator extra = extraBalls.begin(); extra != extraBalls.end(); ++extra)
        (*extra)->setState(false);
    ball->setXPos(paddle->getXPos() + paddle->getWidth() / 2 - ball->getWidth() / 2);
    ball->setYPos(paddle->getYPos() - ball->getHeight() - 2);
    ball->setXVelocity(0);
    ball->setYVelocity(0);
    ball->setState(true);
    roundActive = false;
}

int SimpleGame::speedLevel() const { return std::min(5, score / speedThreshold + 1); }

int SimpleGame::verticalSpeed() const
{
    const int normalSpeed = speedLevel() + 3;
    return (slowUntil && SDL_GetTicks() < slowUntil)
        ? std::max(2, normalSpeed / 2) : normalSpeed;
}

void SimpleGame::stabilizeBall(Ball* currentBall)
{
    if (!currentBall->getState()) return;
    int x = currentBall->getXVelocity();
    int y = currentBall->getYVelocity();
    const int xDirection = x < 0 ? -1 : 1;
    const int yDirection = y < 0 ? -1 : 1;
    x = xDirection * std::max(2, std::min(speedLevel() + 4, std::abs(x)));
    y = yDirection * verticalSpeed();
    currentBall->setXVelocity(x);
    currentBall->setYVelocity(y);
}

bool SimpleGame::hasActiveBalls() const
{
    if (ball->getState()) return true;
    for (std::vector<Ball*>::const_iterator extra = extraBalls.begin(); extra != extraBalls.end(); ++extra)
        if ((*extra)->getState()) return true;
    return false;
}

bool SimpleGame::intersects(const Entity* first, const Entity* second) const
{
    return first->getXPos() < second->getXPos() + second->getWidth()
        && first->getXPos() + first->getWidth() > second->getXPos()
        && first->getYPos() < second->getYPos() + second->getHeight()
        && first->getYPos() + first->getHeight() > second->getYPos();
}

bool SimpleGame::intersects(const SDL_Rect& rect, const Entity* entity) const
{
    return rect.x < entity->getXPos() + entity->getWidth()
        && rect.x + rect.w > entity->getXPos()
        && rect.y < entity->getYPos() + entity->getHeight()
        && rect.y + rect.h > entity->getYPos();
}

void SimpleGame::processBall(Ball* currentBall)
{
    if (currentBall->getXPos() <= 0) {
        currentBall->setXPos(0);
        currentBall->setXVelocity(std::abs(currentBall->getXVelocity()));
        stabilizeBall(currentBall);
        SoundEffects::playWall();
    } else if (currentBall->getXPos() + currentBall->getWidth() >= windowWidth) {
        currentBall->setXPos(windowWidth - currentBall->getWidth());
        currentBall->setXVelocity(-std::abs(currentBall->getXVelocity()));
        stabilizeBall(currentBall);
        SoundEffects::playWall();
    }
    if (currentBall->getYPos() <= 0) {
        currentBall->setYPos(0);
        currentBall->setYVelocity(std::abs(currentBall->getYVelocity()));
        stabilizeBall(currentBall);
        SoundEffects::playWall();
    }
    if (currentBall->getYPos() > windowHeight) {
        currentBall->setState(false);
        return;
    }

    if (currentBall->getYVelocity() > 0 && intersects(currentBall, paddle)) {
        currentBall->setYPos(paddle->getYPos() - currentBall->getHeight());
        const double offset = static_cast<double>(currentBall->getXPos()
            + currentBall->getWidth() / 2 - (paddle->getXPos() + paddle->getWidth() / 2));
        const double halfPaddle = std::max(1.0, paddle->getWidth() / 2.0);
        const int level = speedLevel();
        int horizontal = static_cast<int>(std::round((offset / halfPaddle) * (level + 4)));
        if (std::abs(horizontal) < 2)
            horizontal = currentBall->getXVelocity() < 0 ? -2 : 2;
        currentBall->setXVelocity(std::max(-(level + 4), std::min(level + 4, horizontal)));
        currentBall->setYVelocity(-verticalSpeed());
        SoundEffects::playPaddle();
    }

    for (std::vector<Tile*>::iterator brick = bricks.begin(); brick != bricks.end(); ++brick) {
        if (!(*brick)->getState() || !intersects(currentBall, *brick)) continue;
        const int clearedY = (*brick)->getYPos();
        const int dropX = (*brick)->getXPos() + (*brick)->getWidth() / 2 - 10;
        const int dropY = (*brick)->getYPos();
        (*brick)->setState(false);
        score += 100;
        ++bricksDestroyed;
        SoundEffects::playBrick();
        if (bricksDestroyed % 5 == 0) spawnPowerUp(dropX, dropY);
        if (score % speedThreshold == 0) applySpeedForScore();

        if (SDL_GetTicks() >= pierceUntil) {
            const int horizontalOverlap = std::min(
                currentBall->getXPos() + currentBall->getWidth() - (*brick)->getXPos(),
                (*brick)->getXPos() + (*brick)->getWidth() - currentBall->getXPos());
            const int verticalOverlap = std::min(
                currentBall->getYPos() + currentBall->getHeight() - (*brick)->getYPos(),
                (*brick)->getYPos() + (*brick)->getHeight() - currentBall->getYPos());
            if (horizontalOverlap < verticalOverlap)
            {
                currentBall->setXVelocity(-currentBall->getXVelocity());
                if (currentBall->getXVelocity() > 0)
                    currentBall->setXPos((*brick)->getXPos() + (*brick)->getWidth());
                else currentBall->setXPos((*brick)->getXPos() - currentBall->getWidth());
            }
            else {
                currentBall->setYVelocity(-currentBall->getYVelocity());
                if (currentBall->getYVelocity() > 0)
                    currentBall->setYPos((*brick)->getYPos() + (*brick)->getHeight());
                else currentBall->setYPos((*brick)->getYPos() - currentBall->getHeight());
            }
            stabilizeBall(currentBall);
        }
        updateHud();
        advanceRowsIfNeeded(clearedY);
        break;
    }
}

void SimpleGame::spawnPowerUp(int x, int y)
{
    PowerUp power = {static_cast<PowerUpType>(nextPowerUp), {x, y, 24, 24}, true};
    powerUps.push_back(power);
    nextPowerUp = (nextPowerUp + 1) % 5;
}

void SimpleGame::updatePowerUps()
{
    const Uint32 now = SDL_GetTicks();
    if (wideUntil && now >= wideUntil) { setPaddleWidth(normalPaddleWidth); wideUntil = 0; }
    if (slowUntil && now >= slowUntil) { slowUntil = 0; applySpeedForScore(); }
    if (pierceUntil && now >= pierceUntil) pierceUntil = 0;

    for (std::vector<PowerUp>::iterator power = powerUps.begin(); power != powerUps.end(); ++power) {
        if (!power->active) continue;
        power->bounds.y += 3;
        if (intersects(power->bounds, paddle)) {
            power->active = false;
            activatePowerUp(power->type);
        } else if (power->bounds.y > windowHeight) power->active = false;
    }
}

void SimpleGame::activatePowerUp(PowerUpType type)
{
    const Uint32 now = SDL_GetTicks();
    std::string label;
    switch (type) {
    case WIDE_PADDLE:
        setPaddleWidth(widePaddleWidth); wideUntil = now + effectDuration; label = "WIDE PADDLE"; break;
    case MULTIBALL:
        for (std::size_t index = 0; index < extraBalls.size(); ++index) {
            Ball* extra = extraBalls[index];
            extra->setXPos(ball->getXPos()); extra->setYPos(ball->getYPos());
            extra->setXVelocity(index == 0 ? -std::abs(ball->getXVelocity()) : std::abs(ball->getXVelocity()));
            extra->setYVelocity(ball->getYVelocity()); extra->setState(true);
        }
        label = "MULTIBALL"; break;
    case EXTRA_LIFE: ++lives; label = "EXTRA LIFE"; break;
    case SLOW_BALL:
        slowUntil = now + effectDuration;
        ball->setXVelocity(ball->getXVelocity() / 2); ball->setYVelocity(ball->getYVelocity() / 2);
        for (std::vector<Ball*>::iterator extra = extraBalls.begin(); extra != extraBalls.end(); ++extra) {
            (*extra)->setXVelocity((*extra)->getXVelocity() / 2);
            (*extra)->setYVelocity((*extra)->getYVelocity() / 2);
        }
        label = "SLOW MOTION"; break;
    case PIERCE_BALL: pierceUntil = now + effectDuration; label = "PIERCING BALL"; break;
    }
    SoundEffects::playClick();
    showMessage(label, 20);
    updateHud();
}

void SimpleGame::setPaddleWidth(int width)
{
    const int center = paddle->getXPos() + paddle->getWidth() / 2;
    paddle->setWidth(width);
    paddle->getGameObjectTexture()->setSize(width, paddle->getHeight());
    paddle->setXPos(std::max(0, std::min(windowWidth - width, center - width / 2)));
}

void SimpleGame::resetEffects()
{
    wideUntil = slowUntil = pierceUntil = 0;
    if (paddle) setPaddleWidth(normalPaddleWidth);
}

void SimpleGame::applySpeedForScore()
{
    std::vector<Ball*> allBalls = extraBalls;
    allBalls.push_back(ball);
    for (std::vector<Ball*>::iterator current = allBalls.begin(); current != allBalls.end(); ++current) {
        if (!(*current)->getState()) continue;
        stabilizeBall(*current);
    }
}

void SimpleGame::refillBoard()
{
    for (std::size_t index = 0; index < bricks.size(); ++index) {
        bricks[index]->setXPos(brickStartX + static_cast<int>(index % brickColumns) * brickStepX);
        bricks[index]->setYPos(brickStartY + static_cast<int>(index / brickColumns) * brickStepY);
        bricks[index]->setState(true);
    }
}

void SimpleGame::advanceRowsIfNeeded(int clearedY)
{
    int lowestY = -1;
    for (std::vector<Tile*>::const_iterator brick = bricks.begin(); brick != bricks.end(); ++brick)
        if ((*brick)->getState()) lowestY = std::max(lowestY, (*brick)->getYPos());
    if (lowestY >= clearedY) return;
    if (lowestY == -1) {
        refillBoard(); powerUps.clear(); resetEffects(); parkBall();
        showMessage("NEW BOARD - PRESS SPACE TO SERVE", 18);
        return;
    }
    for (std::vector<Tile*>::iterator brick = bricks.begin(); brick != bricks.end(); ++brick)
        if ((*brick)->getState()) (*brick)->setYPos((*brick)->getYPos() + brickStepY);
    int column = 0;
    for (std::vector<Tile*>::iterator brick = bricks.begin(); brick != bricks.end() && column < brickColumns; ++brick) {
        if ((*brick)->getState()) continue;
        (*brick)->setXPos(brickStartX + column * brickStepX);
        (*brick)->setYPos(brickStartY); (*brick)->setState(true); ++column;
    }
    showMessage("NEW ROW!  KEEP GOING", 18);
}

void SimpleGame::updateHud()
{
    const std::string text = "SCORE " + std::to_string(score) + "   LIVES "
        + std::to_string(lives) + "   SPEED " + std::to_string(speedLevel());
    hud->loadFromRenderedText(fontPath, 20, text, hudColor);
    hud->setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL); hud->setPosition(12, 12);
}

void SimpleGame::showMessage(const std::string& text, int preferredSize)
{
    message->loadFromRenderedTextCenteredWrapped(fontPath, preferredSize, text,
        messageColor, windowWidth - 48);
    message->setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL);
    message->setPosition(CENTER_X, messageY);
}
