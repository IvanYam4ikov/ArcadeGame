# Arcade System

A retro desktop arcade platform built in C++ with SDL2. The application brings
Brick Breaker, Tetris, and Frogger together behind a shared menu system with
audio settings, instructions, persistent high scores, and a consistent
pixel-art presentation.

![Arcade System main menu](source/rootNodeImages/rootNodeScreenBackground.png)

## Features

- Three complete, independently scored games
- Mouse-driven menus and keyboard-controlled gameplay
- Shared Controls screen with instructions for every game
- Configurable sound effects and background music
- Separate persistent top-ten leaderboard for each game
- Reusable object-oriented screen, node, entity, texture, and game-state
  components
- Pixel-art graphics and retro fonts throughout the interface

## Games

### Brick Breaker

Brick Breaker features paddle-based ball physics, collision-aware bounce angles,
scoring, three lives, and five speed tiers. The ball begins parked on the paddle
and returns there before each serve, preventing an immediate lost life when a
round begins.

The brick field continues indefinitely. Clearing its lowest occupied row moves
the remaining bricks downward and adds a new row at the top. Clearing the entire
field restores all four starting rows and parks the ball for the next serve.

Every fifth destroyed brick drops one of five randomized pixel-art power-ups:

- Wider paddle
- Multiball
- Extra life
- Temporary slow motion
- Temporary piercing ball

Timed effects last ten seconds. Multiball spawning is constrained to the arena,
and the paddle speed scales with the ball so later speed tiers remain playable.
The game also includes pause handling and a three-second resume countdown.

### Tetris

Tetris uses a 10x20 board and includes all seven tetrominoes, gravity, collision
detection, piece locking, line clearing, wall kicks, soft drops, hard drops, and
game-over detection.

Piece generation uses a shuffled seven-piece bag. The interface displays the
next tetromino as a centered pixel-art shape, while an outlined ghost piece
previews the active piece's landing position. Scoring follows the standard
single, double, triple, and Tetris values; the level and falling speed increase
after every ten cleared lines.

### Frogger

Frogger includes four traffic lanes, four river lanes, moving vehicles and logs,
five home bays, three lives, and a timer for each crossing. The frog moves with
the supporting log while in the river and loses a life after hitting traffic,
landing in the water, leaving the arena, or running out of time.

Players earn points for forward progress, reaching a home bay, and finishing
with time remaining. Filling all five homes awards a completion bonus and starts
a faster round. Frogs, cars, trucks, logs, and completed homes use dedicated
pixel-art sprites.

## Controls

The same information is available from the application's **Controls** menu.

| Game | Controls |
| --- | --- |
| Brick Breaker | `Left`/`Right` or `A`/`D` to move, `Space` to serve, `P` to pause |
| Tetris | `Left`/`Right` to move, `Down` to soft drop, `Up` or `Z` to rotate, `Space` to hard drop |
| Frogger | Arrow keys or `W`/`A`/`S`/`D` to hop |
| All games | `N` to restart, `Q` or `Escape` to return to the carousel |

## Architecture

The project applies object-oriented design to separate navigation, presentation,
and gameplay responsibilities:

- `Node` objects organize the main menu, game carousel, settings, controls,
  leaderboards, and individual games.
- `Screen` classes coordinate input, updates, rendering, and transitions.
- Reusable `Entity` and `ArcadeTexture` abstractions represent interactive
  objects and SDL textures.
- Each game owns its rules and state while sharing navigation, audio, and score
  persistence services.
- `ScoreStore` maintains an independent local top-ten score file for each game.

Sound effects are synthesized at runtime and respect the global Sound option.
The Options menu also provides calm, intense, and disabled background-music
choices.

## Build on macOS

The repository currently includes the SDL2 headers and libraries used by the
original macOS build. Xcode Command Line Tools are required.

```sh
git clone <repository-url>
cd ArcadeSystem_mac
make
make run
```

`make run` launches the executable from `source/`, where the application expects
to resolve its runtime assets.

To remove generated object files and the executable:

```sh
make clean
```

## Local data

High scores are stored in the following runtime files:

- `source/highscores_brick_breaker.txt`
- `source/highscores_tetris.txt`
- `source/highscores_frogger.txt`

These files contain local player data and are intentionally ignored by Git.

## Project layout

- `source/` - application entry point, fonts, audio, sprites, and other assets
- `GUI_files/headers/` - interfaces for menus, nodes, screens, entities, and games
- `GUI_files/cpp_files/` - gameplay and framework implementations
- `SDL_files/` - SDL2 headers and macOS libraries used by the current build

## Future improvements

- Add hold-piece and pause support to Tetris
- Add animation and pause support to Frogger
- Add automated tests for scoring, collision rules, and navigation
- Replace bundled SDL libraries with CMake-based dependency discovery
- Add portable Linux and Windows build configurations
