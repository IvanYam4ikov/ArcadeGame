#include "TetrisGame.h"
#include "ScoreStore.h"
#include "SoundEffects.h"

#include <algorithm>

namespace {
const int cellSize = 22;
const int boardX = 210;
const int boardY = 20;
const int previewX = 500;
const int previewY = 145;
const int previewCellSize = 18;
const int previewBoxSize = 92;

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
      linesHud(nullptr), linesCleared(0), score(0), level(1), nextPieceType(0),
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
    linesCleared = 0;
    score = 0;
    level = 1;
    dropInterval = 650;
    pieceBag.clear();
    nextPieceType = takeNextPiece();
    setGameState(true);
    setisNewGame(false);
    lastDropTime = SDL_GetTicks();
    updateStatus("ARROWS MOVE  UP/Z ROTATE  SPACE DROP");
    updateLinesHud();
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
            if (key == SDLK_LEFT) {
                if (!tryMove(-1, 0)) SoundEffects::playWall();
            }
            else if (key == SDLK_RIGHT) {
                if (!tryMove(1, 0)) SoundEffects::playWall();
            }
            else if (key == SDLK_DOWN) {
                if (tryMove(0, 1)) {
                    ++score;
                    updateLinesHud();
                } else lockPiece();
                lastDropTime = SDL_GetTicks();
            } else if (key == SDLK_UP && event->key.repeat == 0) rotatePiece(1);
            else if (key == SDLK_z && event->key.repeat == 0) rotatePiece(-1);
            else if (key == SDLK_SPACE && event->key.repeat == 0) {
                int droppedRows = 0;
                while (tryMove(0, 1)) ++droppedRows;
                score += droppedRows * 2;
                updateLinesHud();
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
    return transformedBlockForType(activePiece.type, blockIndex, rotation);
}

TetrisGame::Block TetrisGame::transformedBlockForType(
    int type, int blockIndex, int rotation) const
{
    int x = baseShapes[type][blockIndex][0];
    int y = baseShapes[type][blockIndex][1];
    if (type == 1) return {x, y};
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
        const Block transformed = transformedBlockForType(
            candidate.type, block, candidate.rotation);
        int x = transformed.x;
        int y = transformed.y;
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
    const int cleared = clearCompletedLines();
    if (cleared > 0) {
        const int linePoints[] = {0, 100, 300, 500, 800};
        score += linePoints[cleared] * level;
        linesCleared += cleared;
        level = linesCleared / 10 + 1;
        dropInterval = static_cast<Uint32>(std::max(100, 650 - (level - 1) * 50));
        updateLinesHud();
        for (int line = 0; line < cleared; ++line) SoundEffects::playBrick();
    }
    spawnPiece();
}

int TetrisGame::clearCompletedLines()
{
    int destinationRow = boardHeight - 1;
    int cleared = 0;

    for (int sourceRow = boardHeight - 1; sourceRow >= 0; --sourceRow) {
        bool complete = true;
        for (int column = 0; column < boardWidth; ++column) {
            if (board[sourceRow][column] == 0) {
                complete = false;
                break;
            }
        }

        if (complete) {
            ++cleared;
            continue;
        }

        if (destinationRow != sourceRow) {
            for (int column = 0; column < boardWidth; ++column)
                board[destinationRow][column] = board[sourceRow][column];
        }
        --destinationRow;
    }

    while (destinationRow >= 0) {
        for (int column = 0; column < boardWidth; ++column)
            board[destinationRow][column] = 0;
        --destinationRow;
    }
    return cleared;
}

void TetrisGame::spawnPiece()
{
    activePiece = {nextPieceType, 0, 3, 0};
    nextPieceType = takeNextPiece();
    if (!canPlace(activePiece)) {
        gameOver = true;
        setGameState(false);
        ScoreStore::submit("tetris", score);
        updateStatus("GAME OVER  SCORE SAVED  PRESS N");
    }
}

int TetrisGame::takeNextPiece()
{
    if (pieceBag.empty()) {
        for (int type = 0; type < 7; ++type) pieceBag.push_back(type);
        std::shuffle(pieceBag.begin(), pieceBag.end(), randomGenerator);
    }
    const int type = pieceBag.back();
    pieceBag.pop_back();
    return type;
}

void TetrisGame::updateStatus(const char* text)
{
    if (!status) return;
    status->loadFromRenderedTextCenteredWrapped("fonts/pixel/classic.ttf", 15,
        text, {255, 255, 0, 255}, 175);
    status->setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL);
    status->setPosition(12, 260);
}

