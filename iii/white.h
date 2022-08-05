
#ifndef WHITE_H
#define WHITE_H

#include "bit2.h"
#include <stdbool.h>
#include <seq.h>

typedef struct spot spot;

Bit2_T pbmread(FILE *input);

void addBlackEdges(int i, int j, Bit2_T bit, int b, void *p1);

void cleanBlackEdges(Bit2_T bitmap, Seq_T bstack);

void pbmwrite(FILE *output, Bit2_T bitmap); 


#endif 