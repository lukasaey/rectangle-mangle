#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "block.h"
#include "constants.h"
#include "raylib.h"
#include "raymath.h"
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

void draw_block_cell(Vector2 pos, Color color, bool transparent, float scale) {
    Color mod_color = ColorAlpha(color, transparent ? 0.5f : 1.0f);

    Vector2 cell_size =
        Vector2Scale((Vector2){BLOCK_CELL_WIDTH, BLOCK_CELL_HEIGHT}, scale);

    DrawRectangleV(pos, cell_size, ColorBrightness(mod_color, -0.3f));
    DrawRectangleV(
        pos,
        Vector2Add(cell_size, (Vector2){-(BLOCK_CELL_BORDER_THICKNESS),
                                        -(BLOCK_CELL_BORDER_THICKNESS)}),
        ColorBrightness(mod_color, 0));
    DrawRectangleV(
        Vector2Add(pos, (Vector2){(BLOCK_CELL_BORDER_THICKNESS * 2) / 2,
                                  (BLOCK_CELL_BORDER_THICKNESS * 2) / 2}),
        Vector2Add(cell_size, (Vector2){-(BLOCK_CELL_BORDER_THICKNESS * 2),
                                        -(BLOCK_CELL_BORDER_THICKNESS * 2)}),
        ColorBrightness(mod_color, -0.075f));
}

void draw_block(const Block* block, Vector2 pos, bool transparent,
                float scale) {
    if (block->item == CELL_ITEM_EMPTY) return;

    CellCoords cell_coords = get_shape_coords(block->shape);
    for (int i = 0; i < cell_coords.len; ++i) {
        draw_block_cell(
            Vector2Add(
                pos,
                Vector2Multiply(
                    get_block_cell_coord(block, i),
                    (Vector2){
                        (BLOCK_CELL_WIDTH + FIELD_BORDER_THICKNESS) * scale,
                        (BLOCK_CELL_HEIGHT + FIELD_BORDER_THICKNESS) * scale})),
            get_field_cell_color(block->item), transparent, scale);
    }
}

void draw_field(FieldCellItem* field, int root_x, int root_y) {
    DrawRectangle(root_x, root_y, FIELD_WIDTH, FIELD_HEIGHT,
                  FIELD_BORDER_COLOR);
    for (int i = 0; i < FIELD_SIZE * FIELD_SIZE; ++i) {
        Vector2 cell_pos = (Vector2){
            apply_board_offset(root_x) +
                (i % FIELD_SIZE) * (FIELD_CELL_WIDTH + FIELD_BORDER_THICKNESS),
            apply_board_offset(root_y) +
                (i / FIELD_SIZE) *
                    (FIELD_CELL_HEIGHT + FIELD_BORDER_THICKNESS)};
        Color color = get_field_cell_color(field[i]);
        if (field[i] == CELL_ITEM_EMPTY) {
            draw_field_cell(cell_pos, color, false);
        } else {
            draw_block_cell(cell_pos, color, false, 1.0f);
        }
    }
}

void set_field_row(FieldCellItem* field, int row, FieldCellItem item) {
    assert(row >= 0 && row < FIELD_SIZE);
    for (int i = 0; i < FIELD_SIZE; ++i) {
        field[row * FIELD_SIZE + i] = item;
    }
}

void set_field_col(FieldCellItem* field, int col, FieldCellItem item) {
    assert(col >= 0 && col < FIELD_SIZE);
    for (int i = 0; i < FIELD_SIZE; ++i) {
        field[i * FIELD_SIZE + col] = item;
    }
}

