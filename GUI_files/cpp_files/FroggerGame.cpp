#include "FroggerGame.h"

#include "Config.h"
#include "ScoreStore.h"
#include "SoundEffects.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace {
const int frogSize = 24;
const int hopX = 32;
const int hopY = 40;
const int startX = 308;
const int startY = 444;
const int roadTop = 270;
const int riverTop = 70;
const int riverBottom = 230;
const int homeY = 44;
const int homeCenters[5] = {64, 192, 320, 448, 576};
const int roundSeconds = 45;
}

FroggerGame::FroggerGame()
    : frogX(startX), frogY(startY), score(0), lives(3), level(1),
      bestY(startY), gameOver(false), lastUpdate(0), roundStarted(0),
      hud(nullptr), message(nullptr), frogTexture(nullptr), carTexture(nullptr),
      truckTexture(nullptr), logTexture(nullptr)
{
    for (int home = 0; home < 5; ++home) homes[home] = false;
}

FroggerGame::~FroggerGame()
{
    SDL_DestroyTexture(frogTexture);
    SDL_DestroyTexture(carTexture);
    SDL_DestroyTexture(truckTexture);
    SDL_DestroyTexture(logTexture);
}

void FroggerGame::newGame()
{
    score = 0;
    lives = 3;
    level = 1;
    gameOver = false;
    for (int home = 0; home < 5; ++home) homes[home] = false;
    buildLanes();
    setGameState(true);
    setisNewGame(false);
    lastUpdate = SDL_GetTicks();
    resetFrog("ARROWS/WASD MOVE   Q EXITS");
}

Action FroggerGame::update(SDL_Event* event)
{
    if (getIsNewGame()) newGame();
    if (event && event->type == SDL_KEYDOWN) {
        const SDL_Keycode key = event->key.keysym.sym;
        if (key == SDLK_q || key == SDLK_ESCAPE) {
            setisNewGame(true);
            return {MOVE_NODES, getParentNode()};
        }
        if (key == SDLK_n && gameOver) newGame();
        if (!gameOver && event->key.repeat == 0) {
            if (key == SDLK_LEFT || key == SDLK_a) moveFrog(-hopX, 0);
            else if (key == SDLK_RIGHT || key == SDLK_d) moveFrog(hopX, 0);
            else if (key == SDLK_UP || key == SDLK_w) moveFrog(0, -hopY);
            else if (key == SDLK_DOWN || key == SDLK_s) moveFrog(0, hopY);
        }
    }

    if (!event && !gameOver) {
        const Uint32 now = SDL_GetTicks();
        const float seconds = std::min(0.05f, (now - lastUpdate) / 1000.0f);
        lastUpdate = now;
        updateWorld(seconds);
        if (!gameOver && now - roundStarted >= roundSeconds * 1000u)
            loseLife("TIME UP");
        updateHud();
    }
    return {DO_NOTHING, nullptr};
}

void FroggerGame::buildLanes()
{
    vehicles.clear();
    platforms.clear();
    const float multiplier = 1.0f + (level - 1) * 0.14f;

    for (int lane = 0; lane < 4; ++lane) {
        const int y = roadTop + lane * 40 + 7;
        const int width = lane % 2 == 0 ? 48 : 62;
        const float speed = (lane % 2 == 0 ? 80.0f : -105.0f)
            * multiplier * (1.0f + lane * 0.08f);
        for (int item = 0; item < 4; ++item)
            vehicles.push_back({static_cast<float>(item * 185 + lane * 45),
                y, width, 26, speed, lane});
    }

    for (int lane = 0; lane < 4; ++lane) {
        const int y = riverTop + lane * 40 + 5;
        const int width = lane % 2 == 0 ? 112 : 82;
        const float speed = (lane % 2 == 0 ? 62.0f : -76.0f)
            * multiplier;
        for (int item = 0; item < 4; ++item)
            platforms.push_back({static_cast<float>(item * 205 + lane * 65),
                y, width, 30, speed, lane});
    }
}

void FroggerGame::moveFrog(int dx, int dy)
{
    frogX = std::max(0, std::min(windowWidth - frogSize, frogX + dx));
    frogY = std::max(homeY, std::min(startY, frogY + dy));
    if (frogY < bestY) {
        score += 10;
        bestY = frogY;
    }
    SoundEffects::playClick();
    if (frogY <= homeY + 5) reachHome();
    updateHud();
}

