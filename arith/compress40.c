/*
*     compress40.c
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Implementation of the compress40.h interface. Exports two major
*     functions: compress40 and decompress40. This is the main module of the 
*     program. It calls all other modules to perfrom the compression and 
*     decompression steps. It is also responsible to perform image trimming.
*/


#include <stdlib.h>
#include <stdio.h>
#include <pnm.h>
#include "compress40.h"
#include "a2methods.h"
#include "a2plain.h"
#include "DCT.h"
#include "CVRGB.h"
#include "mem.h"
#include "assert.h"
#include "bitprep.h"
#include "reader.h"

/* define a set denominator value*/
const int DENOM = 255;

/* 
 * trim
 * Summary: trims the image dimensions in case the image demensions are odd 
 * numbers. This allows the "perfect" divison of the set image into 2 by 2 
 * blocks
 * 
 * Inputs: A pointer to a ppm image
 * 
 * Outputs: none
 * 
 * Error Conditons: CRE is thrown if image or *image is null
 * 
 * Note: We preform a "fake" trim here, we don't actually delete any data but
 * simply change the height and the width of the ppm image. Since the 
 * dimensions of the new uarrays we create are based on the dimensions of the
 * updated ppm, the effect of trimming is achieved.
 */
void trim(Pnm_ppm *image) 
{
    assert(image != NULL && *image != NULL);

    unsigned *width = &((*image) -> width);
    if (*width % 2 != 0) {
        *width -= 1;
    }
    unsigned *height = &((*image) -> height);
    if (*height % 2 != 0) {
        *height -= 1;
    }
}

/* 
 * PPMmaker
 * Summary: helper function that creates a ppm image
 * 
 * Inputs: all necessary components of a ppm image
 * 
 * Outputs: a PPM image
 * 
 * Error Conditons: CRE is thrown is allocating memory for the new image fails
 */
Pnm_ppm PPMmaker(unsigned denom, unsigned width, unsigned height, 
                                 A2Methods_UArray2 pixels, A2Methods_T methods)
{
    Pnm_ppm image;
    NEW(image);
    assert(image != NULL);
    image -> denominator = denom;
    image -> width = width;
    image -> height = height;
    image -> methods = methods;
    image -> pixels = pixels;
    return image;
}

/* 
 * compress40
 *
 * Summary: Function that perfroms the compression of an input image. Calls 
 * all the functions exported by the included modules to perform compression 
 * and write out
 * 
 * Inputs: A file stream containing image data
 * 
 * Outputs: none
 * 
 * Error Conditons: 
 */
extern void compress40(FILE *input) 
{    
    /* read in process */
    A2Methods_T methods = uarray2_methods_plain;
    Pnm_ppm image = Pnm_ppmread(input, methods);
    
    /* trim */
    trim(&image); 

    A2Methods_mapfun *map = methods -> map_default;
    /* Create an array with CV structs to store the output of RGB to CV
    conversions */
    A2Methods_UArray2 RGBtoCV = methods -> new(image -> width, image -> height, 
                                                        sizeof(struct Pnm_CV));
    
    /* RGB -> CV */
    RGBtoCVConversion(RGBtoCV, image, map);
    
    /* Create an array with block structs that stores the output of CV to DCT
    conversions; The dimensions are adjusted accordingly */
    A2Methods_UArray2 compressed = methods -> new((image -> width) / 2,
                            (image -> height) / 2, sizeof(struct Pnm_block));

    /*Create a ppm image that can be passed into our CV -> DCT transfomration*/
    Pnm_ppm compressedImage = PPMmaker(DENOM, methods -> width(compressed),
                        methods -> height(compressed), compressed, methods);

    /*populate compressed with DCT values*/
    DCTTransform(RGBtoCV, compressedImage, map);

    /* map through compressed again to pack the codewords */
    packWord(compressed, map);

    /* write to stdout */
    writeout(image -> width, image -> height, compressed, map);

    methods -> free(&RGBtoCV);
    Pnm_ppmfree(&image);
    Pnm_ppmfree(&compressedImage); 
}

/* 
 * decompress40
 *
 * Summary: Function that perfroms the decompression of the input image. Calls 
 * all the functions exported by the included modules to perform decompression 
 * and write out the decompressed image to stdout
 * 
 * Inputs: A file stream containing image data
 * 
 * Outputs: none
 * 
 * Error Conditons: 
 */
extern void decompress40(FILE *input) 
{
    A2Methods_T methods = uarray2_methods_plain;
    A2Methods_mapfun *map = methods -> map_default;
    /*width and height of the original image*/
    unsigned width = 0;
    unsigned height = 0;
    
    /* compressed should be a UArray2 with block structs*/ 
    A2Methods_UArray2 compressed = readin(input, methods, &width, &height);
    
    /* map through compressed to unpack all the codewords and populate 
    corresponding fields in the block struct*/
    unpackWord(compressed, map);

    /* create a new uarray2 of the original image's width and height to store
    the output of DCT to CV transformation*/
    A2Methods_UArray2 inverse = methods -> new(width, height, 
                                                        sizeof(struct Pnm_CV));

    /* create a ppm image to pass to our inverse DCT transformation, ppm image
    contains the inverse array*/
    Pnm_ppm decompressedImage = PPMmaker(DENOM, width, height, inverse, 
                                                                    methods);

    /* perfrom inverse DCT transformation and populate the invese array with
    CV values */
    inDCTTransform(compressed, decompressedImage, map);

    /* create an uarray2 that stores rgb structs */
    A2Methods_UArray2 rgbMap = methods -> new(width, height, 
                                                    sizeof(struct Pnm_rgb));
    
    /*perfrom CV -> RGB and populate the rgbMap*/
    CVtoRGBConversion(rgbMap, decompressedImage, map);

    /* assigned rgbmap to the created ppm image, the orignal uarray2 stored
    in the decompressedImage will be freed later */
    decompressedImage -> pixels = rgbMap;
    
    Pnm_ppmwrite(stdout, decompressedImage);
   

    methods -> free(&compressed);
    methods -> free(&inverse);
    Pnm_ppmfree(&decompressedImage);
}

