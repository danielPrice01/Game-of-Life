# Game-of-Life (Raylib, C)

**Quick project I threw together as I am learning raylib**

---

## Running
- ./game-of-life

### Prerequisites:
- need raylib installed

### Args:
- -v (verbose mode, prints stuff to stdout)
- -i (interactive mode, draw things to screen, otherwise cells are randomly chosen to be alive/dead)

--- 

## Macros to redefine

### Screen:
- WIDTH (default 900)
- HEIGHT (default 600)

### Screen updates:
- TARGET_FPS (default 60)
- UPDATES_PER_S (default 2)

### Grid properties
- GRID_ROWS (default 20)
- GRID_COLS (default 20)
- PERCENTAGE_ACCURACY (default 100, where number of 0s correspond to number of significant digits of accuracy)
- GRID_ALIVE_PERCENT (defaut 0.35, represents likelihood a cell starts off alive)

### Visual properties
- LINE_THICKNESS (default 2)
- BORDER_COLOR (default WHITE)
- CELL_COLOR (default RED)

---
## Compiling
See [https://www.raylib.com](https://www.raylib.com) for how to compile raylib project depending on OS. Otherwise no additional requirements. 
