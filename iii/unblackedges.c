
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include "bit2.h"
#include "white.h"


int main(int argc, char* argv[])
{
    assert(argc == 1 || argc == 2);

    Bit2_T map;

    if (argc == 1) {
        map = pbmread(stdin);
    }
    else {
        FILE *fp = fopen(argv[1], "r");
        assert(fp != NULL);
        map = pbmread(fp);
        fclose(fp);
    }

    Seq_T bstack = Seq_new(0);

    Bit2_map_row_major(map, addBlackEdges, &bstack);

    cleanBlackEdges(map, bstack);

    pbmwrite(stdout, map);

    Seq_free(&bstack);
    Bit2_free(&map);

    return EXIT_SUCCESS;
}



