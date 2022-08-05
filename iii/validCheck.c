/*
 *     validCheck.c
 *     by Matt Zhou(czhou06) and Angus Christie(achris10), 2/14/2022
 *     iii
 * 
 *     Implementation for validCheck.h; Responsible for checking if a given
 *     sudoku solution is valid by checking if there are duplicates in the 
 *     rows, columns, and subsquares of the solution. Also checks if the input 
 *     is a valid pgm and if all the int values in the input is within 
 *     expected range.     
 */ 

#include "validCheck.h"
#include <assert.h>

const int DIM = 9;

/*
 * function: populate
 *
 * purpose: an apply function designed to populate a given UArray2_T based 
 * on the contents of a file; called by readin with map_row_major
 *
 * parameters: int i and j specifying the position in the UArray2_T, UArray2_T 
 * to be populated, p1 points to the element in the UArray2_T at index i, j, p2 
 * closure argument that is a pointer to a pnmrdr
 *
 * returns: none
 *
 * exceptions: A CRE is raised if arr2, *p1, or *p2 is NULL
 */
void populate(int i, int j, UArray2_T arr2, void *p1, void *p2)
{
    assert(arr2 != NULL && p1 != NULL && p2 != NULL);
    Pnmrdr_T popReader = (Pnmrdr_T) p2;
    unsigned *at = UArray2_at(arr2, i, j);
    *at = Pnmrdr_get(popReader);

    (void) p1;
}

/*
 * function: readin
 *
 * purpose: takes in a input stream and populates a UArray2_T based on the   
 * contents of the file
 *
 * parameters: Input file stream; int representing the map type of the input 
 * file; int denominator represents the maxval of the given pgm  
 *
 * returns: a UArray2_T 2d array
 *
 * exceptions: A CRE is raised if FILE *input, int *maptype, or int *demon 
 * is NULL
 */
UArray2_T readin(FILE *input, int *mapType, int *denom)
{
    assert(input != NULL && mapType != NULL && denom != NULL);
    Pnmrdr_T reader = Pnmrdr_new(input);
    Pnmrdr_mapdata data = Pnmrdr_data(reader);
    *mapType = data.type;
    *denom = data.denominator;
    UArray2_T board = UArray2_new(data.width, data.height, sizeof(unsigned));
    UArray2_map_row_major(board, populate, reader);
    Pnmrdr_free(&reader);

    return board;
}

/*
 * function: checkMapdata
 *
 * purpose: checks to make sure the dimensions, type, and maxval of the pgm 
 * file are correct
 *
 * parameters: width of pgm, height of pgm, maxval of pgm, type of pnm 
 *
 * returns: boolean that indicates whether or not the values are all correct
 *
 * exceptions: none
 */
bool checkMapdata(int width, int height, int denom, int type)
{
    if (type != 2 || width != DIM || height != DIM || denom != DIM) {
        return false;
    }

    return true;
}

/*
 * function: checkVal
 *
 * purpose: apply function designed to check every single value in a given 
 * UArray2_T and make sure that they are in range; called by valInRange with
 * map_row_major
 *
 * parameters: int i and j specifying the position in the UArray2_T, 
 * UArray2_T to be populated, p1 points to the element in the UArray2_T at 
 * index i, j, p2 closure argument that is a pointer to a boolean stating 
 * if a value is valid or not   
 *
 * returns: none
 *
 * exceptions: A CRE is raised if arr2, p1, or p2 null
 */
void checkVal(int i, int j, UArray2_T arr2, void *p1, void *p2)
{
    assert(arr2 != NULL && p1 != NULL && p2 != NULL);
    bool *valid = (bool *) p2;
    unsigned *at = (unsigned *) p1;
    if (*at < 1 || *at > 9) {
        *valid = false;
    }
    (void) i;
    (void) j;
    (void) arr2;
}

/*
 * function: valInRange
 *
 * purpose: checks if all of the values in the given UArray2_T are in a 
 * the range of 1-9
 *
 * parameters: A UArray2_T 2D array  
 *
 * returns: boolean that indicates whether or not all of the values are valid
 *
 * exceptions: A CRE is raised if arr2 null
 */
bool valInRange(UArray2_T arr2)
{
    assert(arr2 != NULL);
    bool validVals = true;
    UArray2_map_row_major(arr2, checkVal, &validVals);

    return validVals;
}

/*
 * function: rowApply 
 *
 * purpose: apply function designed to check if a given row in an array 
 * contains duplicate values; Called with UArray2_map_row_major;
 * A check array of size DIM + 1 is passed into the closure, we use the check 
 * array to see if there are any duplicates in a given row. The checkArr[DIM] 
 * acts as a sentinel value for dups, if dups exits, then the value is changed 
 * from the known original value.
 *
 * parameters: int i and j specifying the position in the 2D uarray, 
 * an 2D uarray to be checked, p1 points to the element in the target uarray2 
 * at index i, j, p2 is the closure argument where a check array is passed in 
 *
 * returns: none
 *
 * exceptions: A CRE is raised if arr2, p1, or p2 is null
 */
void rowApply(int i, int j, UArray2_T arr2, void *p1, void *p2)
{
    assert(arr2 != NULL && p1 != NULL && p2 != NULL);
    int *checkArr = (int *)p2;
    int value = *((int *)p1);
    /*increase specific index of the checkArr based on the values in the row, 
    i.e. if value is 5, then we increase index 4 of the checkArr*/
    checkArr[value - 1]++;

    if(i == DIM - 1) {
        for (int index = 0; index < DIM; index++) {
            /* if there are no duplicates, then each value in the index of the
            checkArr should increase by 1 with each pass through a row*/
            if (checkArr[index] != j + 1) {
                /* change sentinel value to negative*/
                checkArr[DIM]--;
            }         
        }     
    } 
    (void) arr2;
}

