#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <raylib.h>

/* Screen */
#define WIDTH 900
#define HEIGHT 600

/* Update Time */
#define TARGET_FPS 60
#define UPDATES_PER_S 2

/* Grid Properties */
#define GRID_ROWS 20
#define GRID_COLS 20
#define PERCENTAGE_ACCURACY \
  100  // number of 0s correspond to digits of accuracy
#define GRID_BEGIN_ALIVE_PERCENT 0.35

/* Visual Properties*/
#define LINE_THICKNESS 2
#define BORDER_COLOR WHITE
#define CELL_COLOR RED

/* Modify grid internals */
void init_grid(uint8_t grid[GRID_ROWS][GRID_COLS]);
void update_grid(uint8_t grid[GRID_ROWS][GRID_COLS]);

/* Draw to screen */
void draw_grid(uint8_t grid[GRID_ROWS][GRID_COLS]);

/* Helpers */
uint8_t cell_fate(uint8_t grid[GRID_ROWS][GRID_COLS], uint32_t y, uint32_t x);
static inline uint32_t bound_check(uint32_t coord,
                                   int8_t direction,
                                   uint32_t max);

/* Settings modified at runtime */
uint8_t INTERACTIVE = 0;

int main(int argc, char** argv) {
  int c;
  uint8_t verbose = 0;
  while ((c = getopt(argc, argv, "vi")) != -1) {
    if (c == 'v') {
      verbose = 1;
    } else if (c == 'i') {
      INTERACTIVE = 1;
    }
  }

  if (!verbose) {
    int logfd = open("/dev/null", O_WRONLY);
    if (logfd == -1) {
      perror("open");
      return 1;
    }

    dup2(logfd, STDOUT_FILENO);
    dup2(logfd, STDERR_FILENO);
  }

  uint8_t grid[GRID_ROWS][GRID_COLS];
  if (!INTERACTIVE)
    init_grid(grid);
  else
    memset(grid, 0, GRID_ROWS * GRID_COLS);

  uint8_t game_started = (!INTERACTIVE);
  float update_time = 0;

  InitWindow(WIDTH, HEIGHT, "Game of Life");
  SetTargetFPS(TARGET_FPS);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    if (IsKeyPressed(KEY_Q))
      goto done;
    if (IsKeyPressed(KEY_ENTER) && INTERACTIVE)
      game_started = 1;
    if (IsKeyPressed(KEY_R)) {
      if (INTERACTIVE) {
        memset(grid, 0, GRID_ROWS * GRID_COLS);
        game_started = 0;
      } else {
        init_grid(grid);
      }
    }

    if (INTERACTIVE) {
      uint32_t row = (GRID_ROWS) / ((float)HEIGHT / GetMouseY());
      uint32_t col = (GRID_COLS) / ((float)WIDTH / GetMouseX());
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        grid[row][col] = !grid[row][col];
      }
    }

    if (game_started) {
      if (update_time <= 0) {
        update_grid(grid);
        update_time = 1.0f / UPDATES_PER_S;
      }
      update_time -= GetFrameTime();
    }

    draw_grid(grid);
    EndDrawing();
  }

done:
  EndDrawing();

  return 0;
}

void init_grid(uint8_t grid[GRID_ROWS][GRID_COLS]) {
  srand(time(NULL));
  int divisor = floor((PERCENTAGE_ACCURACY * GRID_BEGIN_ALIVE_PERCENT));
  for (uint32_t y = 0; y < GRID_ROWS; ++y) {
    for (uint32_t x = 0; x < GRID_COLS; ++x) {
      if ((rand() % PERCENTAGE_ACCURACY) < divisor) {
        grid[y][x] = 1;
      } else {
        grid[y][x] = 0;
      }
    }
  }
}

void update_grid(uint8_t grid[GRID_ROWS][GRID_COLS]) {
  uint8_t grid_copy[GRID_ROWS][GRID_COLS];
  memcpy(grid_copy, grid, GRID_ROWS * GRID_COLS * sizeof(uint8_t));

  for (uint32_t y = 0; y < GRID_ROWS; ++y) {
    for (uint32_t x = 0; x < GRID_COLS; ++x) {
      grid[y][x] = cell_fate(grid_copy, y, x);
    }
  }
}

void draw_grid(uint8_t grid[GRID_ROWS][GRID_COLS]) {
  /* Draw the cells */
  for (uint32_t y = 0; y < GRID_ROWS; ++y) {
    for (uint32_t x = 0; x < GRID_COLS; ++x) {
      if (grid[y][x] == 1) {
        DrawRectangle(x * ((float)WIDTH / GRID_COLS),
                      y * ((float)HEIGHT / GRID_ROWS), (float)WIDTH / GRID_COLS,
                      (float)HEIGHT / GRID_ROWS, CELL_COLOR);
      }
    }
  }

  /* Draw the grid borders */
  uint32_t num_cols_to_draw = GRID_COLS + 1;
  uint32_t num_rows_to_draw = GRID_ROWS + 1;
  for (uint32_t col = 0; col < num_cols_to_draw; ++col) {
    float x = col * ((float)WIDTH / GRID_COLS);
    Vector2 start_pos = {x, 0};
    Vector2 end_pos = {x, HEIGHT};
    DrawLineEx(start_pos, end_pos, LINE_THICKNESS, BORDER_COLOR);
  }
  for (uint32_t row = 0; row < num_rows_to_draw; ++row) {
    float y = row * ((float)HEIGHT / GRID_ROWS);
    Vector2 start_pos = {0, y};
    Vector2 end_pos = {WIDTH, y};
    DrawLineEx(start_pos, end_pos, LINE_THICKNESS, BORDER_COLOR);
  }
}

// B3/S23
uint8_t cell_fate(uint8_t grid[GRID_ROWS][GRID_COLS], uint32_t y, uint32_t x) {
  uint32_t alive_neighbors = 0;

  uint32_t neg_y = bound_check(y, -1, GRID_ROWS);
  uint32_t reg_y = bound_check(y, 0, GRID_ROWS);
  uint32_t pos_y = bound_check(y, 1, GRID_ROWS);

  uint32_t neg_x = bound_check(x, -1, GRID_COLS);
  uint32_t reg_x = bound_check(x, 0, GRID_COLS);
  uint32_t pos_x = bound_check(x, 1, GRID_COLS);

  /* Up - LCR */
  alive_neighbors += grid[neg_y][neg_x];  // up left
  alive_neighbors += grid[neg_y][reg_x];  // up same
  alive_neighbors += grid[neg_y][pos_x];  // up right

  /* Same y - LR */
  alive_neighbors += grid[reg_y][neg_x];  // same left
  alive_neighbors += grid[reg_y][pos_x];  // same right

  /* Down - LCR */
  alive_neighbors += grid[pos_y][neg_x];  // down left
  alive_neighbors += grid[pos_y][reg_x];  // down same
  alive_neighbors += grid[pos_y][pos_x];  // down right

  if (grid[y][x] == 1)
    return (alive_neighbors == 2 || alive_neighbors == 3);
  else
    return alive_neighbors == 3;
}

static inline uint32_t bound_check(uint32_t coord,
                                   int8_t direction,
                                   uint32_t max) {
  if (coord >= max - 1 && direction >= 1)
    return 0;
  if (coord == 0 && direction < 0)
    return max - 1;

  return coord + direction;
}
