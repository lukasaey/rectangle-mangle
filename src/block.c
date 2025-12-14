#include "block.h"

#include <stdlib.h>
#include <assert.h>

#include "constants.h"
#include "vector_fns.h"

Vector2 get_block_cell_coord(const Block* block, int i) {
    CellCoords coords = get_shape_coords(block->shape);
    return Vector2Rotate(coords.cell_coords[i], PI * 0.5 * block->rotation);
}

bool placed_block_space_free(FieldCellItem* field, Vector2 coords,
                             const Block* block) {
    CellCoords cell_coords = get_shape_coords(block->shape);
    for (int i = 0; i < cell_coords.len; ++i) {
        Vector2 cell_pos = Vector2Add(coords, get_block_cell_coord(block, i));
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
        Vector2 cell_pos = Vector2Add(coords, get_block_cell_coord(block, i));
        if (!vector_in_field_bounds(cell_pos)) return false;
    }
    return true;
}

Vector2 clamp_block_pos_to_field(Vector2 coords, const Block* block) {
    CellCoords cell_coords = get_shape_coords(block->shape);
    for (int i = 0; i < cell_coords.len; ++i) {
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

inline Color get_field_cell_color(FieldCellItem item) { 
    assert(item >= 0 && item < CELL_ITEMS_N);
    return field_cell_item_color_lookup[item];    
}

CellCoords get_shape_coords(BlockShape shape) { 
    assert(shape >= 0 && shape < BLOCK_SHAPES_N);
    return block_shape_cell_coords_lookup[shape];
 }
