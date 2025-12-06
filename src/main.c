#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

#define FIELD_SIZE 8
#define FIELD_WIDTH 500
#define FIELD_HEIGHT 500

#define FIELD_BORDER_THICKNESS 1

#define FIELD_CELL_HEIGHT \
    ((float)(FIELD_HEIGHT - FIELD_BORDER_THICKNESS * (FIELD_SIZE)) / FIELD_SIZE)
#define FIELD_CELL_WIDTH \
    ((float)(FIELD_WIDTH - FIELD_BORDER_THICKNESS * (FIELD_SIZE)) / FIELD_SIZE)

#define BLOCK_CELL_HEIGHT FIELD_CELL_HEIGHT
#define BLOCK_CELL_WIDTH FIELD_CELL_WIDTH
#define BLOCK_CELL_BORDER_THICKNESS (6)

#define EMPTY_CELL_COLOR ColorBrightness(LIGHTGRAY, -0.225f)
#define FIELD_BORDER_COLOR DARKGRAY

Color field[FIELD_SIZE * FIELD_SIZE];

static inline float apply_board_offset(float v) {
    return v + FIELD_BORDER_THICKNESS * 1.5;
}

static inline Vector2 apply_board_offset_v(Vector2 v) {
    return (Vector2){apply_board_offset(v.x), apply_board_offset(v.y)};
}

Vector2 project_mouse_on_board(Vector2 field_pos, Vector2 mouse_pos) {
    Vector2 norm_mouse_pos = Vector2Subtract(mouse_pos, field_pos);
    Vector2 coords = Vector2Divide(
        norm_mouse_pos, (Vector2){(FIELD_CELL_WIDTH + FIELD_BORDER_THICKNESS),
                                  FIELD_CELL_HEIGHT + FIELD_BORDER_THICKNESS});

    return coords;
}

Vector2 translate_board_coords(Vector2 field_pos, Vector2 coords) {
    Vector2 additional_offset = Vector2Scale(coords, FIELD_BORDER_THICKNESS);
    return Vector2Add(
        Vector2Add(apply_board_offset_v(field_pos), additional_offset),
        Vector2Multiply(coords,
                        (Vector2){FIELD_CELL_WIDTH, FIELD_CELL_HEIGHT}));
}

static inline Vector2 Vector2Floor(Vector2 v) {
    return (Vector2){floorf(v.x), floorf(v.y)};
}

static inline int vector_field_index(Vector2 v) {
    return roundf(v.x) + roundf(v.y) * FIELD_SIZE;
}

static inline bool vector_in_field_bounds(Vector2 v) {
    return v.x >= 0 && v.y >= 0 && v.x < FIELD_SIZE && v.y < FIELD_SIZE;
}

typedef struct Block {
    Color color;
    int n_cells;
    int rotation;  // how many right angle rotations to the right
    const Vector2* cell_coords;
} Block;

Vector2 get_block_cell_coord(const Block* block, int i) {
    return Vector2Rotate(block->cell_coords[i], PI * 0.5 * block->rotation);
}

bool placed_block_fits_in_field(Vector2 coords, const Block* block) {
    for (int i = 0; i < block->n_cells; ++i) {
        Vector2 cell_pos = Vector2Add(coords, get_block_cell_coord(block, i));
        if (!vector_in_field_bounds(cell_pos)) return false;
    }
    return true;
}

bool placed_block_space_free(Vector2 coords, const Block* block) {
    for (int i = 0; i < block->n_cells; ++i) {
        Vector2 cell_pos = Vector2Add(coords, get_block_cell_coord(block, i));
        Color cell_color = field[vector_field_index(cell_pos)];
        if (!ColorIsEqual(cell_color, EMPTY_CELL_COLOR)) {
            return false;
        }
    }
    return true;
}

