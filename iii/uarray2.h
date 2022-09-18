/*
 *     uarray2.h
 *     by Matt Zhou(czhou06) and Angus Christie(achris10), 2/14/2022
 *     iii
 *
 *     Interface of the UArray2_T data structure
 */ 

#ifndef UARRAY2_H
#define UARRAY2_H
#define T UArray2_T
typedef struct T *T;

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
extern T UArray2_new(int width, int height, int size);

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
extern void UArray2_free(T *uarr2);

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
extern int UArray2_height(T uarr2);

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
extern int UArray2_width(T uarr2);

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
extern int UArray2_size(T uarr2);

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
void *UArray2_at(T uarr2, int x, int y);


/*
 * function: UArray2_map_row_major
 *
 * purpose: calls apply for each element in the 2D array in a row-major order 
 * 
 * parameters: A UArray2_T 2D array; An apply function; a closure argument  
 * 
 * returns: none
 * 
 * exceptions: A CRE is raised if uarr2 null
 */
extern void UArray2_map_row_major(T uarr2, void apply(int i, int j, 
                                T a, void *p1, void *p2), void *cl);

/*
 * function: UArray2_map_col_major
 *
 * purpose: calls apply for each element in the 2D array in a col-major order 
 * 
 * parameters: A UArray2_T 2D array; An apply function; a closure argument  
 * 
 * returns: none
 * 
 * exceptions: A CRE is raised if uarr2 null
 */
extern void UArray2_map_col_major(T uarr2, void apply(int i, int j, 
                                 T a, void *p1, void *p2), void *cl);

#undef T
#endif
