/*
*     CVRGB.h
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Implementation of DCT.h; Exports functions that perfroms DCT 
*     and inverse DCT calculations. 
*/ 

#include <stdlib.h>
#include <stdio.h>

#include "DCT.h"
#include "CVRGB.h"
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"
#include "pnm.h"
#include "mem.h"


/*   AssignCV
 *   Summary: helper functions that populates a Pnm_CV pixel struct with the 
 *   provided y, pb, and pr values
 * 
 *   Expected Input: y, pb, and pr values in floats, and a Pnm_CV struct to be
 *   popualted
 * 
 *   Error Conditions: CRE is thrown if pix is NULL
 */
void assignCV(float y, float pb, float pr, Pnm_CV pix) 
{
    assert(pix != NULL);
    pix -> y = y;
    pix -> pb = pb;
    pix -> pr = pr;
}


/*   DCTMath 
 *   Summary: helper function that performs DCT calculations on the y values 
 *   of 4 elements to obtain the a, b, c, d values. Populate a block struct 
 *   with the calculated values.
 * 
 *   Expected Input: Y values of 4 pixel elements, a Pnm_block struct to be 
 *   populated
 * 
 *   Error Conditions: CRE is thrown if block is NULL
 */
void DCTMath(float y1, float y2, float y3, float y4, Pnm_block block)
{
    assert(block != NULL);
    float a = (y4 + y3 + y2 + y1) / 4.0;
    float b = (y4 + y3 - y2 - y1) / 4.0;
    float c = (y4 - y3 + y2 - y1) / 4.0;
    float d = (y4 - y3 - y2 + y1) / 4.0;
   
    block -> a = a;
    block -> b = b;
    block -> c = c;
    block -> d = d;
    /*initialize codeword to 0*/
    block -> codeword = 0;
}


/*   inDCTMath 
 *   Summary: helper function that performs inverse DCT transformations and
 *   populates pnm_CV structs with the calculated values
 * 
 *   Expected Input: 4 Pnm_CV structs to be populated, a Pnm_block structs that
 *   contins the a, b, c, d, mpb, and mpr values 
 * 
 *   Error Conditions: CRE is thrown if block is NULL
 * 
 *   Note: the helper func called (assignCV) helps check if the CV structs 
 *   passed in are NULL
 */
void inDCTMath(Pnm_CV pix1, Pnm_CV pix2, Pnm_CV pix3, Pnm_CV pix4, 
                                                            Pnm_block block)
{
    assert(block != NULL);

    float a = block -> a;
    float b = block -> b;
    float c = block -> c;
    float d = block -> d;
    float mpb = block -> mpb;
    float mpr = block -> mpr;
    
    /* inverse DCT calculatioins*/
    float y1 = a - b - c + d;
    float y2 = a - b + c - d;
    float y3 = a + b - c - d;
    float y4 = a + b + c + d;
    
    /*populate the CV structs with calculated values*/
    assignCV(y1, mpb, mpr, pix1);
    assignCV(y2, mpb, mpr, pix2);
    assignCV(y3, mpb, mpr, pix3);
    assignCV(y4, mpb, mpr, pix4);
}

/*   blockMap 
 *   Summary: mapping function that performs DCT transformations by block. 
 *   This mapping function should be called on a UArray2 with Pnm_CV structs,
 *   it populates a UArray2 of Pnm_block structs
 * 
 *   Expected Input: col and row index, and UArray2 of Pnm_CV structs, 
 *   a pointer to the current element in the UArray2, and a closure argument 
 *   that takes in a Pnm_ppm image. The image should contain a UArray2 of 
 *   Pnm_block structs.
 * 
 *   Error Conditions: CRE is thrown if the ppm image passed in is NULL or 
 *   if the mapped array is NULL
 * 
 */