Vector2 clamp_block_pos_to_field(Vector2 coords, const Block* block) {
    for (int i = 0; i < block->n_cells; ++i) {
        Vector2 cell_pos = Vector2Add(coords, get_block_cell_coord(block, i));
        if (cell_pos.x < 0)
            coords.x += -cell_pos.x;
        else if (cell_pos.x > 7)
            coords.x -= cell_pos.x - 7;
        if (cell_pos.y < 0)
            coords.y += -cell_pos.y;
        else if (cell_pos.y > 7)
            coords.y -= cell_pos.y - 7;
    }
    return coords;
}

#define BLOCK_2X2(clr, rot) \
    ((Block){               \
        .color = (clr),     \
        .n_cells = 4,       \
        .rotation = (rot),  \
        .cell_coords = (const Vector2[]){{0, 0}, {1, 0}, {0, 1}, {1, 1}}})

#define BLOCK_3X3(clr, rot)                           \
    ((Block){.color = (clr),                          \
             .n_cells = 9,                            \
             .rotation = (rot),                       \
             .cell_coords = (const Vector2[]){{0, 0}, \
                                              {0, 1}, \
                                              {0, 2}, \
                                              {1, 0}, \
                                              {1, 1}, \
                                              {1, 2}, \
                                              {2, 0}, \
                                              {2, 1}, \
                                              {2, 2}}})

#define BLOCK_3X2(clr, rot)                    \
    ((Block){.color = (clr),                   \
             .n_cells = 6,                     \
             .rotation = (rot),                \
             .cell_coords = (const Vector2[]){ \
                 {0, 0},                       \
                 {0, 1},                       \
                 {1, 0},                       \
                 {1, 1},                       \
                 {2, 0},                       \
                 {2, 1},                       \
             }})

#define BLOCK_2X3(clr, rot)                    \
    ((Block){.color = (clr),                   \
             .n_cells = 6,                     \
             .rotation = (rot),                \
             .cell_coords = (const Vector2[]){ \
                 {0, 0},                       \
                 {0, 1},                       \
                 {0, 2},                       \
                 {1, 0},                       \
                 {1, 1},                       \
                 {1, 2},                       \
             }})

#define BLOCK_L(clr, rot)  \
    ((Block){              \
        .color = (clr),    \
        .rotation = (rot), \
        .n_cells = 4,      \
        .cell_coords = (const Vector2[]){{0, 0}, {1, 1}, {0, 1}, {0, -1}}})

#define BLOCK_1x4(clr, rot) \
    ((Block){               \
        .color = (clr),     \
        .rotation = (rot),  \
        .n_cells = 4,       \
        .cell_coords = (const Vector2[]){{0, -1}, {0, 0}, {0, 1}, {0, 2}}})

#define BLOCK_1x5(clr, rot)     \
    ((Block){.color = (clr),    \
             .rotation = (rot), \
             .n_cells = 5,      \
             .cell_coords =     \
                 (const Vector2[]){{0, -2}, {0, -1}, {0, 0}, {0, 1}, {0, 2}}})

void draw_field_cell(Vector2 pos, Color color, bool transparent) {
    Color mod_color = ColorAlpha(color, transparent ? 0.5f : 1.0f);

    DrawRectangleV(pos, (Vector2){BLOCK_CELL_WIDTH, BLOCK_CELL_HEIGHT},
                   ColorBrightness(mod_color, -0.225f));
    // DrawRectangleV(
    //     Vector2Add(pos, (Vector2){(BLOCK_CELL_BORDER_THICKNESS * 2) / 2,
    //                               (BLOCK_CELL_BORDER_THICKNESS * 2) / 2}),
    //     Vector2Add((Vector2){BLOCK_CELL_WIDTH, BLOCK_CELL_HEIGHT},
    //                (Vector2){-(BLOCK_CELL_BORDER_THICKNESS * 2),
    //                          -(BLOCK_CELL_BORDER_THICKNESS * 2)}),
    //     mod_color);
}

