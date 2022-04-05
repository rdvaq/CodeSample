/*
*     CVRGB.h
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Interface for CVRBG.c This header file defines functions
*     called by compress40.c. The functions here are responsible for 
*     both converting the RGB values in the given uncompressed image to 
*     y, pb, pr values and the inverse conversion. These functions are 
*     responsible for the following steps: Floating Point Transformation,
*     RGB to CV transformation, Inverse Floating Point Transformation, and 
*     CV to RGB transformation. CVRGB.h is included in all files that use 
*     the Pnm_CV struct defined here. 
*/

#include <stdlib.h>
#include <stdio.h>
#include "a2methods.h"
#include <pnm.h>

#ifndef CVRGB_H
#define CVRGB_H


/*
 * Pnm_Cv struct 
 * pixel struct that stores y, pb, pr values
 */
typedef struct Pnm_CV {
    float y, pb, pr;
} *Pnm_CV;




/*
*   Summary: RGBtoCVConversion is the function the client should call when 
*            transforming from an RGB image representation to a Component
*            Video transformation.
*   Expected Input: array should be a valid A2Methods_UArray2 with space
*                   allocated for Pnm_CV structs that the user wishes
*                   to populate, image should be the uncompressed image
*                   containing an RGB representation, and a function pointer
*                   to the desired mapping function (in this case row_major
*                   is expected)
*/
void RGBtoCVConversion(A2Methods_UArray2 array, Pnm_ppm image, 
                                                        A2Methods_mapfun *map);

/*
*   Summary: CVtoRBGConversion is the function the client should call when 
*            transforming from an CV image representation to a RGB 
*            transformation.
*   Expected Input: array should be a valid A2Methods_UArray2 with space
*                   allocated for Pnm_rgb structs that the user wishes
*                   to populate, image should be the compressed image
*                   containing an CV representation, and a function pointer
*                   to the desired mapping function (in this case row_major
*                   is expected)
*/
void CVtoRGBConversion(A2Methods_UArray2 array, Pnm_ppm image, 
                                                        A2Methods_mapfun *map);

#endif