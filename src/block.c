#include "block.h"
#include "constants.h"
#include "vector_fns.h"

Vector2 get_block_cell_coord(const Block* block, int i) {
    return Vector2Rotate(block->cell_coords[i], PI * 0.5 * block->rotation);
}

bool placed_block_space_free(Color* field, Vector2 coords, const Block* block) {
    for (int i = 0; i < block->n_cells; ++i) {
        Vector2 cell_pos = Vector2Add(coords, get_block_cell_coord(block, i));
        Color cell_color = field[vector_field_index(cell_pos)];
        if (!ColorIsEqual(cell_color, EMPTY_CELL_COLOR)) {
            return false;
        }
    }
    return true;
}

bool placed_block_fits_in_field(Vector2 coords, const Block* block) {
    for (int i = 0; i < block->n_cells; ++i) {
        Vector2 cell_pos = Vector2Add(coords, get_block_cell_coord(block, i));
        if (!vector_in_field_bounds(cell_pos)) return false;
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