void blockMap(int col, int row, A2Methods_UArray2 array, void *CVstruct, 
                                                                void *blockppm)
{
    assert(array != NULL && blockppm != NULL);
    /* we only operate on the "top left" element of a 2 by 2 block*/
    if (row % 2 != 0 || col % 2 != 0) {
        return;
    }
    
    Pnm_ppm image = blockppm;
    A2Methods_UArray2 pixels = image -> pixels;
    A2Methods_T methods = (const A2Methods_T) image -> methods;
    /* get all CV structs inside the 2 by 2 block*/
    Pnm_CV pix1 = methods -> at(array, col, row);
    Pnm_CV pix2 = methods -> at(array, col + 1, row);
    Pnm_CV pix3 = methods -> at(array, col, row + 1);
    Pnm_CV pix4 = methods -> at(array, col + 1, row + 1);
    /* calculate mean pb and pr values */
    float mpb = ((pix1 -> pb) + (pix2 -> pb) + (pix3 -> pb) + 
                                                    (pix4 -> pb)) / 4.0;
    float mpr = ((pix1 -> pr) + (pix2 -> pr) + (pix3 -> pr) + 
                                                    (pix4 -> pr)) / 4.0;

    float y1 = pix1 -> y;
    float y2 = pix2 -> y;
    float y3 = pix3 -> y;
    float y4 = pix4 -> y;
    /* get the corresponding block struct*/
    Pnm_block curr = methods -> at(pixels, col / 2, row / 2);
    curr -> mpb = mpb;
    curr -> mpr = mpr;
    /*call helper to do math*/
    DCTMath(y1, y2, y3, y4, curr);
    (void) CVstruct;
}

/*   inblockMap 
 *   Summary: mapping function that performs inverse DCT transformations 
 *   by block. 
 *   This mapping function should be called on a UArray2 with Pnm_block 
 *   structs, it populates a UArray2 of Pnm_CV structs
 * 
 *   Expected Input: col and row index, and UArray2 of Pnm_block structs, 
 *   a pointer to the current element in the UArray2, and a closure argument 
 *   that takes in a Pnm_ppm image. The image should contain a UArray2 of 
 *   Pnm_CV structs.
 * 
 *   Error Conditions: CRE is thrown if the ppm image passed in is NULL 
 * 
 */
void inBlockMap(int col, int row, A2Methods_UArray2 array, void *blockStruct, 
                                                                void *CVppm)
{
    assert(CVppm != NULL);
    Pnm_ppm image = CVppm;
    A2Methods_UArray2 pixels = image -> pixels;
    A2Methods_T methods = (const A2Methods_T) image -> methods;

    /*get the corresponding index of the CV UArray2*/
    int newRow = row * 2;
    int newCol = col * 2;
    /*get all the CV pixels in a 2 by 2 block*/
    Pnm_CV pix1 = methods -> at(pixels, newCol, newRow);
    Pnm_CV pix2 = methods -> at(pixels, newCol + 1, newRow);
    Pnm_CV pix3 = methods -> at(pixels, newCol, newRow + 1);
    Pnm_CV pix4 = methods -> at(pixels, newCol + 1, newRow + 1);

    Pnm_block curr = blockStruct;
    /* call helper to do inverse calculations*/
    inDCTMath(pix1, pix2, pix3, pix4, curr);

    (void) array;
}

/*
 *   Summary: Wrapper for blockmap mapping function. DCTTransform is the 
 *   function the client should call when transforming from CV pixel format 
 *   to DCT values. Note that the mean of the chroma Pb and Pr values are also 
 *   calculated per block along the DCT calculations.
 * 
 *   Expected Input: A UArray2 that's populated with CV pixel structs, an ppm
 *   image that contains an uarray2 populated with "empty" block structs, and 
 *   a specified mapping function
 * 
 *   Error Conditions: CRE is thrown if the parameters passed in are NULL
 */
void DCTTransform(A2Methods_UArray2 array, Pnm_ppm image,
                                                         A2Methods_mapfun *map)
{
    assert(array != NULL && image != NULL && map != NULL);
    map(array, blockMap, image);
} 


/*
 *   Summary: wrapper for our inblockmap mapping function. inDCTTransform is 
 *   the function the client should call when transforming from DCT values 
 *   back to CV values.
 * 
 *   Expected Input: A UArray2 that's populated with block pixel structs, 
 *   a ppm image that contains an array with "empty" structs, and a specified 
 *   mapping function
 * 
 *   Error Conditions: CRE is thrown if the parameters passed in are NULL
 */
void inDCTTransform(A2Methods_UArray2 array, Pnm_ppm image, 
                                                        A2Methods_mapfun *map)
{
    assert(array != NULL && image != NULL && map != NULL);
    map(array, inBlockMap, image);
}
