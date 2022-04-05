/*
*     quantization.h
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Interface for quantization.h. This header file defines functions
*              called by bitprep.c. These functions are responsible for 
*              the quantization and inverse quantization required by
*              image compression. 
*/ 


#ifndef QUANTIZATION_H
#define QUANTIZATION_H


/*
*   Summary: quantChroma takes in the b, c, or d values from a CV 
*            representation and quantizes them to an integer representation.
*   Expected Input: valid float
*   Error Conditions: No CRE called.
*/
int quantChroma(float bcd);

/*
*   Summary: quantA takes in the a value from a CV 
*            representation and quantizes it to an unsigned representation.
*   Expected Input: valid float
*   Error Conditions: No CRE called.
*/
unsigned quantA(float a);

/*
*   Summary: inQuantA takes in the quantized a value unpacked from a 
*            codeword and calculates it to its original a value. 
*   Expected Input: valid unsigned
*   Error Conditions: No CRE called.
*/
float inQuantA(unsigned a);

/*
*   Summary: inQuantChroma takes in the quantized representation of b, c,
*            or d values from an unpacked codeword and calculates them to
*            their original values.
*   Expected Input: valid integer
*   Error Conditions: No CRE called.
*/
float inQuantChroma(int bcd);

#endif
