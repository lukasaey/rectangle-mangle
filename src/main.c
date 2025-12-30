#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "block.h"
#include "constants.h"
#include "raylib.h"
#include "raymath.h"
#include "vector_fns.h"

typedef struct GameState {
    FieldCellItem field[FIELD_SIZE * FIELD_SIZE];
    int points;
    int combo;
    int blocks_placed;
    int block_selected;
    Block held_blocks[3];
    bool cleared_in_turn;
} GameState;

GameState make_gamestate() {
    GameState state = {
        .points = 0,
        .combo = 1,
        .blocks_placed = 0,
        .block_selected = 0,
        .cleared_in_turn = false,
    };
    for (int i = 0; i < FIELD_SIZE * FIELD_SIZE; ++i) {
        state.field[i] = CELL_ITEM_EMPTY;
    }

    for (int i = 0; i < HELD_BLOCKS_N; ++i) {
        state.held_blocks[i] = get_random_block();
    }
    return state;
}

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

Vector2 snap_mouse_coords(Vector2 mouse_field_coords, const Block* block) {
    Vector2 projected_mouse_coords = Vector2Scale(
        Vector2SubtractValue(mouse_field_coords, FIELD_SIZE / 2.0f),
        2.0f / FIELD_SIZE);

    BlockAlignmentType alignment_type = get_block_alignment(block);
    Vector2 offset = Vector2Zero();
    switch (alignment_type) {
        case BLOCK_ALIGNMENT_TYPE_MIDDLE:
            offset = (Vector2){0.5, 0.5};
            break;
        case BLOCK_ALIGNMENT_TYPE_CORNER:
            // nothing
            break;
        case BLOCK_ALIGNMENT_TYPE_EDGE:
            offset = block->rotation % 2 == 0 ? (Vector2){0.5, 0.}
                                              : (Vector2){0., 0.5};
            break;
    }
    if (projected_mouse_coords.x < 0) offset.x = -offset.x;
    if (projected_mouse_coords.y < 0) offset.y = -offset.y;

    Vector2 rounded =
        Vector2Round(Vector2Add(mouse_field_coords, projected_mouse_coords));
    rounded = Vector2Add(rounded, offset);
    return rounded;
}

void handle_block_placement(GameState* state, Vector2 mouse_coords) {
    const Block* held_block = &state->held_blocks[state->block_selected];
    state->blocks_placed++;

    CellCoords cell_coords = get_shape_coords(held_block->shape);
    // placing the block into the field
    for (int i = 0; i < cell_coords.len; ++i) {
        Vector2 cell_pos =
            Vector2Add(mouse_coords, get_block_cell_coord(held_block, i));
        int index = vector_field_index(cell_pos);
        state->field[index] = held_block->item;
    }

    // field clearing and adding points
    int points_obtained = clear_field(state->field, state->combo);

    bool increase_combo = false;
    if (points_obtained > 0) {
        state->cleared_in_turn = true;
        increase_combo = true;
    }

    if (increase_combo) {
        state->combo += 1;
    } else if (!state->cleared_in_turn && state->blocks_placed == 3) {
        state->combo = 1;
    }

    // held blocks handling
    if (state->blocks_placed == 3) {
        state->blocks_placed = 0;
        state->cleared_in_turn = false;
        for (int i = 0; i < HELD_BLOCKS_N; ++i) {
            state->held_blocks[i] = get_random_block();
        }
    } else {
        state->held_blocks[state->block_selected] = get_empty_block();
    }

    state->points += points_obtained;
}

