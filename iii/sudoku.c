/*
 *     sudoku.c
 *     by Matt Zhou(czhou06) and Angus Christie(achris10), 2/14/2022
 *     iii
 *      
 *     Main file for the check-sudoku-solution program. The program takes input
 *     from stdin or an input file and check if the supplied solutions are 
 *     valid. If the solution is valid, the program exits with code 0, else,
 *     it exits with code 1.
 *     
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "uarray2.h"
#include "validCheck.h"

/*
 *  If 0 args, run program on stdin;
 *  If 1 args, run program on file input;
 *  A CRE is thrown if more than 1 argument is supplied or if the provided 
 *  input file can not be opened
 */
int main(int argc, char *argv[])
{
    assert(argc == 1 || argc == 2);
    UArray2_T board;
    bool validSolution = true;
    int mapType;
    int denom;
    if (argc == 1) {
        board = readin(stdin, &mapType, &denom);
    } else if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        assert(fp != NULL);
        board = readin(fp, &mapType, &denom);
        fclose(fp);
    }
    /*preform 3 checks*/
    bool validPGM = checkMapdata(UArray2_width(board), UArray2_height(board),
                                                               denom, mapType);

    bool validVals = valInRange(board);

    validSolution &= validPGM && validVals;
    /*checkDuplicates assume valid pgm and valid vals, thus it is necessary 
    to check if the first two tests return true*/
    if (validSolution) {
        bool noDups = checkDuplicates(board);
        validSolution = noDups;
    }

    UArray2_free(&board);

    if (validSolution) {
        exit(0);
    } else {
        exit(1);
    }
}
