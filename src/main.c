#include <string.h>

#include "raylib.h"
#include "raymath.h"

#define FIELD_SIZE 8
#define FIELD_WIDTH 500
#define FIELD_HEIGHT 500

#define FIELD_BORDER_THICKNESS 6

#define FIELD_CELL_HEIGHT ((FIELD_HEIGHT - FIELD_BORDER_THICKNESS * 9) / 8)
#define FIELD_CELL_WIDTH ((FIELD_WIDTH - FIELD_BORDER_THICKNESS * 9) / 8)

#define BLOCK_CELL_HEIGHT FIELD_CELL_HEIGHT
#define BLOCK_CELL_WIDTH FIELD_CELL_WIDTH
#define BLOCK_CELL_BORDER_THICKNESS (FIELD_BORDER_THICKNESS)

#define EMPTY_CELL_COLOR LIGHTGRAY
#define FIELD_BACKGROUND_COLOR DARKBLUE

Color field[FIELD_SIZE * FIELD_SIZE];

float apply_board_offset(float v) { return v + FIELD_BORDER_THICKNESS * 1.5; }

Vector2 apply_board_offset_v(Vector2 v) {
    return (Vector2){apply_board_offset(v.x), apply_board_offset(v.y)};
}

Vector2 translate_board_coords(Vector2 board_size, Vector2 coords) {
    Vector2 additional_offset = Vector2Scale(coords, FIELD_BORDER_THICKNESS);
    return Vector2Add(Vector2Add(apply_board_offset_v(board_size), additional_offset),
                      Vector2Multiply(coords, (Vector2){FIELD_CELL_WIDTH,
                                                        FIELD_CELL_HEIGHT}));
}

void draw_field(int root_x, int root_y) {
    DrawRectangle(root_x, root_y, FIELD_WIDTH, FIELD_HEIGHT,
                  FIELD_BACKGROUND_COLOR);
    for (int i = 0; i < FIELD_SIZE * FIELD_SIZE; ++i) {
        DrawRectangle(
            apply_board_offset(root_x) +
                (i % 8) * (FIELD_CELL_WIDTH + FIELD_BORDER_THICKNESS),
            apply_board_offset(root_y) +
                (i / 8) * (FIELD_CELL_HEIGHT + FIELD_BORDER_THICKNESS),
            FIELD_CELL_WIDTH, FIELD_CELL_HEIGHT, field[i]);
    }
}

typedef struct Block {
    Color color;
    int n_cells;
    const Vector2* cell_coords;
} Block;

#define BLOCK_2X2(clr)       \
    ((Block){.color = (clr), \
             .n_cells = 4,   \
             .cell_coords = (const Vector2[]){{0, 0}, {1, 0}, {0, 1}, {1, 1}}})

#define BLOCK_L(clr)       \
    ((Block){.color = (clr), \
             .n_cells = 4,   \
             .cell_coords = (const Vector2[]){{0, 0}, {1, 1}, {0, 1}, {0, -1}}})

void draw_block_cell(Vector2 pos, Color color) {
    DrawRectangleV(pos, (Vector2){BLOCK_CELL_HEIGHT, BLOCK_CELL_WIDTH},
                   ColorBrightness(color, -0.25f));
    DrawRectangleV(
        Vector2Add(pos, (Vector2){(BLOCK_CELL_BORDER_THICKNESS * 2) / 2,
                                  (BLOCK_CELL_BORDER_THICKNESS * 2) / 2}),
        Vector2Add((Vector2){BLOCK_CELL_HEIGHT, BLOCK_CELL_WIDTH},
                   (Vector2){-(BLOCK_CELL_BORDER_THICKNESS * 2),
                             -(BLOCK_CELL_BORDER_THICKNESS * 2)}),
        color);
}

void draw_block(const Block* block, Vector2 pos) {
    for (int i = 0; i < block->n_cells; ++i) {
        draw_block_cell(
            Vector2Add(
                pos,
                Vector2Multiply(
                    block->cell_coords[i],
                    (Vector2){BLOCK_CELL_HEIGHT + BLOCK_CELL_BORDER_THICKNESS,
                              BLOCK_CELL_WIDTH + BLOCK_CELL_BORDER_THICKNESS})),
            block->color);
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight,
               "raylib [core] example - basic window");

    SetTargetFPS(60);

    for (int i = 0; i < FIELD_SIZE * FIELD_SIZE; ++i) {
        field[i] = EMPTY_CELL_COLOR;
    }

    Block b1 = BLOCK_2X2(RED);
    Block b2 = BLOCK_2X2(GREEN);
    Block b3 = BLOCK_L(PURPLE);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_field(150, 50);
        draw_block(&b1, translate_board_coords((Vector2){150, 50}, (Vector2){0, 0}));
        draw_block(&b2, translate_board_coords((Vector2){150, 50}, (Vector2){2, 0}));
        draw_block(&b3, translate_board_coords((Vector2){150, 50}, (Vector2){4, 2}));
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}