bool get_fuzzy_block_placement(const GameState* state, Vector2 location,
                               Vector2 grid_clamped_location,
                               Vector2* fuzzy_location_out) {
    const Block* held_block = &state->held_blocks[state->block_selected];

    int starting_dx = location.x < 4.0f ? -1 : 1;
    int ddx = location.x < 4.0f ? 1 : -1;

    int starting_dy = location.y < 4.0f ? -1 : 1;
    int ddy = location.y < 4.0f ? 1 : -1;

    for (int dx = starting_dx; abs(dx - starting_dx) < 3; dx += ddx) {
        for (int dy = starting_dy; abs(dy - starting_dy) < 3; dy += ddy) {
            Vector2 new_location =
                Vector2Add(grid_clamped_location, (Vector2){dx, dy});
            if (vector_in_field_bounds(new_location) &&
                placed_block_space_free(state->field, new_location,
                                        held_block)) {
                *fuzzy_location_out = new_location;
                return true;
            }
        }
    }
    return false;
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "rectangle mangle");

    SetTargetFPS(60);

    GameState state = make_gamestate();

    int board_x = 150;
    int board_y = 65;

    while (!WindowShouldClose()) {
        Vector2 mouse_field_coords = project_mouse_on_board(
            (Vector2){board_x, board_y}, GetMousePosition());

        float wheel = GetMouseWheelMove();
        int block_delta = (int)wheel;
        state.block_selected =
            wrapping_mod((state.block_selected + block_delta), HELD_BLOCKS_N);

        // skip if an empty block is selected
        if (block_delta == 0) block_delta = 1;
        while (state.held_blocks[state.block_selected].item ==
               CELL_ITEM_EMPTY) {
            state.block_selected =
                wrapping_mod(state.block_selected + block_delta, HELD_BLOCKS_N);
        }

        if (IsKeyPressed(KEY_R)) {
            state = make_gamestate();
        }

        Block held_block = state.held_blocks[state.block_selected];

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            vector_in_field_bounds(mouse_field_coords) &&
            held_block.item != CELL_ITEM_EMPTY) {
            Vector2 clamped_mouse_coords = clamp_block_pos_to_field(
                snap_mouse_coords(mouse_field_coords, &held_block),
                &held_block);
            Vector2 fuzzy_placement;
            if (placed_block_space_free(state.field, clamped_mouse_coords,
                                        &held_block)) {
                handle_block_placement(&state, clamped_mouse_coords);
            } else if (get_fuzzy_block_placement(&state, mouse_field_coords,
                                                 clamped_mouse_coords,
                                                 &fuzzy_placement)) {
                handle_block_placement(&state, fuzzy_placement);
            }
        }

        char points_buf[32];
        sprintf(points_buf, "Points: %d", state.points);

        char combo_buf[32];
        sprintf(combo_buf, "Combo: %d", state.combo);

        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_field(state.field, board_x, board_y);

        DrawText(points_buf, 20, 20, 30, BLACK);
        DrawText(combo_buf, 20 + 20 + MeasureText(points_buf, 30), 20, 30,
                 BLACK);

        // small block previews on the bottom
        for (int i = 0; i < HELD_BLOCKS_N; ++i) {
            Vector2 pos = {
                (screenWidth / (HELD_BLOCKS_N + 1)) * (i + 1),
                board_y + FIELD_CELL_HEIGHT * FIELD_SIZE + 100,
            };
            draw_block(&state.held_blocks[i], pos, false, 0.5f);
            if (state.block_selected == i) {
                int size = 175;
                Rectangle rec = {
                    .x = pos.x - size / 2,
                    .y = pos.y - size / 2,
                    .height = size,
                    .width = size,
                };
                DrawRectangleLinesEx(rec, 6.f, DARKPURPLE);
            }
        }

        // held block
        if (vector_in_field_bounds(mouse_field_coords)) {
            Vector2 clamped_coords_no_snap =
                clamp_block_pos_to_field(mouse_field_coords, &held_block);

            Vector2 clamped_coords_snap = clamp_block_pos_to_field(
                snap_mouse_coords(mouse_field_coords, &held_block),
                &held_block);
            Vector2 fuzzy_coords;
            // the transparent preview of where the block will end up
            if (placed_block_space_free(state.field, clamped_coords_snap,
                                        &held_block)) {
                draw_block(&held_block,
                           translate_board_coords((Vector2){board_x, board_y},
                                                  (clamped_coords_snap)),
                           true, 1.0f);
            } else if (get_fuzzy_block_placement(&state, mouse_field_coords,
                                                 clamped_coords_snap,
                                                 &fuzzy_coords)) {
                draw_block(&held_block,
                           translate_board_coords((Vector2){board_x, board_y},
                                                  (fuzzy_coords)),
                           true, 1.0f);
            }

            // the non transparent block the player is holding with their mouse
            draw_block(&held_block,
                       translate_board_coords((Vector2){board_x, board_y},
                                              (clamped_coords_no_snap)),
                       false, 1.0f);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}