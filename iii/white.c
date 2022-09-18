#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pnmrdr.h>
#include <assert.h>
#include <seq.h>
#include "white.h"

#define BLACK 1
#define WHITE 0

struct spot
{
    int x;
    int y;
};


Bit2_T pbmread(FILE *input)
{
    Pnmrdr_T reader = Pnmrdr_new(input);
    Pnmrdr_mapdata mapdata = Pnmrdr_data(reader);
    assert(mapdata.width > 0 && mapdata.height > 0 && mapdata.type == 1);
    Bit2_T og = Bit2_new(mapdata.width, mapdata.height);
    Bit2_map_row_major(og, populate, reader);
    Pnmrdr_free(&reader);
    return og;
}

void populate(int i, int j, Bit2_T bit, int b, void *p1)
{
    Pnmrdr_T popReader = (Pnmrdr_T) p1;
    Bit2_put(bit, i, j, Pnmrdr_get(popReader));
    (void) b;
    (void) p1;
}

void addBlackEdges(int i, int j, Bit2_T bit, int b, void *p1)
{
    Seq_T *stk = (Seq_T*) p1;
    if (b == BLACK && isEdge(i, j, Bit2_height(bit), Bit2_width(bit))) {
        spot *sp = createSpot(i, j);
        Seq_addhi(*stk, sp);
    }
    
}

bool isEdge(int i, int j, int height, int width)
{
    if (i == 0 || i == width - 1 || j  == 0 || j == height - 1) {
        return true;
    }
    
    return false;
}


void cleanBlackEdges(Bit2_T bitmap, Seq_T bstack)
{
    assert(bitmap != NULL && bstack != NULL);
    while (Seq_length(bstack) > 0) {
        spot* targetSpot = Seq_remhi(bstack);
        int xpos = targetSpot->x;
        int ypos = targetSpot->y;
        free(targetSpot);
        int color = Bit2_get(bitmap, xpos, ypos);

        if (color == BLACK) {
            Bit2_put(bitmap, xpos, ypos, WHITE);
            addNeighbors(bitmap, &bstack, xpos, ypos);
        }
       
    }
}


void addNeighbors(Bit2_T bitmap, Seq_T *stk, int i, int j)
{
    assert(stk != NULL && *stk != NULL && bitmap != NULL);

    int width = Bit2_width(bitmap);
    int height = Bit2_height(bitmap);

    if (i - 1 > 0) {
        spot *sp = createSpot(i - 1, j);
        Seq_addhi(*stk, sp);
    }

    if (i + 1 < width) {

        spot *sp = createSpot(i + 1, j);
        Seq_addhi(*stk, sp);
    }
    
    if (j - 1 > 0) {
        spot *sp = createSpot(i, j - 1);
        Seq_addhi(*stk, sp);
    }
    
    if (j + 1 < height) {
        spot *sp = createSpot(i, j + 1);
        Seq_addhi(*stk, sp);
    }

}

spot *createSpot(int i, int j)
{
    spot *sp = malloc(sizeof(*sp));
    assert(sp != NULL);
    sp->x = i;
    sp->y = j;

    return sp;
}

void pbmwrite(FILE *output, Bit2_T bitmap)
{
    assert(bitmap != NULL);
    fprintf(output, "%s", "P1\n");
    fprintf(output, "%d %d\n", Bit2_width(bitmap), Bit2_height(bitmap));
    Bit2_map_row_major(bitmap, write, output);
}

void write(int i, int j, Bit2_T map, int b, void *p1)
{
    fprintf(p1, "%d", b);

    if (i == Bit2_width(map) - 1) {
        fprintf(p1, "\n");
    }

    (void) j;
}
