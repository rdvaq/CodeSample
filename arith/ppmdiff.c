/*
 *   Matt Zhou(czhou06) and Violet McCabe(vmccab01)
 *   PPMDIFF
 *   A tester that tests the percent difference between the rgb vals of two 
 *   images
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pnm.h"
#include <math.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "mem.h"

/*
* closure struct for the map function, contains data of the "other image"
* also contains mapping bounds (minHeight & minWidth)
*/
struct Mycl {
    
    A2Methods_T methods;
    A2Methods_UArray2 array;
    double *sum;
    int minHeight;
    int minWidth;
    double denom;

};
typedef struct Mycl *Mycl;

/*
* getmin function
*/
int mins(int a, int b)
{
    return (a > b) ? b : a;
}

/*
* map function that calculates the difference betwen rgb values at a pixel
*/
void diff(int col, int row, A2Methods_UArray2 array, void *elem, void *cl)
{
    (void) array;
    Pnm_rgb curr = elem;
    Mycl vcl = cl;
    if (col < vcl -> minWidth && row < vcl -> minHeight) {
        A2Methods_T methods = vcl -> methods;
        A2Methods_UArray2 array2 = vcl -> array;
        Pnm_rgb other = methods -> at(array2, col, row);
        double denom = vcl -> denom;
        double redDiff = ((double)(curr -> red) - (other -> red)) / denom;
        double greenDiff = ((double)(curr -> green) - (other -> green)) / denom;
        double blueDiff = ((double)(curr -> blue) - (other -> blue)) / denom;

        double *sum = vcl -> sum;
        *sum += (double)pow(redDiff, 2);
        *sum += (double)pow(greenDiff, 2);
        *sum += (double)pow(blueDiff, 2);       
    }
    
}

/*
main func of the program, takes in two ppm files and outputs diff value
*/
int main(int argc, char *argv[])
{
    if (argc != 3) {
        
        printf("Usage: ./ppmdiff [file] [file]");
        exit(1);
    } 

    FILE *fp1;
    FILE *fp2;

    fp1 = fopen(argv[1], "r");
    fp2 = fopen(argv[2], "r");

    assert(fp1 != NULL && fp2 != NULL);

    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods != NULL);

    /* default to best map */
    A2Methods_mapfun *map = methods->map_default; 
    assert(map != NULL);
    
    Pnm_ppm image1 = Pnm_ppmread(fp1, methods);
    Pnm_ppm image2 = Pnm_ppmread(fp2, methods);

 
    int width1 = image1 -> width;
    int width2 = image2 -> width;

    int height1 = image1 -> height;
    int height2 = image2 -> height;
    
    /* Catch width and height error */
    if (abs(width1 - width2) > 1 || abs(height1 - height2) > 1) {
        fprintf(stderr, "Error: height and width of images should differ %s", 
                        "by at most 1.0 \n");
        fprintf(stdout, "1.0\n");
        exit(1);
    }

    int minWidth = mins(width1, width2);
    int minHeight = mins(height1, height2);
    double sum = 0;

    Mycl cl;
    NEW(cl);
    assert(cl != NULL);
   
    cl -> methods = methods;
    cl -> array = image2 -> pixels;
    cl -> minHeight = minHeight;
    cl -> minWidth = minWidth;
    cl -> denom = (double)(image1 -> denominator);
    cl -> sum = &sum;
    
    map(image1 -> pixels, diff, cl);

    double bigDenom = 3 * minWidth * minHeight;
    double rmsd = sqrt((sum / bigDenom));

    printf("%.4f\n", rmsd);
}