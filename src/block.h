#if !defined(BLOCK_H)
#define BLOCK_H

#include <raylib.h>
#include <raymath.h>

#include "constants.h"

typedef struct Block {
    FieldCellItem item;
    BlockShape shape;
    int rotation;  // how many right angle rotations to the right
} Block;

typedef enum BlockAlignmentType {
    BLOCK_ALIGNMENT_TYPE_MIDDLE,
    BLOCK_ALIGNMENT_TYPE_CORNER,
    BLOCK_ALIGNMENT_TYPE_EDGE,
} BlockAlignmentType;

Block make_block(FieldCellItem item, BlockShape shape, int rotation);
Block get_random_block();
Block get_empty_block();

Vector2 get_block_cell_coord(const Block* block, int i);
bool placed_block_space_free(FieldCellItem* field, Vector2 coords, const Block* block);
bool placed_block_fits_in_field(Vector2 coords, const Block* block);
BlockAlignmentType get_block_alignment(const Block* block);
Vector2 clamp_block_pos_to_field(Vector2 coords, const Block* block);

Color get_field_cell_color(FieldCellItem item);
CellCoords get_shape_coords(BlockShape shape);

#endif  // BLOCK_H
