/*
 *     test.c
 *     by Matt Zhou(czhou06) and Angus Christie(achris10), 2/7/2022
 *     iii
 *
 *     
 */ 
#include <stdlib.h>
#include <stdio.h>=
#include <stdbool.h>
#include "uarray2.h"




typedef long number;


void
check_and_print(int i, int j, UArray2_T a, void *p1, void *p2) 
{
        number *entry_p = p1;

        *((bool *)p2) &= UArray2_at(a, i, j) == entry_p;

        unsigned *num = UArray2_at(a, i, j);
        printf("%u\n", *num);
}


void printCheck(UArray2_T arr2)
{
    
    bool OK = true;
    OK = (UArray2_width(arr2) == 9) &&
	     (UArray2_height(arr2) == 9) ;
         

    printf("Trying column major\n");
    UArray2_map_col_major(arr2, check_and_print, &OK);

    printf("Trying row major\n");
    UArray2_map_row_major(arr2, check_and_print, &OK);

    printf("The array is %sOK!\n", (OK ? "" : "NOT "));
}