/*
 * function: colApply 
 *
 * purpose: apply function designed to check if a given column in an array 
 * contains duplicate values; Called by UArray2_map_col_major; employs
 * the same principle as rowApply to check for dups
 *
 * parameters: int i and j specifying the position in the 2D uarray, 
 * an 2D uarray to be checked, p1 points to the element in the target uarray2 at
 * index i, j, p2 is the closure argument where a check array is passed in 
 *
 * returns: none
 *
 * exceptions: A CRE is raised if arr2, p1, or p2 is null
 */
void colApply(int i, int j, UArray2_T arr2, void *p1, void *p2)
{
    assert(arr2 != NULL && p1 != NULL && p2 != NULL);
    int *checkArr = (int*)p2;
    int value = *((int *)p1);

    checkArr[value - 1]++;

    if(j == DIM - 1) {
        for (int index = 0; index < DIM; index++) {
            if (checkArr[index] != i + 1) {
                checkArr[DIM]--;
            }         
        }     
    } 
    (void) arr2;
}

/*
 * function: populateBySquare
 *
 * purpose: fills an int array in the order of 9 3x3 squares so that each 
 * square can be checked for duplicates; called by UArray2_row_major
 *
 * parameters: int i and j specifying the position in the UArray2_T, 
 * UArray2_T arr2 to get every value from, p1 points to the element in the 
 * UArray2_T at index i, j, p2 closure argument that is a pointer to a int 
 * array that stores all the values within a subquare consequtively.
 *
 * returns: none
 *
 * exceptions: A CRE is raised if arr2, p1, or p2 null
 */
void populateBySquare(int i, int j, UArray2_T arr2, void *p1, void *p2)
{
    assert(arr2 != NULL && p1 != NULL && p2 != NULL);
    int *wholeArr = (int *)p2;
    int value = *((int *)p1);
    /*complicated eq to group the squares together in a 1D array*/
    int newIndex = (j / 3) * 27 + 3 * (j % 3) + (i / 3) * 9 + i % 3;

    wholeArr[newIndex] = value;
    (void)arr2;
}

/*
 * function: squareCheck
 *
 * purpose: checks if there are any duplicates in any of the 3x3 squares 
 * in the sudoku
 *
 * parameters: A UArray2_T 2D array  
 *
 * returns: boolean that says whether or not there are any duplicates in 
 * any of the 3x3 squares
 *
 * exceptions: A CRE is raised if arr2 null
 */
bool squareCheck(UArray2_T arr2)
{
    assert(arr2 != NULL);
    bool valid = true;
    /* all values in checkerArr initialized to 0*/
    int *squareArr = (int*)calloc(DIM * DIM, sizeof(int));
    int *checkerArr = (int*)calloc(DIM, sizeof(int));
    UArray2_map_row_major(arr2, populateBySquare, squareArr);

    int elemCount = 0;
    for (int i = 0; i < DIM * DIM; i++) {
        int checkerIndex = squareArr[i] - 1;
        checkerArr[checkerIndex]++;
        elemCount++;
        /* when elemCount is 9, then we know that we have looked through all
        elements of a given subsquare*/
        if (elemCount == 9) {
            for (int j = 0; j < DIM; j++) {
                /*same principle here as rowApply*/
                if (checkerArr[j] != 1) {
                    valid = false;
                }
                /*reset checkerArr for the next subsquare*/
                checkerArr[j] = 0; 
            }  
            elemCount = 0;
        }          
    } 
    free(squareArr);
    free(checkerArr);
    return valid;   
}

/*
 * function: rowAndColCheck
 *
 * purpose: checks if there are any duplicates in each row and column; 
 *
 * parameters: A UArray2_T 2D array  
 *
 * returns: boolean if there are any duplicates in any row or column
 *
 * exceptions: A CRE is raised if arr2 null
 */
bool rowAndColCheck(UArray2_T arr2)
{
    assert(arr2 != NULL);
    bool valid = true;
    /* all values in the arrays is initialized to 0*/
    int *rarr = (int*)calloc(DIM + 1, sizeof(int));
    int *carr = (int*)calloc(DIM + 1, sizeof(int));
    
    UArray2_map_row_major(arr2, rowApply, rarr);
    UArray2_map_col_major(arr2, colApply, carr);

    /*we check the sentinel values here*/
    if (rarr[DIM] != 0 || carr[DIM] != 0) {
        valid =  false;
    }

    free(rarr);
    free(carr);
    return valid;
}

/*
 * function: checkDuplicates
 *
 * purpose: checks if there are any duplicate values in columns, rows, and
 * squares of the given UArray2_T that represents a sudoku
 *
 * parameters: A UArray2_T 2D array  
 *
 * returns: boolean that indicates whether or not duplicates are present in 
 * the solution
 *
 * exceptions: A CRE is raised if arr2 null
 */
bool checkDuplicates(UArray2_T arr2)
{   
    assert(arr2 != NULL);
    bool goodSudoku = true;
    bool validRandC = rowAndColCheck(arr2);
    bool validS = squareCheck(arr2);

    goodSudoku &= validS && validRandC;
    
    return goodSudoku;
}