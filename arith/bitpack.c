/*
*     bitpack.c
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Implementation of bitpack.h
*/ 

#include <stdlib.h> 
#include <stdio.h>

#include "bitpack.h"
#include "assert.h"

const unsigned WIDTHMAX = 64;
const unsigned WIDTHMIN = 0;

Except_T Bitpack_Overflow = { "Overflow packing bits" };


/*
* Function name: rightShiftu
* Summary: rightShiftu is a helper function for right shifting unsigned numbers
* Inputs: number is the number to be shifted and shift is the amount the number
*         should be shifted by
* Outputs: function returns a uint64_t of the shifted number 
* Side Effects: N/A
* Error Conditons: CRE if shift is greater than or equal to 64
*/
uint64_t rightShiftu(uint64_t number, unsigned shift)
{  
    assert(shift <= WIDTHMAX);

    if (shift == WIDTHMAX) {
        return 0;
    }

    number = number >> shift;
    return number;
}

/*
* Function name: rightShifts
* Summary: rightShifts is a helper function for right shifting signed numbers
* Inputs: number is the number to be shifted and shift is the amount the number
*         should be shifted by
* Outputs: function returns a int64_t of the shifted number 
* Side Effects: N/A
* Error Conditons: CRE if shift is greater than or equal to 64
*/
int64_t rightShifts(int64_t number, unsigned shift)
{
    assert(shift <= WIDTHMAX);

    if (shift == WIDTHMAX) {
        return 0;
    }
    
    number = number >> shift;
    return number;
}

/*
* Function name: leftShift
* Summary: leftShift is a helper function for left shifting both signed 
*          and unsigned numbers
* Inputs: number is the number to be shifted and shift is the amount the number
*         should be shifted by
* Outputs: function returns a uint64_t of the shifted number 
* Side Effects: N/A
* Error Conditons: CRE if shift is greater than or equal to 64
*/
uint64_t leftShift(uint64_t number, unsigned shift)
{
    assert(shift <= WIDTHMAX);

    if (shift == WIDTHMAX) {
        return 0;
    }

    number = number << shift;
    return number;
}


/*
* Function name: Bitpack_fitsu
* Summary: Bitpack_fitsu checks whether an unsigned number 'n' will fit in 
*          'width' number of bits 
* Inputs: n is the unsigned number and width is the number of bits available
* Outputs: returns bool that is true if n can fit in the desired width and 
*          false if it cannot.
* Side Effects: N/A
* Error Conditons: CRE if width is 0 or greater than or equal to 64
*/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= WIDTHMAX);
    
    if (width == WIDTHMAX) {
        return true;
    } else if (width == WIDTHMIN) {
        return false;
    }

    /* product is max val that can be held using width # of bits */
    uint64_t product = leftShift(1, width);

    return (n < product);
}

/*
* Function name: Bitpack_fitss
* Summary: Bitpack_fitss checks whether an signed number 'n' will fit in 
*          'width' number of bits 
* Inputs: n is the signed number and width is the number of bits available
* Outputs: returns bool that is true if n can fit in the desired width and 
*          false if it cannot.
* Side Effects: N/A
* Error Conditons: CRE if width is 0 or greater than or equal to 64
*/
bool Bitpack_fitss(int64_t n, unsigned width)
{   
    assert(width <= WIDTHMAX);

    if (width == WIDTHMIN) {
        return false;
    }
    
    /* lower is the max negative value that can be held in width bits */
    int64_t lower = leftShift(~0, width - 1);
    
    /* upper is the max positive value that can be held in width bits */
    int64_t upper = rightShiftu(~0, WIDTHMAX - width + 1);

    return (n >= lower && n <= upper);
}


/*
* Function name: Bitpack_getu
* Summary: Bitpack_getu returns the unsigned value of the number
*          represented at the field from the provided word with 
*          the provided width beginning at the provided
*          least significant bit. 
* Inputs: word is the codeword, width is the width of the value 
*         to be extracted, and lsb is the least significant but
*         of the desired number
* Outputs: returns a uint64_t with the extracted value
* Side Effects: N/A
* Error Conditons: CRE if width is less than 0 or greater than 64. 
*                  CRE if the the width beginning at lsb goes past 64.
*/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= WIDTHMAX && (width + lsb) <= WIDTHMAX);

    if (width == WIDTHMIN) {
        return 0;
    }

    /* leadingZeros is the number of zeros before the desired field */
    unsigned leadingZeros = WIDTHMAX - width;

    /* creates a mask with the correct number of zeros before the field */
    uint64_t mask = rightShiftu(~0, leadingZeros);
    /* shifts the mask so that there are all zeros before the lsb */
    mask = leftShift(mask, lsb);

    /* & statement extracts only the values at the desired field */
    uint64_t extract = word & mask;
    /* shifts the extracted value to the beginning of the value */
    extract = rightShiftu(extract, lsb);

    return extract;
}


