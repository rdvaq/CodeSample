/*
*     quantization.c
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Implementation of quantization.h
*/ 

#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include "assert.h"
#include "quantization.h"

const float interval = 103.0;
const float upperChroma = 0.30;
const float lowerChroma = -0.30;
const int aVal = 63;


/*
* Function name: quantChroma
* Summary: quantChroma takes in the float representation of b, c, or d value
*          from a CV image representation and quantizes them to a number 
*          within the -15 to 15 number range required by the spec. 
* Inputs: b, c, or d value from a CV representation
* Outputs: integer representation between -15 and 15 of the provided 
*          b, c, or d value. 
* Side Effects: N/A
* Error Conditons: No CRE called.
*/
int quantChroma(float bcd) 
{
    /* Caps values to be within range indicated by the spec */
    if (bcd > upperChroma) {
        bcd = upperChroma;
    } else if (bcd < lowerChroma) {
        bcd = lowerChroma;
    }

    int chroma = roundf(bcd * interval);
    return chroma;   
}

/*
* Function name: quantA
* Summary: quantA takes in the float representation of an a value
*          from a CV image representation and quantizes it to a
*          number that can be represented in 9 bits. 
* Inputs: a value from a CV representation
* Outputs: unsigned representation of a that can fit in 9 bits.
* Side Effects: N/A
* Error Conditons: No CRE called.
*/
unsigned quantA(float a)
{
    /* Caps values to be within range indicated by the spec */
    if (a > 1) {
        a = 1;
    } else if (a < 0){
        a = 0;
    }

    return roundf(a * aVal);

} 

/*
* Function name: inQuantChroma
* Summary: inQuantChroma takes in the quantized representation of 
*          b, c, or d value and reverts them to their original CV 
*          representation. 
* Inputs: b, c, or d quantized value
* Outputs: float representation between spec provided chroma range
* Side Effects: N/A
* Error Conditons: No CRE called.
*/
float inQuantChroma(int bcd) 
{
    float chroma = (float) bcd / interval;

    if (chroma > upperChroma) {
        chroma = upperChroma;
    } else if (chroma < lowerChroma) {
        chroma = lowerChroma;
    }

    return chroma;
}

/*
* Function name: inQuantA
* Summary: inQuantA takes in the quantized representation of 
*          an a CV value and reverts it to its original CV representation 
* Inputs: a quantized 'a' CV value
* Outputs: float value for 'a' in CV representation
* Side Effects: N/A
* Error Conditons: No CRE called.
*/
float inQuantA(unsigned a) 
{
    float chroma = (float) a / aVal;

    return chroma;
}