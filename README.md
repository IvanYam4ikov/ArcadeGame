# Arcade System

A small retro arcade front end built in C++ with SDL2. It includes a navigable
main menu, game carousel, audio options, a high-score screen, and a complete
single-level Breakout game and a playable Tetris core.

![Arcade System main menu](source/rootNodeImages/rootNodeScreenBackground.png)

## Status

This is a working portfolio MVP restored from an older student project. The
Breakout includes paddle control, brick collision, scoring, three lives, and an
endless advancing board: clearing the lowest row moves the remaining bricks
down and adds a new row at the top. Tetris now has a 10x20 board with all seven
tetrominoes, movement, rotation, gravity, collision, locking, respawning, and
game-over detection. Completed rows are removed, scoring follows the standard
single/double/triple/Tetris values, and the game accelerates every ten lines.
A seven-piece bag keeps piece selection fair, the next piece is previewed, and
an outlined ghost piece shows where the active piece will land. Tetris scores
are saved to its own leaderboard. Frogger remains a menu concept.

The Sound option controls generated menu-click and Breakout collision effects
as well as the selected background music. Clearing the entire board restores
all four rows and returns the ball to the paddle for the next serve.

The ball advances through five speed tiers, increasing every 1,000 points.
Breakout begins with the ball parked on the paddle; press `Space` to serve.
When all lives are lost, the score is written to
`source/highscores_brick_breaker.txt`. The Highscores menu lets you choose
Brick Breaker, Tetris, or Frogger, each backed by its own top-ten score file.
These local player-data files are intentionally ignored by Git.

Every fifth destroyed brick drops a power-up. The drops cycle through a wider
paddle, multiball, an extra life, temporary slow motion, and a temporary
piercing ball that passes through bricks. Pink, cyan, green, blue, and orange
pixel-art drops represent those effects respectively. Timed effects last ten
seconds.

Paddle impact position controls the ball's outgoing angle, while minimum
horizontal and vertical speeds prevent repetitive flat or near-vertical loops.
Long status messages wrap onto multiple screen rows.

## Build on macOS

The repository currently includes the SDL2 headers and libraries used by the
original macOS build. Xcode command-line tools are required.

```sh
make
make run
```

Run the executable from `source/` because asset paths are resolved from that
directory. Use the mouse to navigate. In Breakout, move with the arrow keys or
`A`/`D`, press `Space` to serve after losing a ball, `N` to start a new game,
`P` to pause or resume with a three-second countdown, and `Q` or `Escape` to
return to the carousel.

In Tetris, use Left/Right to move, Down to soft drop, Up or `Z` to rotate,
`Space` to hard drop, `N` to restart after game over, and `Q` or `Escape` to
return to the carousel.

To remove generated object files and the executable:

```sh
make clean
```

## Project layout

- `source/` — application entry point and runtime assets
- `GUI_files/headers/` — menu, screen, node, entity, and game interfaces
- `GUI_files/cpp_files/` — implementation files
- `SDL_files/` — SDL2 headers and macOS libraries from the original project

## Next steps

- Add additional Breakout levels, sound effects, and persistent high scores.
- Add a ghost piece, hold-piece support, and pause handling to Tetris.
- Implement or remove the Frogger placeholder.
- Replace the bundled SDL distribution with package-manager/CMake discovery for
  portable Linux and Windows builds.
- Add automated tests around navigation and game physics.
