/*
*     DCT.h
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Interface for DCT.c This header file defines functions
*     called by compress40.c. The functions here are responsible for 
*     both converting the CV values in the given uncompressed image to 
*     mean pb, mean pr, a, b, c d values and the inverse conversion. 
*     DCT.h is included in all files that use the Pnm_block struct defined here 
*/


#include <stdio.h>
#include <stdlib.h>
#include <pnm.h>
#include "a2methods.h"
#include "CVRGB.h"

#ifndef DCT_H
#define DCT_H

/* Pnm_block
 * Pixel struct that stores mean pb, mean pr, and abcd values calculted from 
 * DCT transformations. The struct also contains the codeword 
 * obtained from packing the above values in the struct. 
 * Note: the struct here contains the information calculated from the elements 
 * stored in a 2x2 block of the original image
 */
typedef struct Pnm_block {
    float mpb, mpr, a, b, c, d;
    uint32_t codeword;
} *Pnm_block;


/*
*   Summary: DCTTransform is the function the client should call when 
*            transforming from CV pixel format to DCT values. Note that the 
*            mean of the chroma Pb and Pr values are also calculated per block
*            along the DCT calculations.
*   Expected Input: A UArray2 that's populated with CV pixel structs, an ppm
*                   image that contains an uarray2 populated with "empty" block
*                   structs, and a specified mapping function
*   Error Conditions: CRE is thrown if the parameters passed in are NULL
*/
void DCTTransform(A2Methods_UArray2 array, Pnm_ppm image, 
                                                        A2Methods_mapfun *map);


/*
*   Summary: inDCTTransform is the function the client should call when 
*            transforming from DCT values back to CV values.
*   Expected Input: A UArray2 that's populated with block pixel structs, an ppm
*                   image that contains an array populated with "empty" CV
*                   structs, and a specified mapping function
*   Error Conditions: CRE is thrown if the parameters passed in are NULL
*/
void inDCTTransform(A2Methods_UArray2 array, Pnm_ppm image,
                                                        A2Methods_mapfun *map);


#endif