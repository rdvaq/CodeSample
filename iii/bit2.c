/*
 *     bit2.c
 *     by Matt Zhou(czhou06) and Angus Christie(achris10), 2/14/2022
 *     iii
 *
 *     Implementation for the bit2.h interface, uses Hanson's 1D Bit vectors;
 */ 

#include "bit2.h"
#include <bit.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define T Bit2_T

struct T
{
    Bit_T bit;
    int width;
    int height;   
};

/*
 * function: Bit2_new
 *
 * purpose: Consturctor for Bit2 struct. Allocates, initializes, and returns
 * a new 2D bit array of dimesions width * height.
 *
 * parameters: an int specifying width, an int specifying height
 *
 * returns: a 2D bit array of type Bit2_T
 *
 * exceptions: CRE is thrown if height and width are negative numbers  
 */
extern T Bit2_new(int width, int height)
{
    assert(width >= 0 && height >= 0);
    T bit2 = malloc(sizeof(*bit2));
    assert(bit2 != NULL);
    
    bit2->width = width;
    bit2->height = height;
    int elemSize = height * width;
    Bit_T bit = Bit_new(elemSize);
    bit2->bit = bit;
    
    return bit2;
}

/*
 * function: Bit2_free
 *
 * purpose: free the allocated memory of the target 2D bit array
 *
 * parameters: A pointer to the target Bit2_T 2D array  
 *
 * returns: none
 *
 * exceptions: A CRE is raised if set or *set is null
 */
extern void Bit2_free(T *set)
{
    assert(set != NULL && *set != NULL);
    Bit_free(&(*set)->bit);
    free(*set);
}

/*
 * function: Bit2_height
 *
 * purpose: returns the height of the 2d bit array
 *
 * parameters: a 2d bit array 
 *
 * returns: and int representing the height
 *
 * exceptions: A CRE is raised if set is null
 */
extern int Bit2_height(T set)
{
    assert(set != NULL);
    return set->height;
}

/*
 * function: Bit2_width
 *
 * purpose: returns the width of the 2d bit array
 *
 * parameters: a 2d bit array 
 *
 * returns: an int representing the width
 *
 * exceptions: A CRE is raised if set is null
 */
extern int Bit2_width(T set)
{
    assert(set != NULL);
    return set->width;
}

/*
 * function: Bit2_get
 *
 * purpose: returns the bit value stored at col i and row j of the 2d bit array
 *
 * parameters: a 2d bit array, an int representing col index and an int 
 * representing row index
 *
 * returns: the bit value at desired index
 *
 * exceptions: CRE is thrown if i is >= width or if j >= height or if i and j
 * are nonpositive numbers; A CRE is raised if set is null
 */
extern int Bit2_get(T set, int i, int j)
{
    assert(set != NULL);
    assert(i < set->width && j < set->height);
    assert(i >= 0 && j >= 0);
    int idx = i + (set->width) * j;
    return Bit_get(set->bit, idx);
}

/*
 * function: Bit2_put
 *
 * purpose: set the bit value stored at col i and row j of the 2d bit array 
 * to the bit value passed in; returns the value that’s previously 
 * stored at (i,j)
 *
 * parameters: a 2d bit array; an int representing col index and an int 
 * representing row index; an int representing the input bit value 
 *
 * returns: the previous bit value stored  desired index
 *
 * exceptions: CRE is thrown if i is >= width or if j >= height or if i and j
 * are nonpositive numbers; A CRE is raised if set is null
 */
extern int Bit2_put(T set, int i, int j, int bit)
{
    assert(set != NULL);
    assert(i < set->width && j < set->height);
    assert(i >= 0 && j >= 0);
    int idx = i + (set->width) * j;     
    return Bit_put(set->bit, idx, bit);
}

/*
 * function: Bit2_map_row_major
 *
 * purpose: calls the apply function on each element in the 2D bit 
 * array in a row-major order
 *
 * parameters: A 2D bit array; An apply function; a closure argument  
 *
 * returns: none
 *
 * exceptions: A CRE is raised if set or apply null
 */
extern void Bit2_map_row_major(T set, void apply(int i, int j, T a,
                                                int b, void *p1), void *cl)
{
    assert(set != NULL);
    assert(apply);
    for (int j = 0; j < set->height; j++) {     
        for (int i = 0; i < set->width; i++) {
            int idx = i + (set->width * j);
            apply(i, j, set, Bit_get(set->bit, idx), cl);
        }
    }
}

/*
 * function: Bit2_map_col_major
 *
 * purpose: calls the apply function on each element in the 2D bit array 
 * in a col-major order
 *
 * parameters: A 2D bit array; An apply function; a closure argument  
 *
 * returns: none
 *
 * exceptions: A CRE is raised if set or apply null
 */ 
extern void Bit2_map_col_major(T set, void apply(int i, int j, T a,
                                                int b, void *p1), void *cl)
{
    assert(set != NULL);
    assert(apply);
    for (int i = 0; i < set->width; i++) {     
        for (int j = 0; j < set->height; j++) {
            int idx = i + (set->width * j);
            apply(i, j, set, Bit_get(set->bit, idx), cl);
        }
    }

}