void TetrisGame::updateLinesHud()
{
    if (!linesHud) return;
    const std::string text = "SCORE " + std::to_string(score)
        + "\nLINES " + std::to_string(linesCleared)
        + "\nLEVEL " + std::to_string(level);
    linesHud->loadFromRenderedTextCenteredWrapped("fonts/pixel/classic.ttf", 18,
        text, {255, 255, 255, 255}, 165);
    linesHud->setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL);
    linesHud->setPosition(22, 145);
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
    // Draw a cabinet-like backdrop before the text textures. The restrained
    // grid and scanlines keep the screen retro without competing with play.
    SDL_SetRenderDrawColor(renderer, 5, 4, 20, 255);
    SDL_Rect backdrop = {0, 0, WIDTH_ORIGINAL, HEIGHT_ORIGINAL};
    SDL_RenderFillRect(renderer, &backdrop);

    SDL_SetRenderDrawColor(renderer, 13, 11, 38, 255);
    for (int y = 0; y < HEIGHT_ORIGINAL; y += 8)
        SDL_RenderDrawLine(renderer, 0, y, WIDTH_ORIGINAL, y);
    SDL_SetRenderDrawColor(renderer, 18, 15, 48, 255);
    for (int x = 0; x < WIDTH_ORIGINAL; x += 32)
        SDL_RenderDrawLine(renderer, x, 0, x, HEIGHT_ORIGINAL);

    SDL_Rect leftPanel = {9, 14, 182, 452};
    SDL_Rect rightPanel = {449, 14, 182, 452};
    SDL_SetRenderDrawColor(renderer, 19, 13, 48, 255);
    SDL_RenderFillRect(renderer, &leftPanel);
    SDL_RenderFillRect(renderer, &rightPanel);
    SDL_SetRenderDrawColor(renderer, 170, 0, 240, 255);
    SDL_RenderDrawRect(renderer, &leftPanel);
    SDL_RenderDrawRect(renderer, &rightPanel);

    SDL_SetRenderDrawColor(renderer, 0, 220, 240, 255);
    SDL_RenderDrawLine(renderer, 10, 15, 190, 15);
    SDL_RenderDrawLine(renderer, 450, 465, 630, 465);

    GameScreen::render(renderer);
    SDL_Rect boardRect = {boardX - 3, boardY - 3,
        boardWidth * cellSize + 6, boardHeight * cellSize + 6};
    SDL_SetRenderDrawColor(renderer, 0, 220, 240, 255);
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
        Piece ghostPiece = activePiece;
        Piece lowerGhost = ghostPiece;
        ++lowerGhost.y;
        while (canPlace(lowerGhost)) {
            ghostPiece = lowerGhost;
            ++lowerGhost.y;
        }

        const SDL_Color color = colorFor(activePiece.type + 1);
        if (ghostPiece.y > activePiece.y) {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 150);
            for (int block = 0; block < 4; ++block) {
                const Block transformed = transformedBlockForType(
                    ghostPiece.type, block, ghostPiece.rotation);
                SDL_Rect ghostCell = {
                    boardX + (ghostPiece.x + transformed.x) * cellSize + 4,
                    boardY + (ghostPiece.y + transformed.y) * cellSize + 4,
                    cellSize - 8, cellSize - 8};
                SDL_RenderDrawRect(renderer, &ghostCell);
                SDL_Rect innerOutline = {ghostCell.x + 1, ghostCell.y + 1,
                    ghostCell.w - 2, ghostCell.h - 2};
                SDL_RenderDrawRect(renderer, &innerOutline);
            }
        }

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        for (int block = 0; block < 4; ++block) {
            const Block transformed = transformedBlock(block, activePiece.rotation);
            SDL_Rect cell = {boardX + (activePiece.x + transformed.x) * cellSize + 1,
                boardY + (activePiece.y + transformed.y) * cellSize + 1,
                cellSize - 2, cellSize - 2};
            SDL_RenderFillRect(renderer, &cell);
        }
    }

    SDL_Rect previewBox = {previewX - 10, previewY - 10,
        previewBoxSize, previewBoxSize};
    SDL_SetRenderDrawColor(renderer, 170, 0, 240, 255);
    SDL_RenderDrawRect(renderer, &previewBox);
    SDL_Rect previewBackground = {previewBox.x + 2, previewBox.y + 2,
        previewBox.w - 4, previewBox.h - 4};
    SDL_SetRenderDrawColor(renderer, 8, 8, 16, 255);
    SDL_RenderFillRect(renderer, &previewBackground);

    int minimumX = 4;
    int maximumX = 0;
    int minimumY = 4;
    int maximumY = 0;
    for (int block = 0; block < 4; ++block) {
        const Block transformed = transformedBlockForType(nextPieceType, block, 0);
        minimumX = std::min(minimumX, transformed.x);
        maximumX = std::max(maximumX, transformed.x);
        minimumY = std::min(minimumY, transformed.y);
        maximumY = std::max(maximumY, transformed.y);
    }
    const int shapeWidth = (maximumX - minimumX + 1) * previewCellSize;
    const int shapeHeight = (maximumY - minimumY + 1) * previewCellSize;
    const int shapeX = previewBox.x + (previewBox.w - shapeWidth) / 2;
    const int shapeY = previewBox.y + (previewBox.h - shapeHeight) / 2;

    const SDL_Color previewColor = colorFor(nextPieceType + 1);
    for (int block = 0; block < 4; ++block) {
        const Block transformed = transformedBlockForType(nextPieceType, block, 0);
        SDL_Rect cell = {shapeX + (transformed.x - minimumX) * previewCellSize + 1,
            shapeY + (transformed.y - minimumY) * previewCellSize + 1,
            previewCellSize - 2, previewCellSize - 2};
        SDL_SetRenderDrawColor(renderer, previewColor.r, previewColor.g,
            previewColor.b, 255);
        SDL_RenderFillRect(renderer, &cell);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 120);
        SDL_RenderDrawLine(renderer, cell.x, cell.y,
            cell.x + cell.w - 1, cell.y);
        SDL_RenderDrawLine(renderer, cell.x, cell.y,
            cell.x, cell.y + cell.h - 1);
    }
}
