#include "TetrisGame.h"
#include "SoundEffects.h"

#include <algorithm>

namespace {
const int cellSize = 22;
const int boardX = 210;
const int boardY = 20;

const int baseShapes[7][4][2] = {
    {{0,1}, {1,1}, {2,1}, {3,1}}, // I
    {{1,0}, {2,0}, {1,1}, {2,1}}, // O
    {{1,0}, {0,1}, {1,1}, {2,1}}, // T
    {{1,0}, {2,0}, {0,1}, {1,1}}, // S
    {{0,0}, {1,0}, {1,1}, {2,1}}, // Z
    {{0,0}, {0,1}, {1,1}, {2,1}}, // J
    {{2,0}, {0,1}, {1,1}, {2,1}}  // L
};
}

TetrisGame::TetrisGame()
    : gameOver(false), lastDropTime(0), dropInterval(650), status(nullptr),
      randomGenerator(std::random_device()())
{
    activePiece = {0, 0, 3, 0};
    for (int row = 0; row < boardHeight; ++row)
        for (int column = 0; column < boardWidth; ++column) board[row][column] = 0;
}

void TetrisGame::newGame()
{
    for (int row = 0; row < boardHeight; ++row)
        for (int column = 0; column < boardWidth; ++column) board[row][column] = 0;
    gameOver = false;
    setGameState(true);
    setisNewGame(false);
    lastDropTime = SDL_GetTicks();
    updateStatus("ARROWS MOVE  UP/Z ROTATE  SPACE DROP");
    spawnPiece();
}

Action TetrisGame::update(SDL_Event* event)
{
    if (getIsNewGame()) newGame();
    Action action = {DO_NOTHING, nullptr};

    if (event && event->type == SDL_KEYDOWN) {
        const SDL_Keycode key = event->key.keysym.sym;
        if (key == SDLK_q || key == SDLK_ESCAPE) {
            setisNewGame(true);
            return {MOVE_NODES, getParentNode()};
        }
        if (key == SDLK_n && gameOver) newGame();
        if (!gameOver) {
            if (key == SDLK_LEFT) tryMove(-1, 0);
            else if (key == SDLK_RIGHT) tryMove(1, 0);
            else if (key == SDLK_DOWN) {
                if (!tryMove(0, 1)) lockPiece();
                lastDropTime = SDL_GetTicks();
            } else if (key == SDLK_UP && event->key.repeat == 0) rotatePiece(1);
            else if (key == SDLK_z && event->key.repeat == 0) rotatePiece(-1);
            else if (key == SDLK_SPACE && event->key.repeat == 0) {
                while (tryMove(0, 1)) {}
                lockPiece();
            }
        }
    }

    if (!event && !gameOver) {
        const Uint32 now = SDL_GetTicks();
        if (now - lastDropTime >= dropInterval) {
            if (!tryMove(0, 1)) lockPiece();
            lastDropTime = now;
        }
    }
    return action;
}

TetrisGame::Block TetrisGame::transformedBlock(int blockIndex, int rotation) const
{
    int x = baseShapes[activePiece.type][blockIndex][0];
    int y = baseShapes[activePiece.type][blockIndex][1];
    if (activePiece.type == 1) return {x, y};
    for (int turn = 0; turn < rotation; ++turn) {
        const int oldX = x;
        x = 3 - y;
        y = oldX;
    }
    return {x, y};
}

bool TetrisGame::canPlace(const Piece& candidate) const
{
    for (int block = 0; block < 4; ++block) {
        int x = baseShapes[candidate.type][block][0];
        int y = baseShapes[candidate.type][block][1];
        if (candidate.type != 1) {
            for (int turn = 0; turn < candidate.rotation; ++turn) {
                const int oldX = x;
                x = 3 - y;
                y = oldX;
            }
        }
        x += candidate.x;
        y += candidate.y;
        if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight) return false;
        if (board[y][x] != 0) return false;
    }
    return true;
}

bool TetrisGame::tryMove(int dx, int dy)
{
    Piece candidate = activePiece;
    candidate.x += dx;
    candidate.y += dy;
    if (!canPlace(candidate)) return false;
    activePiece = candidate;
    return true;
}

void TetrisGame::rotatePiece(int direction)
{
    Piece candidate = activePiece;
    candidate.rotation = (candidate.rotation + direction + 4) % 4;
    const int kicks[] = {0, -1, 1, -2, 2};
    for (int index = 0; index < 5; ++index) {
        candidate.x = activePiece.x + kicks[index];
        if (canPlace(candidate)) {
            activePiece = candidate;
            SoundEffects::playClick();
            return;
        }
    }
}

void TetrisGame::lockPiece()
{
    for (int block = 0; block < 4; ++block) {
        const Block transformed = transformedBlock(block, activePiece.rotation);
        board[activePiece.y + transformed.y][activePiece.x + transformed.x] = activePiece.type + 1;
    }
    SoundEffects::playPaddle();
    spawnPiece();
}

void TetrisGame::spawnPiece()
{
    std::uniform_int_distribution<int> distribution(0, 6);
    activePiece = {distribution(randomGenerator), 0, 3, 0};
    if (!canPlace(activePiece)) {
        gameOver = true;
        setGameState(false);
        updateStatus("GAME OVER  -  PRESS N TO RESTART");
    }
}

void TetrisGame::updateStatus(const char* text)
{
    if (!status) return;
    status->loadFromRenderedTextCenteredWrapped("fonts/pixel/classic.ttf", 15,
        text, {255, 255, 0, 255}, 175);
    status->setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL);
    status->setPosition(12, 260);
}

SDL_Color TetrisGame::colorFor(int value) const
{
    const SDL_Color colors[] = {
        {0,0,0,255}, {0,240,240,255}, {240,240,0,255}, {170,0,240,255},
        {0,220,80,255}, {240,40,40,255}, {40,80,240,255}, {240,140,0,255}
    };
    return colors[std::max(0, std::min(7, value))];
}

void TetrisGame::render(SDL_Renderer* renderer)
{
    GameScreen::render(renderer);
    SDL_Rect boardRect = {boardX - 3, boardY - 3,
        boardWidth * cellSize + 6, boardHeight * cellSize + 6};
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderFillRect(renderer, &boardRect);
    SDL_Rect playArea = {boardX, boardY, boardWidth * cellSize, boardHeight * cellSize};
    SDL_SetRenderDrawColor(renderer, 8, 8, 16, 255);
    SDL_RenderFillRect(renderer, &playArea);

    for (int row = 0; row < boardHeight; ++row) {
        for (int column = 0; column < boardWidth; ++column) {
            if (!board[row][column]) continue;
            const SDL_Color color = colorFor(board[row][column]);
            SDL_Rect cell = {boardX + column * cellSize + 1,
                boardY + row * cellSize + 1, cellSize - 2, cellSize - 2};
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderFillRect(renderer, &cell);
        }
    }
    if (!gameOver) {
        const SDL_Color color = colorFor(activePiece.type + 1);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        for (int block = 0; block < 4; ++block) {
            const Block transformed = transformedBlock(block, activePiece.rotation);
            SDL_Rect cell = {boardX + (activePiece.x + transformed.x) * cellSize + 1,
                boardY + (activePiece.y + transformed.y) * cellSize + 1,
                cellSize - 2, cellSize - 2};
            SDL_RenderFillRect(renderer, &cell);
        }
    }
}
