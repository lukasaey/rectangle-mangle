#include "block.h"

#include <assert.h>
#include <stdlib.h>

#include "constants.h"
#include "vector_fns.h"

static const Color field_cell_item_color_lookup[] = {
    [CELL_ITEM_EMPTY] = EMPTY_CELL_COLOR,
    [CELL_ITEM_BLUE] = BLUE,
    [CELL_ITEM_GREEN] = GREEN,
    [CELL_ITEM_YELLOW] = YELLOW,
};

static const CellCoords block_shape_cell_coords_lookup[] = {
    [BLOCK_SHAPE_2x2] = {.len = 4,
                         .cell_coords =
                             (Vector2[]){{-1, -1}, {0, 0}, {0, -1}, {-1, 0}}},
    [BLOCK_SHAPE_3X3] = {.len = 9,
                         .cell_coords = (Vector2[]){{-1.5, -1.5},
                                                    {-1.5, -0.5},
                                                    {-1.5, 0.5},
                                                    {-0.5, -1.5},
                                                    {-0.5, -0.5},
                                                    {-0.5, 0.5},
                                                    {0.5, -1.5},
                                                    {0.5, -0.5},
                                                    {0.5, 0.5}}},
    [BLOCK_SHAPE_3X2] = {.len = 6,
                         .cell_coords = (Vector2[]){{-0.5, 0},
                                                    {-1.5, 0},
                                                    {0.5, 0},
                                                    {-0.5, -1},
                                                    {0.5, -1},
                                                    {-1.5, -1}}},
    [BLOCK_SHAPE_L] = {.len = 4,
                       .cell_coords =
                       // ###
                       // #
                           (Vector2[]){
                               {-0.5, -0.5}, {0.5, -0.5}, {-1.5, -0.5}, {-1.5, 0.5}}},
    [BLOCK_SHAPE_1X4] = {.len = 4,
                         .cell_coords =
                             (Vector2[]){
                                 {-0.5, 0}, {-0.5, -1}, {-0.5, -2}, {-0.5, 1}}},
    [BLOCK_SHAPE_1X5] = {.len = 5,
                         .cell_coords =
                             (Vector2[]){
                                 {-0.5, -2.5}, {-0.5, -1.5}, {-0.5, -0.5}, {-0.5, 0.5}, {-0.5, 1.5}}},
};

Vector2 get_block_cell_coord(const Block* block, int i, int snap_direction) {
    CellCoords coords = get_shape_coords(block->shape);
    Vector2 coord = coords.cell_coords[i];
    Vector2 offset = Vector2Add(coord, (Vector2){0.5, 0.5});
    Vector2 rot = Vector2Rotate(offset, PI * 0.5 * block->rotation);
    return Vector2Subtract(rot, (Vector2){0.5, 0.5});
}

bool placed_block_space_free(FieldCellItem* field, Vector2 coords,
                             const Block* block) {
    CellCoords cell_coords = get_shape_coords(block->shape);
    for (int i = 0; i < cell_coords.len; ++i) {
        Vector2 cell_pos =
            Vector2Add(coords, get_block_cell_coord(block, i, 0));
        Color cell_color =
            field_cell_item_color_lookup[field[vector_field_index(cell_pos)]];
        if (!ColorIsEqual(cell_color, EMPTY_CELL_COLOR)) {
            return false;
        }
    }
    return true;
}

bool placed_block_fits_in_field(Vector2 coords, const Block* block) {
    CellCoords cell_coords = get_shape_coords(block->shape);
    for (int i = 0; i < cell_coords.len; ++i) {
        Vector2 cell_pos =
            Vector2Add(coords, get_block_cell_coord(block, i, 0));
        if (!vector_in_field_bounds(cell_pos)) return false;
    }
    return true;
}

BlockAlignmentType get_block_alignment(const Block* block) {
    switch (block->shape) {
        case BLOCK_SHAPE_2x2:
            return BLOCK_ALIGNMENT_TYPE_CORNER;
        case BLOCK_SHAPE_3X3:
            return BLOCK_ALIGNMENT_TYPE_MIDDLE;
        case BLOCK_SHAPE_3X2:
            return BLOCK_ALIGNMENT_TYPE_EDGE;
        case BLOCK_SHAPE_L:
            return BLOCK_ALIGNMENT_TYPE_MIDDLE;
        case BLOCK_SHAPE_1X4:
            return BLOCK_ALIGNMENT_TYPE_EDGE;
        case BLOCK_SHAPE_1X5:
            return BLOCK_ALIGNMENT_TYPE_MIDDLE;
    }
}

Vector2 clamp_block_pos_to_field(Vector2 coords, const Block* block) {
    CellCoords cell_coords = get_shape_coords(block->shape);
    for (int i = 0; i < cell_coords.len; ++i) {
        Vector2 cell_pos =
            Vector2Add(coords, get_block_cell_coord(block, i, 0));
        if (cell_pos.x < 0)
            coords.x += -cell_pos.x;
        else if (cell_pos.x > (FIELD_SIZE - 1))
            coords.x -= cell_pos.x - (FIELD_SIZE - 1);
        if (cell_pos.y < 0)
            coords.y += -cell_pos.y;
        else if (cell_pos.y > (FIELD_SIZE - 1))
            coords.y -= cell_pos.y - (FIELD_SIZE - 1);
    }
    return coords;
}

Block make_block(FieldCellItem item, BlockShape shape, int rotation) {
    return (Block){
        .item = item,
        .shape = shape,
        .rotation = rotation,
    };
}

Block get_random_block() {
    FieldCellItem item = (rand() % CELL_COLORS_N) + 1;
    BlockShape shape = rand() % BLOCK_SHAPES_N;
    int rotation = rand() % 4;
    return (Block){
        .item = item,
        .shape = shape,
        .rotation = rotation,
    };
}

Block get_empty_block() {
    return (Block){
        .item = CELL_ITEM_EMPTY,
        .rotation = 0,
        .shape = 0,
    };
}

inline Color get_field_cell_color(FieldCellItem item) {
    assert(item >= 0 && item < CELL_ITEMS_N);
    return field_cell_item_color_lookup[item];
}

CellCoords get_shape_coords(BlockShape shape) {
    assert(shape >= 0 && shape < BLOCK_SHAPES_N);
    return block_shape_cell_coords_lookup[shape];
}
