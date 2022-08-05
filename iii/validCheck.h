/*
 *     validCheck.h
 *     by Matt Zhou(czhou06) and Angus Christie(achris10), 2/14/2022
 *     iii
 *
 *     Interface for validCheck.c; Responsible for checking if a given
 *     sudoku solution is valid.
 */ 
#ifndef VALIDCHECK_H
#define VALIDCHECK_H


#include <stdlib.h>
#include <stdio.h>
#include <pnmrdr.h>
#include <stdbool.h>
#include "uarray2.h"

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
UArray2_T readin(FILE *input, int *mapType, int *denom);

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
bool checkMapdata(int width, int height, int denom, int type);

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
bool valInRange(UArray2_T arr2);

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
bool checkDuplicates(UArray2_T arr2);


#endif