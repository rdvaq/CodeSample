/*
 *     uarray2.c
 *     by Matt Zhou(czhou06) and Angus Christie(achris10), 2/14/2022
 *     iii
 *
 *     Implementation of UArray2_T interface using Hanson's UArray_T
 */ 

#include "uarray2.h"

#include <uarray.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define T UArray2_T
                                                                                
struct T
{
    UArray_T arr;
    int width;
    int height;
    int size;
};

/*
 * function: UArray2_new
 *
 * purpose: Consturctor for UArray2 struct. Allocates, initializes, and returns
 * a new 2D array of dimesions height * width. 
 * 
 * parameters: an int specifying height, an int specifying width, and the size
 * of each indiviual element stored in the array. 
 * 
 * returns: a 2D array of type UArray2_T 
 * 
 * exceptions: CRE is thrown if height and width are negative numbers or size 
 * is a nonpositive number  
 */
extern T UArray2_new(int width, int height, int size)
{   
    assert(width >= 0 && height >= 0 && size > 0);

    T uarr2 = malloc(sizeof(*uarr2));
    uarr2->width = width;

    uarr2->height = height;

    uarr2->size = size;

    int elemSize = height * width;
    UArray_T arr = UArray_new(elemSize, size);
    uarr2->arr = arr;
    
    return uarr2;
}

/*
 * function: UArray2_free
 *
 * purpose: free the allocated memory of the target 2D array 
 * 
 * parameters: A pointer to a UArray2_T 2D array  
 * 
 * returns: none 
 * 
 * exceptions: A CRE is raised if uarr2 or *uarr2 is null
 */
extern void UArray2_free(T *uarr2)
{
    assert(uarr2 != NULL && *uarr2 != NULL);
    UArray_free(&(*uarr2)->arr);
    free(*uarr2);
}

/*
 * function: UArray2_height
 *
 * purpose: returns the height of the 2d array  
 * 
 * parameters: A UArray2_T 2D array  
 * 
 * returns: an int denoting the number of elements 
 * 
 * exceptions: A CRE is raised if uarr2 null
 */
extern int UArray2_height(T uarr2)
{
    assert(uarr2 != NULL);
    return uarr2->height;
}

/*
 * function: UArray2_width
 *
 * purpose: returns the width of the 2d array  
 * 
 * parameters: A UArray2_T 2D array  
 * 
 * returns: an int denoting the number of elements 
 * 
 * exceptions: A CRE is raised if uarr2 null
 */
extern int UArray2_width(T uarr2)
{
    assert(uarr2 != NULL);
    return uarr2->width;
}

/*
 * function: UArray2_size
 *
 * purpose: returns the size of a single element 
 * 
 * parameters: A UArray2_T 2D array  
 * 
 * returns: an int denoting the byte size 
 * 
 * exceptions: A CRE is raised if uarr2 null
 */
extern int UArray2_size(T uarr2)
{
    assert(uarr2 != NULL);
    return uarr2->size;
}

/*
 * function: UArray2_at
 *
 * purpose: UArray2_at returns a pointer to the element at column x and row y 
 * in the 2D array 
 * 
 * parameters: A UArray2_T 2D array; an int x specifying which column and an
 * int y specifying which row the element is at.  
 * 
 * returns: a void pointer pointing to the desired element 
 * 
 * exceptions: CRE is thrown if x is >= width or if y >= height or if x and y 
 * are nonpositive numbers; A CRE is raised if uarr2 null
 */
void *UArray2_at(T uarr2, int x, int y)
{
    assert(uarr2 != NULL);
    assert(x < uarr2->width && y < uarr2->height);
    assert(x >= 0 && y >= 0);
    int index = x + (uarr2->width * y);
    return UArray_at(uarr2->arr, index);
}

/*
 * function: UArray2_map_row_major
 *
 * purpose: calls apply for each element in the 2D array in a row-major order 
 * 
 * parameters: A UArray2_T 2D array; An apply function; a closure argument  
 * 
 * returns: none
 * 
 * exceptions: A CRE is raised if uarr2 or apply func is null
 */
extern void UArray2_map_row_major(T uarr2, void apply(int i, int j, 
                                T a, void *p1, void *p2), void *cl)
{
    assert(uarr2 != NULL);
    assert(apply);
    for (int j = 0; j < uarr2->height; j++) {     
        for (int i = 0; i < uarr2->width; i++) {
            int idx = i + (uarr2->width * j);
            apply(i, j, uarr2, UArray_at(uarr2->arr, idx), cl);
        }
    }
    
}

/*
 * function: UArray2_map_col_major
 *
 * purpose: calls apply for each element in the 2D array in a col-major order 
 * 
 * parameters: A UArray2_T 2D array; An apply function; a closure argument  
 * 
 * returns: none
 * 
 * exceptions: A CRE is raised if uarr2 or the apply func is null
 */
extern void UArray2_map_col_major(T uarr2, void apply(int i, int j, 
                                T a, void *p1, void *p2), void *cl)
{
    assert(uarr2 != NULL);
    assert(apply);
    for (int i = 0; i < uarr2->width; i++) {
        for (int j = 0; j < uarr2->height; j++) {
            int idx = i + (uarr2->width * j);
            apply(i, j, uarr2, UArray_at(uarr2->arr, idx), cl);
        }
    }  
}
