#ifndef FROGGER_GAME_H
#define FROGGER_GAME_H

#include "GameScreen.h"

#include <string>
#include <vector>

class FroggerGame : public GameScreen {
public:
    FroggerGame();
    ~FroggerGame();
    Action update(SDL_Event* event);
    void render(SDL_Renderer* renderer);
    void newGame();
    void setHud(ArcadeTexture* value) { hud = value; }
    void setMessage(ArcadeTexture* value) { message = value; }

private:
    struct Mover {
        float x;
        int y;
        int width;
        int height;
        float speed;
        int color;
    };

    void buildLanes();
    void resetFrog(const std::string& text);
    void loseLife(const std::string& reason);
    void reachHome();
    void updateHud();
    void updateMessage(const std::string& text);
    void moveFrog(int dx, int dy);
    void updateWorld(float seconds);
    bool overlaps(const SDL_Rect& first, const Mover& second) const;
    bool isRiverRow() const;
    void ensureSprites(SDL_Renderer* renderer);

    std::vector<Mover> vehicles;
    std::vector<Mover> platforms;
    bool homes[5];
    int frogX;
    int frogY;
    int score;
    int lives;
    int level;
    int bestY;
    bool gameOver;
    Uint32 lastUpdate;
    Uint32 roundStarted;
    ArcadeTexture* hud;
    ArcadeTexture* message;
    SDL_Texture* frogTexture;
    SDL_Texture* carTexture;
    SDL_Texture* truckTexture;
    SDL_Texture* logTexture;
};

#endif
