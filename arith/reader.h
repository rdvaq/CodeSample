/*
*     reader.h
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Interface of reader.c. Contains functions that's responsible
*     for printing the compressed codewords to stdout and reading in supplied
*     compresssed images. 
*/ 

#ifndef READER_H
#define READER_H

#include <stdlib.h>
#include <stdio.h>

#include "a2methods.h"


/*   writeout
 *   Summary: Function responsible for printing the compressed image to stdout;
 *   Prints out header first fllowed by compressed codewords in a row-major 
 *   order
 * 
 *   Expected Input: width and height of the original image, a UArray2 that
 *   contains the compressed codewords, and a specified mapping function(should
 *   be row-maj)
 * 
 *   Error Conditions: CRE is thrown in map or array is NULL
 */
void writeout(int width, int height, A2Methods_UArray2 array, 
                                                        A2Methods_mapfun *map);


/*   readin
 *   Summary: Function responsible for reading in the compressed image. Returns
 *   a UArray2 that contains Pnm_block structs. Each block struct should be 
 *   populated with a codeword. Also returns the width and height of the 
 *   uncompressed image.
 * 
 *   Expected Input: File stream of the compressed image, a A2methods used for
 *   for initialiing UArray2s, and unsigned pointers to width and height so 
 *   that the function can also 'return' these values. 
 * 
 *   Error Conditions: CRE is thrown if methods, width and height are NULL;
 *  
 */
A2Methods_UArray2 readin(FILE *input, A2Methods_T methods, unsigned *width, 
                                                            unsigned *height);





#endif