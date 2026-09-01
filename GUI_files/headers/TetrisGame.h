#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include "GameScreen.h"

#include <random>
#include <vector>

class TetrisGame : public GameScreen {
public:
    TetrisGame();
    Action update(SDL_Event* event);
    void render(SDL_Renderer* renderer);
    void newGame();
    void setStatus(ArcadeTexture* value) { status = value; }
    void setLinesHud(ArcadeTexture* value) { linesHud = value; }

private:
    static const int boardWidth = 10;
    static const int boardHeight = 20;
    struct Block { int x; int y; };
    struct Piece { int type; int rotation; int x; int y; };

    Block transformedBlock(int blockIndex, int rotation) const;
    Block transformedBlockForType(int type, int blockIndex, int rotation) const;
    bool canPlace(const Piece& candidate) const;
    bool tryMove(int dx, int dy);
    void rotatePiece(int direction);
    void lockPiece();
    int clearCompletedLines();
    void spawnPiece();
    int takeNextPiece();
    void updateStatus(const char* text);
    void updateLinesHud();
    SDL_Color colorFor(int value) const;

    int board[boardHeight][boardWidth];
    Piece activePiece;
    bool gameOver;
    Uint32 lastDropTime;
    Uint32 dropInterval;
    ArcadeTexture* status;
    ArcadeTexture* linesHud;
    int linesCleared;
    int score;
    int level;
    int nextPieceType;
    std::vector<int> pieceBag;
    std::mt19937 randomGenerator;
};

#endif
