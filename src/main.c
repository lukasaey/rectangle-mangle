#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "block.h"
#include "constants.h"
#include "raylib.h"
#include "raymath.h"

#include "block.h"
#include "constants.h"
#include "vector_fns.h"

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

void draw_field_cell(Vector2 pos, Color color, bool transparent) {
    Color mod_color = ColorAlpha(color, transparent ? 0.5f : 1.0f);

    DrawRectangleV(pos, (Vector2){BLOCK_CELL_WIDTH, BLOCK_CELL_HEIGHT},
                   ColorBrightness(mod_color, -0.225f));
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

void draw_field(Color *field, int root_x, int root_y) {
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

    Color field[FIELD_SIZE * FIELD_SIZE];

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
            if (placed_block_space_free(field, clamped_coords, &temp_block)) {
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
        draw_field(field, board_x, board_y);

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