/*
* Function name: Bitpack_gets
* Summary: Bitpack_gets returns the signed value of the number
*          represented at the field from the provided word with 
*          the provided width beginning at the provided
*          least significant bit. 
* Inputs: word is the codeword, width is the width of the value 
*         to be extracted, and lsb is the least significant but
*         of the desired number
* Outputs: returns a int64_t with the extracted value
* Side Effects: N/A
* Error Conditons: CRE if width is less than 0 or greater than 64. 
*                  CRE if the the width beginning at lsb goes past 64.
*/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= WIDTHMAX && (width + lsb) <= WIDTHMAX);

    if (width == WIDTHMIN) {
        return 0;
    }
    /* uses getu to retrieve the desired field*/
    uint64_t val = Bitpack_getu(word, width, lsb);

    /* 
    * maskNegCheck is a mask used to check whether the returned value 
    * has a negative sign bit
    */
    uint64_t maskNegCheck = rightShiftu(~0, WIDTHMAX - 1);
    maskNegCheck = leftShift(maskNegCheck, width - 1);
    
    /* if the maskNegCheck & val is non zero, the value is negative */
    if (maskNegCheck & val) {
        /* if loop is entered, value is negative and needs leading 1's */
        int64_t makeNeg = leftShift(~0, width);
        val = (int64_t) val | makeNeg;
    }

    return val;
}

/*
* Function name: Bitpack_newu
* Summary: Bitpack_newu is used to create a new codeword that is the same
*          as the supplied code word except the indicated field with supplied
*          width and lsb is populated with the supplied value. 
* Inputs: word is the codeword with the original values, width is the width 
*         of the field to be updated, lsb is the least significant bit of 
*         of the field, and value is the unsigned value that will replace the 
*         indicated field in the newly returned codeword
* Outputs: returns a new codeword that is the same as the supplied word except
*          the indicated field has been updated with the desired value
* Side Effects: N/A
* Error Conditons: CRE if supplied width is less than zero or greater than 64.
*                  lsb if a field of the supplied width goes past the 64 bit
*                  limit    
*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
    assert(width <= WIDTHMAX && (width + lsb) <= WIDTHMAX);

    /* raises overflow error if value does not fit in the supplied width */
    if (!Bitpack_fitsu(value, width)) {
        RAISE(Bitpack_Overflow);
    }
    
    /* creates a mask with zeros in the field to be updated*/
    uint64_t mask_1 = leftShift(~0, width + lsb);

    /* If lsb is greater than 0, updates so that only desired field contains 
       zeros */
    if (lsb != 0) {
        uint64_t mask_lsb = rightShiftu(~0, WIDTHMAX - lsb);
        mask_1 = mask_1 | mask_lsb;
    } 

    /* mask that contains value at the desired field */
    uint64_t mask_val = leftShift(value, lsb);

    /* creates a new word with the original codeword values, but zeros at the 
      desired field */
    uint64_t newWord = word & mask_1;
    newWord = newWord | mask_val; 
    
    return newWord;
}

/*
* Function name: Bitpack_news
* Summary: Bitpack_news is used to create a new codeword that is the same
*          as the supplied code word except the indicated field with supplied
*          width and lsb is populated with the supplied value. 
* Inputs: word is the codeword with the original values, width is the width 
*         of the field to be updated, lsb is the least significant bit of 
*         of the field, and value is the signed value that will replace the 
* Outputs: returns a new codeword that is the same as the supplied word except
*          the indicated field has been updated with the desired value
* Side Effects: N/A
* Error Conditons: CRE if supplied width is less than zero or greater than 64.
*                  lsb if a field of the supplied width goes past the 64 bit
*                  limit    
*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                      int64_t value)
{
    assert(width <= WIDTHMAX && (width + lsb) <= WIDTHMAX);

    /* raises overflow error if value does not fit in the supplied width */
    if (!Bitpack_fitss(value, width)) {
        RAISE(Bitpack_Overflow);
    }

    /* creates a mask with zeros in the field to be updated*/
    uint64_t mask_1 = leftShift(~0, width + lsb);

    if (lsb != 0) {
        uint64_t mask_lsb = rightShiftu(~0, WIDTHMAX - lsb);
        mask_1 = mask_1 | mask_lsb;
    }
 
    /* create a mask that has value at correct pos and 0s elsewhere*/
    uint64_t mask_val = leftShift(value, WIDTHMAX - width);
    mask_val = rightShiftu(mask_val, (WIDTHMAX - (lsb + width)));
    
    /* now newWord has 0s at desired field*/
    uint64_t newWord = word & mask_1;
    newWord = newWord | mask_val; 

    return newWord;
}