void FroggerGame::updateWorld(float seconds)
{
    for (std::vector<Mover>::iterator item = vehicles.begin(); item != vehicles.end(); ++item) {
        item->x += item->speed * seconds;
        if (item->speed > 0 && item->x > windowWidth) item->x = -item->width;
        if (item->speed < 0 && item->x + item->width < 0) item->x = windowWidth;
    }
    for (std::vector<Mover>::iterator item = platforms.begin(); item != platforms.end(); ++item) {
        item->x += item->speed * seconds;
        if (item->speed > 0 && item->x > windowWidth) item->x = -item->width;
        if (item->speed < 0 && item->x + item->width < 0) item->x = windowWidth;
    }

    SDL_Rect frog = {frogX, frogY, frogSize, frogSize};
    if (frogY >= roadTop && frogY < roadTop + 160) {
        for (std::vector<Mover>::const_iterator car = vehicles.begin(); car != vehicles.end(); ++car) {
            if (overlaps(frog, *car)) {
                loseLife("SPLAT!");
                return;
            }
        }
    }

    if (isRiverRow()) {
        const Mover* support = nullptr;
        for (std::vector<Mover>::const_iterator platform = platforms.begin();
             platform != platforms.end(); ++platform) {
            if (overlaps(frog, *platform)) {
                support = &*platform;
                break;
            }
        }
        if (!support) {
            loseLife("SPLASH!");
            return;
        }
        frogX += static_cast<int>(std::round(support->speed * seconds));
        if (frogX < 0 || frogX + frogSize > windowWidth) loseLife("SWEPT AWAY!");
    }
}

bool FroggerGame::overlaps(const SDL_Rect& first, const Mover& second) const
{
    return first.x < second.x + second.width
        && first.x + first.w > second.x
        && first.y < second.y + second.height
        && first.y + first.h > second.y;
}

bool FroggerGame::isRiverRow() const
{
    return frogY >= riverTop && frogY < riverBottom;
}

void FroggerGame::reachHome()
{
    const int center = frogX + frogSize / 2;
    int closest = 0;
    for (int home = 1; home < 5; ++home)
        if (std::abs(center - homeCenters[home]) < std::abs(center - homeCenters[closest]))
            closest = home;
    if (std::abs(center - homeCenters[closest]) > 34 || homes[closest]) {
        loseLife("MISSED HOME!");
        return;
    }

    homes[closest] = true;
    const int secondsLeft = std::max(0, roundSeconds
        - static_cast<int>((SDL_GetTicks() - roundStarted) / 1000));
    score += 500 + secondsLeft * 10;
    SoundEffects::playBrick();
    bool complete = true;
    for (int home = 0; home < 5; ++home) complete = complete && homes[home];
    if (complete) {
        score += 1000;
        ++level;
        for (int home = 0; home < 5; ++home) homes[home] = false;
        buildLanes();
        resetFrog("ROUND CLEAR!");
    } else resetFrog("HOME SAFE!");
}

void FroggerGame::loseLife(const std::string& reason)
{
    --lives;
    SoundEffects::playWall();
    if (lives <= 0) {
        gameOver = true;
        setGameState(false);
        ScoreStore::submit("frogger", score);
        updateMessage("GAME OVER - SCORE SAVED - PRESS N");
        updateHud();
        return;
    }
    resetFrog(reason);
}

void FroggerGame::resetFrog(const std::string& text)
{
    frogX = startX;
    frogY = startY;
    bestY = startY;
    roundStarted = SDL_GetTicks();
    lastUpdate = roundStarted;
    updateMessage(text);
    updateHud();
}

void FroggerGame::updateHud()
{
    if (!hud) return;
    const int timeLeft = gameOver ? 0 : std::max(0, roundSeconds
        - static_cast<int>((SDL_GetTicks() - roundStarted) / 1000));
    const std::string text = "SCORE " + std::to_string(score)
        + "   LIVES " + std::to_string(lives)
        + "   LEVEL " + std::to_string(level)
        + "   TIME " + std::to_string(timeLeft);
    hud->loadFromRenderedText("fonts/pixel/classic.ttf", 17, text,
        {255, 255, 255, 255});
    hud->setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL);
    hud->setPosition(CENTER_X, 8);
}

void FroggerGame::updateMessage(const std::string& text)
{
    if (!message) return;
    message->loadFromRenderedTextCenteredWrapped("fonts/pixel/classic.ttf", 14,
        text, {255, 255, 0, 255}, windowWidth - 30);
    message->setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL);
    message->setPosition(CENTER_X, 244);
}

