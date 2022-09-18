/*
 *     bit2.h
 *     by Matt Zhou(czhou06) and Angus Christie(achris10), 2/14/2022
 *     iii
 *
 *     Interface for the BIT2_T data strucure, which is a 2d bit array
 */ 

#ifndef BIT2_H
#define BIT2_H

#define T Bit2_T
typedef struct T *T;


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
extern T Bit2_new(int width, int height);

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
extern void Bit2_free(T *set);

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
extern int Bit2_height(T set);

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
extern int Bit2_width(T set);

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
extern int Bit2_get(T set, int i, int j);

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
extern int Bit2_put(T set, int i, int j, int bit);

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
                                                int b, void *p1), void *cl);

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
                                                int b, void *p1), void *cl);


#undef T
#endif