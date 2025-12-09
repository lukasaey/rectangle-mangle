#if !defined(BLOCK_H)
#define BLOCK_H

#include <raylib.h>
#include <raymath.h>

#include "constants.h"

typedef struct Block {
    FieldCellItem item;
    int n_cells;
    int rotation;  // how many right angle rotations to the right
    const Vector2* cell_coords;
} Block;

Vector2 get_block_cell_coord(const Block* block, int i);
bool placed_block_space_free(FieldCellItem* field, Vector2 coords, const Block* block);
bool placed_block_fits_in_field(Vector2 coords, const Block* block);
Vector2 clamp_block_pos_to_field(Vector2 coords, const Block* block);

#define BLOCK_2X2(itm, rot) \
    ((Block){               \
        .item = (itm),     \
        .n_cells = 4,       \
        .rotation = (rot),  \
        .cell_coords = (const Vector2[]){{0, 0}, {1, 0}, {0, 1}, {1, 1}}})

#define BLOCK_3X3(itm, rot)                           \
    ((Block){.item = (itm),                          \
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

#define BLOCK_3X2(itm, rot)                    \
    ((Block){.item = (itm),                   \
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

#define BLOCK_2X3(itm, rot)                    \
    ((Block){.item = (itm),                   \
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

#define BLOCK_L(itm, rot)  \
    ((Block){              \
        .item = (itm),    \
        .rotation = (rot), \
        .n_cells = 4,      \
        .cell_coords = (const Vector2[]){{0, 0}, {1, 1}, {0, 1}, {0, -1}}})

#define BLOCK_1x4(itm, rot) \
    ((Block){               \
        .item = (itm),     \
        .rotation = (rot),  \
        .n_cells = 4,       \
        .cell_coords = (const Vector2[]){{0, -1}, {0, 0}, {0, 1}, {0, 2}}})

#define BLOCK_1x5(itm, rot)     \
    ((Block){.item = (itm),    \
             .rotation = (rot), \
             .n_cells = 5,      \
             .cell_coords =     \
                 (const Vector2[]){{0, -2}, {0, -1}, {0, 0}, {0, 1}, {0, 2}}})

#endif  // BLOCK_H