void FroggerGame::ensureSprites(SDL_Renderer* renderer)
{
    if (!frogTexture)
        frogTexture = IMG_LoadTexture(renderer, "froggerNodeImages/frog.png");
    if (!carTexture)
        carTexture = IMG_LoadTexture(renderer, "froggerNodeImages/car.png");
    if (!truckTexture)
        truckTexture = IMG_LoadTexture(renderer, "froggerNodeImages/truck.png");
    if (!logTexture)
        logTexture = IMG_LoadTexture(renderer, "froggerNodeImages/log.png");
}

void FroggerGame::render(SDL_Renderer* renderer)
{
    ensureSprites(renderer);
    SDL_SetRenderDrawColor(renderer, 7, 5, 22, 255);
    SDL_RenderClear(renderer);

    SDL_Rect homesArea = {0, 34, windowWidth, 36};
    SDL_SetRenderDrawColor(renderer, 14, 92, 40, 255);
    SDL_RenderFillRect(renderer, &homesArea);
    SDL_Rect river = {0, riverTop, windowWidth, riverBottom - riverTop};
    SDL_SetRenderDrawColor(renderer, 5, 36, 104, 255);
    SDL_RenderFillRect(renderer, &river);
    for (int y = riverTop + 12; y < riverBottom; y += 20) {
        SDL_SetRenderDrawColor(renderer, 10, 65, 145, 255);
        SDL_RenderDrawLine(renderer, 0, y, windowWidth, y);
    }
    SDL_Rect median = {0, riverBottom, windowWidth, roadTop - riverBottom};
    SDL_SetRenderDrawColor(renderer, 20, 118, 46, 255);
    SDL_RenderFillRect(renderer, &median);
    SDL_Rect road = {0, roadTop, windowWidth, 160};
    SDL_SetRenderDrawColor(renderer, 24, 24, 34, 255);
    SDL_RenderFillRect(renderer, &road);
    for (int y = roadTop + 40; y < roadTop + 160; y += 40) {
        SDL_SetRenderDrawColor(renderer, 235, 200, 45, 255);
        for (int x = 0; x < windowWidth; x += 35)
            SDL_RenderDrawLine(renderer, x, y, x + 18, y);
    }
    SDL_Rect start = {0, 430, windowWidth, 50};
    SDL_SetRenderDrawColor(renderer, 18, 90, 38, 255);
    SDL_RenderFillRect(renderer, &start);

    for (int home = 0; home < 5; ++home) {
        SDL_Rect bay = {homeCenters[home] - 28, homeY - 5, 56, 30};
        SDL_SetRenderDrawColor(renderer, homes[home] ? 24 : 5,
            homes[home] ? 105 : 22, homes[home] ? 34 : 12, 255);
        SDL_RenderFillRect(renderer, &bay);
        SDL_SetRenderDrawColor(renderer, 165, 255, 90, 255);
        SDL_RenderDrawRect(renderer, &bay);
        if (homes[home] && frogTexture) {
            SDL_Rect homeFrog = {homeCenters[home] - frogSize / 2,
                homeY - 2, frogSize, frogSize};
            SDL_RenderCopy(renderer, frogTexture, nullptr, &homeFrog);
        }
    }

    for (std::vector<Mover>::const_iterator log = platforms.begin(); log != platforms.end(); ++log) {
        SDL_Rect body = {static_cast<int>(log->x), log->y, log->width, log->height};
        if (logTexture) SDL_RenderCopy(renderer, logTexture, nullptr, &body);
        else {
            SDL_SetRenderDrawColor(renderer, 133, 72, 35, 255);
            SDL_RenderFillRect(renderer, &body);
        }
    }
    for (std::vector<Mover>::const_iterator car = vehicles.begin(); car != vehicles.end(); ++car) {
        SDL_Rect body = {static_cast<int>(car->x), car->y, car->width, car->height};
        SDL_Texture* vehicleTexture = car->width > 50 ? truckTexture : carTexture;
        if (vehicleTexture) {
            const SDL_RendererFlip flip = car->speed > 0
                ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderCopyEx(renderer, vehicleTexture, nullptr, &body,
                0.0, nullptr, flip);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 45, 80, 255);
            SDL_RenderFillRect(renderer, &body);
        }
    }

    if (!gameOver) {
        SDL_Rect frog = {frogX, frogY, frogSize, frogSize};
        if (frogTexture) SDL_RenderCopy(renderer, frogTexture, nullptr, &frog);
        else {
            SDL_SetRenderDrawColor(renderer, 65, 245, 70, 255);
            SDL_RenderFillRect(renderer, &frog);
        }
    }

    GameScreen::render(renderer);
}
