/*
*     bitprep.h
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Interface for bitprep.c. This header file provides
*              functions avaialable to other files that call 
*              mapping functions to unpack data from and pack data into
*              32 bit codewords.
*/ 

#include <stdlib.h>
#include <stdio.h>
#include "a2methods.h"
#include <pnm.h>

#ifndef BITPREP_H
#define BITPREP_H


/*
*   Summary: packWord is the function called by other implementations.
*            its only functionality is to call the applicable functions to 
*            pack data from the provided array to codewords within the array.
*   Expected Input: array should be an array of Pnm_block containing valid
*                   a, b, c, d, mpb, and mpr data and map should be a valid
*                   mapping function
*   Error Conditions: CRE if any of the provdied values are NULL
*/
void packWord(A2Methods_UArray2 array, A2Methods_mapfun *map);


/*
*   Summary: unpackWord is the function called by other implementations.
*            its only functionality is to call the applicable functions to 
*            unpack data from the codewords in the provided array.
*   Expected Input: array should be an array of Pnm_block containing valid
*                   codewords and map should be a valid mapping function
*   Error Conditions: CRE if any of the provdied values are NULL
*/
void unpackWord(A2Methods_UArray2 array, A2Methods_mapfun *map);




#endif