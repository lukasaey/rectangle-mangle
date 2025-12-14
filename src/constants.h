#if !defined(CONSTANTS_H)
#define CONSTANTS_H

#include <raylib.h>

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

#define EMPTY_CELL_COLOR LIGHTGRAY
#define FIELD_BORDER_COLOR DARKGRAY

typedef enum FieldCellItem {
    CELL_ITEM_EMPTY,
    CELL_ITEM_BLUE,
    CELL_ITEM_GREEN,
    CELL_ITEM_YELLOW,
    CELL_ITEMS_N,  // should be last
} FieldCellItem;
#define CELL_COLORS_N (CELL_ITEMS_N - 1)

typedef enum BlockShape {
    BLOCK_SHAPE_2x2,
    BLOCK_SHAPE_3X3,
    BLOCK_SHAPE_3X2,
    BLOCK_SHAPE_2X3,
    BLOCK_SHAPE_L,
    BLOCK_SHAPE_1X4,
    BLOCK_SHAPE_1X5,
    BLOCK_SHAPES_N,  // should be last
} BlockShape;

typedef struct CellCoords {
    int len;
    Vector2* cell_coords;
} CellCoords;

static const Color field_cell_item_color_lookup[] = {
    [CELL_ITEM_EMPTY] = EMPTY_CELL_COLOR,
    [CELL_ITEM_BLUE] = BLUE,
    [CELL_ITEM_GREEN] = GREEN,
    [CELL_ITEM_YELLOW] = YELLOW,
};

static const CellCoords block_shape_cell_coords_lookup[] = {
    [BLOCK_SHAPE_2x2] = {.len = 4,
                         .cell_coords =
                             (Vector2[]){{0, 0}, {1, 0}, {0, 1}, {1, 1}}},
    [BLOCK_SHAPE_3X3] = {.len = 9,
                         .cell_coords = (Vector2[]){{0, 0},
                                                    {0, 1},
                                                    {0, 2},
                                                    {1, 0},
                                                    {1, 1},
                                                    {1, 2},
                                                    {2, 0},
                                                    {2, 1},
                                                    {2, 2}}},
    [BLOCK_SHAPE_3X2] =
        {.len = 6,
         .cell_coords =
             (Vector2[]){{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}}},
    [BLOCK_SHAPE_2X3] =
        {.len = 6,
         .cell_coords =
             (Vector2[]){{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}}},
    [BLOCK_SHAPE_L] = {.len = 4,
                       .cell_coords =
                           (Vector2[]){{0, 0}, {1, 1}, {0, 1}, {0, -1}}},
    [BLOCK_SHAPE_1X4] = {.len = 4,
                         .cell_coords =
                             (Vector2[]){{0, -1}, {0, 0}, {0, 1}, {0, 2}}},
    [BLOCK_SHAPE_1X5] = {.len = 5,
                         .cell_coords =
                             (Vector2[]){
                                 {0, -2}, {0, -1}, {0, 0}, {0, 1}, {0, 2}}},
};

#endif  // CONSTANTS_H
