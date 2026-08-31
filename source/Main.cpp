#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "Config.h"
#include "RootNode.h"
#include "SoundEffects.h"

#include <cstdio>

namespace {
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::fprintf(stderr, "SDL could not initialize: %s\n", SDL_GetError());
        return false;
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        std::fprintf(stderr, "SDL_image could not initialize: %s\n", IMG_GetError());
        return false;
    }
    if (TTF_Init() == -1) {
        std::fprintf(stderr, "SDL_ttf could not initialize: %s\n", TTF_GetError());
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::fprintf(stderr, "SDL_mixer could not initialize: %s\n", Mix_GetError());
        return false;
    }
    SoundEffects::initialize();

    window = SDL_CreateWindow("Arcade System", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::fprintf(stderr, "Window could not be created: %s\n", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::fprintf(stderr, "Renderer could not be created: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    return true;
}

void shutdown()
{
    Mix_HaltMusic();
    SoundEffects::shutdown();
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
    window = nullptr;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
} // namespace

int main(int, char**)
{
    if (!initialize()) {
        shutdown();
        return 1;
    }

    {
        RootNode rootNode(renderer, nullptr);
        Node* currentNode = &rootNode;
        Mix_Music* currentMusic = nullptr;
        bool soundEnabled = false;
        bool quit = false;

        while (!quit) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                    continue;
                }

                const Action action = currentNode->update(&event);
                switch (action.actionName) {
                case MOVE_NODES:
                    if (action.actionParameter) {
                        currentNode->exitNode();
                        currentNode = static_cast<Node*>(action.actionParameter);
                        currentNode->enter();
                    }
                    break;
                case CHANGE_SOUND:
                    soundEnabled = action.actionParameter != nullptr;
                    SoundEffects::setEnabled(soundEnabled);
                    if (soundEnabled) SoundEffects::playClick();
                    if (soundEnabled && currentMusic) Mix_PlayMusic(currentMusic, -1);
                    else Mix_HaltMusic();
                    break;
                case CHANGE_MUSIC:
                    currentMusic = static_cast<Mix_Music*>(action.actionParameter);
                    if (soundEnabled && currentMusic) Mix_PlayMusic(currentMusic, -1);
                    else Mix_HaltMusic();
                    break;
                case QUIT_GAME:
                    quit = true;
                    break;
                default:
                    break;
                }
            }

            currentNode->update(nullptr);
            SDL_RenderClear(renderer);
            currentNode->render(renderer);
            SDL_RenderPresent(renderer);
        }
    }

    shutdown();
    return 0;
}