// returns the amount of points earned
int clear_field(FieldCellItem* field, int combo) {
    int lines_cleared = 0;
    int points_earned = 0;

    for (int i = 0; i < FIELD_SIZE; ++i) {
        int line_count_x = 0;
        int line_count_y = 0;

        for (int j = 0; j < FIELD_SIZE; ++j) {
            if (field[i * FIELD_SIZE + j] != CELL_ITEM_EMPTY) {
                line_count_x++;
            }
        }
        if (line_count_x == FIELD_SIZE) {
            set_field_row(field, i, CELL_ITEM_EMPTY);
            lines_cleared++;
            points_earned += line_count_x;
        }
        for (int j = 0; j < FIELD_SIZE; ++j) {
            if (field[j * FIELD_SIZE + i] != CELL_ITEM_EMPTY) {
                line_count_y++;
            }
        }
        if (line_count_y == FIELD_SIZE) {
            set_field_col(field, i, CELL_ITEM_EMPTY);
            lines_cleared++;
            points_earned += line_count_y;
        }
        line_count_x = 0;
        line_count_y = 0;
    }
    return points_earned * lines_cleared * combo;
}

static inline int wrapping_mod(int n, int M) { return ((n % M) + M) % M; }

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "rectangle mangle");

    SetTargetFPS(60);

    FieldCellItem field[FIELD_SIZE * FIELD_SIZE];

    for (int i = 0; i < FIELD_SIZE * FIELD_SIZE; ++i) {
        field[i] = CELL_ITEM_EMPTY;
    }

    int board_x = 150;
    int board_y = 75;

    int points = 0;
    int combo = 1;

    int blocks_placed = 0;

    Block held_blocks[] = {get_random_block(), get_random_block(),
                           get_random_block()};
    const int held_blocks_n = sizeof(held_blocks) / sizeof(*held_blocks);
    int block_selected = 0;

    while (!WindowShouldClose()) {
        Vector2 mouse_field_coords = project_mouse_on_board(
            (Vector2){board_x, board_y}, GetMousePosition());

        float wheel = GetMouseWheelMove();
        block_selected =
            wrapping_mod((block_selected + (int)wheel), held_blocks_n);

        Block held_block = held_blocks[block_selected];

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            vector_in_field_bounds(mouse_field_coords) &&
            held_block.item != CELL_ITEM_EMPTY) {
            Vector2 clamped_coords = clamp_block_pos_to_field(
                Vector2Floor(mouse_field_coords), &held_block);
            if (placed_block_space_free(field, clamped_coords, &held_block)) {
                CellCoords cell_coords = get_shape_coords(held_block.shape);
                for (int i = 0; i < cell_coords.len; ++i) {
                    Vector2 cell_pos = Vector2Add(
                        clamped_coords, get_block_cell_coord(&held_block, i));
                    int index = vector_field_index(cell_pos);
                    field[index] = held_block.item;
                }
                blocks_placed++;
                if (blocks_placed == 3) {
                    blocks_placed = 0;
                    for (int i = 0; i < held_blocks_n; ++i) {
                        held_blocks[i] = get_random_block();
                    }
                } else {
                    held_blocks[block_selected] = get_empty_block();
                }
                int points_obtained = clear_field(field, combo);
                if (points_obtained > 0)
                    combo++;
                else
                    combo = 1;
                points += points_obtained;
            }
        }

        char points_buf[32];
        sprintf(points_buf, "Points: %d", points);

        char combo_buf[32];
        sprintf(combo_buf, "Combo: %d", combo);

        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_field(field, board_x, board_y);

        DrawText(points_buf, 20, 20, 30, BLACK);
        DrawText(combo_buf, 20 + 20 + MeasureText(points_buf, 30), 20, 30,
                 BLACK);

        for (int i = 0; i < held_blocks_n; ++i) {
            Vector2 pos = {
                (screenWidth / (held_blocks_n + 1)) * (i + 1),
                board_y + FIELD_CELL_HEIGHT * FIELD_SIZE + 75,
            };
            draw_block(&held_blocks[i], pos, false, 0.5f);
        }

        if (vector_in_field_bounds(mouse_field_coords)) {
            Vector2 clamped_coords = clamp_block_pos_to_field(
                Vector2Floor(mouse_field_coords), &held_block);
            Vector2 translated_pos = translate_board_coords(
                (Vector2){board_x, board_y}, (clamped_coords));
            draw_block(&held_block, translated_pos, true, 1.0f);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}