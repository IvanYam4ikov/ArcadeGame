#ifndef HIGHSCORES_NODE_H
#define HIGHSCORES_NODE_H

#include "Config.h"
#include "MenuScreen.h"
#include "Node.h"
#include "ScoreStore.h"
#include "SimpleButton.h"

#include <string>
#include <vector>

class HighscoresNode : public Node {
public:
    HighscoresNode(SDL_Renderer* renderer = nullptr, Node* parent = nullptr)
        : Node(renderer, parent), selectorScreen(nullptr)
    {
        const char* gameIds[] = {"brick_breaker", "tetris", "frogger"};
        const char* gameNames[] = {"BRICK BREAKER", "TETRIS", "FROGGER"};

        selectorScreen = createMenuScreen();
        selectorScreen->addTextureToScreen(createImage(renderer,
            "highscoresNodeImages/highscoresNodeScreenBackground.png", true));

        ArcadeTexture* selectorTitle = createSimpleText(renderer,
            "fonts/retro/italic.ttf", 72, "CHOOSE A GAME", 255, 255, 0);
        selectorTitle->setPosition(CENTER_X, 35);
        selectorScreen->addTextureToScreen(selectorTitle);

        for (int game = 0; game < 3; ++game) {
            gameIds_.push_back(gameIds[game]);
            MenuScreen* scoreScreen = createMenuScreen();
            scoreScreen->addTextureToScreen(createImage(renderer,
                "highscoresNodeImages/highscoresNodeScreenBackground.png", true));

            ArcadeTexture* title = createSimpleText(renderer,
                "fonts/retro/italic.ttf", 62, gameNames[game], 255, 255, 0);
            title->setPosition(CENTER_X, 20);
            scoreScreen->addTextureToScreen(title);

            std::vector<ArcadeTexture*> lines;
            for (int rank = 0; rank < 10; ++rank) {
                const std::string rankLabel = rank + 1 < 10
                    ? " " + std::to_string(rank + 1) : std::to_string(rank + 1);
                ArcadeTexture* line = createSimpleText(renderer,
                    "fonts/pixel/classic.ttf", 18,
                    rankLabel + ".  ---", 255, 255, 255);
                line->setPosition(220, 95 + rank * 27);
                scoreScreen->addTextureToScreen(line);
                lines.push_back(line);
            }
            scoreLines_.push_back(lines);

            SimpleButton* backButton = createSimpleTextButton(renderer,
                "fonts/pixel/classic.ttf", 24, "BACK", 255, 0, 0);
            backButton->setButtonPosition(35, windowHeight - 45);
            backButton->setButtonAction(createAction(MOVE_SCREENS, selectorScreen));
            scoreScreen->addButtonToScreen(backButton);

            SimpleButton* menuButton = createSimpleTextButton(renderer,
                "fonts/pixel/classic.ttf", 24, "MAIN MENU", 255, 0, 0);
            menuButton->setButtonPosition(windowWidth - menuButton->getWidth() - 35,
                windowHeight - 45);
            menuButton->setButtonAction(createAction(MOVE_NODES, getParentNode()));
            scoreScreen->addButtonToScreen(menuButton);

            SimpleButton* gameButton = createSimpleTextButton(renderer,
                "fonts/pixel/classic.ttf", 28, gameNames[game], 255, 0, 0);
            gameButton->setButtonPosition(windowWidth / 2 - gameButton->getWidth() / 2,
                150 + game * 65);
            gameButton->setButtonAction(createAction(MOVE_SCREENS, scoreScreen));
            selectorScreen->addButtonToScreen(gameButton);

            addScreen(scoreScreen);
        }

        SimpleButton* mainMenuButton = createSimpleTextButton(renderer,
            "fonts/pixel/classic.ttf", 25, "MAIN MENU", 255, 0, 0);
        mainMenuButton->setButtonPosition(windowWidth / 2 - mainMenuButton->getWidth() / 2,
            windowHeight - 55);
        mainMenuButton->setButtonAction(createAction(MOVE_NODES, getParentNode()));
        selectorScreen->addButtonToScreen(mainMenuButton);

        addScreen(selectorScreen);
        setCurrentScreen(selectorScreen);
        enter();
    }

    void enter()
    {
        setCurrentScreen(selectorScreen);
        for (std::size_t game = 0; game < gameIds_.size(); ++game) {
            const std::vector<int> scores = ScoreStore::load(gameIds_[game]);
            for (std::size_t rank = 0; rank < scoreLines_[game].size(); ++rank) {
                const std::string rankLabel = rank + 1 < 10
                    ? " " + std::to_string(rank + 1) : std::to_string(rank + 1);
                const std::string score = rank < scores.size()
                    ? std::to_string(scores[rank]) : "---";
                scoreLines_[game][rank]->loadFromRenderedText(
                    "fonts/pixel/classic.ttf", 18,
                    rankLabel + ".  " + score,
                    {255, 255, 255, 255});
                scoreLines_[game][rank]->setSize(WIDTH_ORIGINAL, HEIGHT_ORIGINAL);
            }
        }
    }

private:
    MenuScreen* selectorScreen;
    std::vector<std::string> gameIds_;
    std::vector<std::vector<ArcadeTexture*> > scoreLines_;
};

#endif