void draw_block_cell(Vector2 pos, Color color, bool transparent) {
    Color mod_color = ColorAlpha(color, transparent ? 0.5f : 1.0f);

    DrawRectangleV(pos, (Vector2){BLOCK_CELL_WIDTH, BLOCK_CELL_HEIGHT},
                   ColorBrightness(mod_color, -0.3f));
    DrawRectangleV(pos,
                   Vector2Add((Vector2){BLOCK_CELL_WIDTH, BLOCK_CELL_HEIGHT},
                              (Vector2){-(BLOCK_CELL_BORDER_THICKNESS),
                                        -(BLOCK_CELL_BORDER_THICKNESS)}),
                   ColorBrightness(mod_color, 0));
    DrawRectangleV(
        Vector2Add(pos, (Vector2){(BLOCK_CELL_BORDER_THICKNESS * 2) / 2,
                                  (BLOCK_CELL_BORDER_THICKNESS * 2) / 2}),
        Vector2Add((Vector2){BLOCK_CELL_WIDTH, BLOCK_CELL_HEIGHT},
                   (Vector2){-(BLOCK_CELL_BORDER_THICKNESS * 2),
                             -(BLOCK_CELL_BORDER_THICKNESS * 2)}),
        ColorBrightness(mod_color, -0.075f));
}

void draw_block(const Block* block, Vector2 pos, bool transparent) {
    for (int i = 0; i < block->n_cells; ++i) {
        draw_block_cell(
            Vector2Add(
                pos,
                Vector2Multiply(
                    get_block_cell_coord(block, i),
                    (Vector2){BLOCK_CELL_WIDTH + FIELD_BORDER_THICKNESS,
                              BLOCK_CELL_HEIGHT + FIELD_BORDER_THICKNESS})),
            block->color, transparent);
    }
}

void draw_field(int root_x, int root_y) {
    DrawRectangle(root_x, root_y, FIELD_WIDTH, FIELD_HEIGHT,
                  FIELD_BORDER_COLOR);
    for (int i = 0; i < FIELD_SIZE * FIELD_SIZE; ++i) {
        Vector2 cell_pos = (Vector2){
            apply_board_offset(root_x) +
                (i % FIELD_SIZE) * (FIELD_CELL_WIDTH + FIELD_BORDER_THICKNESS),
            apply_board_offset(root_y) +
                (i / FIELD_SIZE) *
                    (FIELD_CELL_HEIGHT + FIELD_BORDER_THICKNESS)};
        if (ColorIsEqual(field[i], EMPTY_CELL_COLOR)) {
            draw_field_cell(cell_pos, field[i], false);
        } else {
            draw_block_cell(cell_pos, field[i], false);
        }
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "rectangle mangle");

    SetTargetFPS(60);

    for (int i = 0; i < FIELD_SIZE * FIELD_SIZE; ++i) {
        field[i] = EMPTY_CELL_COLOR;
    }

    int board_x = 150;
    int board_y = 50;

    int rotation = 0;

    while (!WindowShouldClose()) {
        Vector2 mouse_field_coords = project_mouse_on_board(
            (Vector2){board_x, board_y}, GetMousePosition());

        Block temp_block = BLOCK_L(YELLOW, rotation);

        float wheel = GetMouseWheelMove();
        rotation = (unsigned int)(rotation + wheel) % 4;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            vector_in_field_bounds(mouse_field_coords)) {
            Vector2 clamped_coords = clamp_block_pos_to_field(
                Vector2Floor(mouse_field_coords), &temp_block);
            if (placed_block_space_free(clamped_coords, &temp_block)) {
                for (int i = 0; i < temp_block.n_cells; ++i) {
                    Vector2 cell_pos = Vector2Add(
                        clamped_coords, get_block_cell_coord(&temp_block, i));
                    int index = vector_field_index(cell_pos);
                    field[index] = temp_block.color;
                }
            }
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_field(board_x, board_y);

        if (vector_in_field_bounds(mouse_field_coords)) {
            Vector2 clamped_coords = clamp_block_pos_to_field(
                Vector2Floor(mouse_field_coords), &temp_block);
            Vector2 translated_pos = translate_board_coords(
                (Vector2){board_x, board_y}, (clamped_coords));
            draw_block(&temp_block, translated_pos, true);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}