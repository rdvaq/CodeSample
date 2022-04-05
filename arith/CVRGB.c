/*
*     CVRGB.c
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Implementation for CVRGB.h
*/ 

 
#include "CVRGB.h"
#include <math.h>
#include "assert.h"


/*
* Function name: RGBtoCV
* Summary: RGBtoCV is the apply function passed to a mapping function that 
*          converts pixels that hold rgb representation to a CV representation
* Inputs:  col and row is the current index in the array, array is the
*          A2Methods_UArray2 that will hold the CV values, CVelem is the
*          current element being inspected and ppm holds the Pnm_ppm image
*          that holds the applicable method functions and A2Methods_UArray2
*          containing rgb values. 
* Outputs: N/A
* Side Effects: The values in 'array' are permanently updated. 
* Error Conditons: CRE if ppm or CVelem are null pointers
*/
void RGBtoCV(int col, int row, A2Methods_UArray2 array, void *CVelem, void *ppm)
{
    (void) array;
    assert(ppm != NULL);
    assert(CVelem != NULL);

    Pnm_ppm image = ppm;   
    A2Methods_UArray2 pixels = image -> pixels;
    float denom = image -> denominator;
    A2Methods_T methods = (const A2Methods_T) image -> methods;
    
    Pnm_rgb curr = methods -> at(pixels, col, row);

    float r = (curr -> red) / denom;
    float g = (curr -> green) / denom;
    float b = (curr -> blue) / denom;
    
    float y = 0.299 * r + 0.587 * g + 0.114 * b;
    float pb = (-0.168736 ) * r - (0.331264 * g) + 0.5 * b;
    float pr = 0.5 * r - (0.418688 * g) - (0.081312 * b);

    Pnm_CV block = CVelem;
    (block) -> y = y;
    (block) -> pb = pb;
    (block) -> pr = pr;

}


/*
* Function name: CVtoRGB
* Summary: CVtoRGB is the apply function passed to a mapping function that 
*          converts pixels that hold component video representation to 
*          an rgb representation
* Inputs:  col and row is the current index in the array, array is the
*          A2Methods_UArray2 that will hold the rgb values, RGBelem is the
*          current element being inspected and ppm holds the Pnm_ppm image
*          that holds the applicable method functions and A2Methods_UArray2
*          containing CV values. 
* Outputs: N/A
* Side Effects: The values in 'array' are permanently updated. 
* Error Conditons: CRE if ppm or RGBelem are null pointers
*/
void CVtoRGB(int col, int row, A2Methods_UArray2 array, void *RGBelem, 
                                                                    void *ppm)
{
    (void) array;
    assert(ppm != NULL);
    assert(RGBelem != NULL);

    Pnm_ppm compressed = ppm;
    A2Methods_UArray2 compressedPixels = compressed -> pixels;
    unsigned denom = compressed -> denominator;
    A2Methods_T methods = (const A2Methods_T) compressed -> methods;
    
    
    Pnm_CV curr = methods -> at(compressedPixels, col, row);
    
    float y = curr -> y;
    float pb = curr -> pb;
    float pr = curr -> pr;
    
    float r = 1.0 * y + 0.0 * pb + 1.402 * pr;
    float g = 1.0 * y - (0.344136 * pb) - (0.714136 * pr);
    float b = 1.0 * y + 1.772 * pb + 0.0 * pr;
    
    /* 
    * Sometimes the provided equations can cause the rgb values to go 
    * negative. These if statements cap RGB values so they are positive 
    * and within a 0-1 range.
    */
    if (r < 0) {
        r = 0;
    } else if (r > 1) {
        r = 1;
    }

    if (g < 0) {
        g = 0;
    } else if (g > 1) {
        g = 1;
    }

    if (b < 0) {
        b = 0;
    } else if (b > 1) {
        b = 1;
    }

    /* Rounds numbers */
    unsigned red = round(r * denom);
    unsigned green = round(g * denom);
    unsigned blue = round(b * denom);
    
    Pnm_rgb block = RGBelem;
    (block) -> red = red;
    (block) -> green = green;
    (block) -> blue = blue;

}


/*
* Function name: RGBtoCVConversion
* Summary: RGBtoCVConversion is function available to other files.
*          It is responsible for calling RGBtoCV using the supplied
*          array, image, and mapping function.
* Inputs:  Expects a valid array holding Pnm_CV structs, a valid
*          Pnm_ppm image with rgb representation, and a valid map fxn.
* Outputs: N/A
* Side Effects: All side effects of RGBtoCV.
* Error Conditons: CRE if any of the supplied input are NULL pointers.
*/
void RGBtoCVConversion(A2Methods_UArray2 array, Pnm_ppm image,
                                                         A2Methods_mapfun *map)
{
    assert(map != NULL && image != NULL && array != NULL);
    map(array, RGBtoCV, image);
}

/*
* Function name: CVtoRGBConversion
* Summary: CVtoRGBConversion is function available to other files.
*          It is responsible for calling CVtoRGB using the supplied
*          array, image, and mapping function.
* Inputs:  Expects a valid array holding Pnm_rgb structs, a valid
*          Pnm_ppm image with CV representation, and a valid map fxn.
* Outputs: N/A
* Side Effects: All side effects of CVtoRGB.
* Error Conditons: CRE if any of the supplied input are NULL pointers.
*/
void CVtoRGBConversion(A2Methods_UArray2 array, Pnm_ppm image, 
                                                        A2Methods_mapfun *map)
{
    assert(map != NULL && image != NULL && array != NULL);
    map(array, CVtoRGB, image);
}