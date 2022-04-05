/*
*     bitprep.c
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Implementation of bitprep.h
*/ 

#include <stdlib.h>
#include <stdio.h>
#include "bitpack.h"
#include "assert.h"
#include "bitprep.h"
#include "quantization.h"
#include "DCT.h"
#include "arith40.h"



const unsigned lsbPr = 0;
const unsigned lsbPb = 4;
const unsigned lsbD = 8;
const unsigned lsbC = 14;
const unsigned lsbB = 20;
const unsigned lsbA = 26;

const unsigned widthIndex = 4;
const unsigned widthBCD = 6;
const unsigned widthA = 6;


/*
* Function name: pack
* Summary: pack is the apply function passed to a mapping function that 
*          packs data from the supplied Pnm_block into a codeword
* Inputs: col and row is the current index in the passed array, 
          array is an array containing Pnm_blocks, block is the 
          current block, cl is the closure and is expected to be null
* Outputs: N/A
* Side Effects: the information in the supplied block is permanently updated
* Error Conditons: N/A no CRE called
*/
void pack(int col, int row, A2Methods_UArray2 array, void *block, void *cl)
{
    (void) col;
    (void) row;
    (void) array;
    (void) cl;
    Pnm_block curr = block;
    
    unsigned pb4bit = Arith40_index_of_chroma(curr -> mpb);
    unsigned pr4bit = Arith40_index_of_chroma(curr -> mpr);
    
    unsigned a = quantA(curr -> a);
    int b = quantChroma(curr -> b);
    int c = quantChroma(curr -> c);
    int d = quantChroma(curr -> d);
    
    curr -> codeword = Bitpack_newu(curr -> codeword, widthIndex, lsbPr, pr4bit);
    curr -> codeword = Bitpack_newu(curr -> codeword, widthIndex, lsbPb, pb4bit);    

    curr -> codeword = Bitpack_newu(curr -> codeword, widthA, lsbA, a);
    curr -> codeword = Bitpack_news(curr -> codeword, widthBCD, lsbB, b);
    curr -> codeword = Bitpack_news(curr -> codeword, widthBCD, lsbC, c);
    curr -> codeword = Bitpack_news(curr -> codeword, widthBCD, lsbD, d);

}


/*
* Function name: packWord
* Summary: this is the function available to other files that include 
*          bitprep.h. it uses the supplied map function and calls
*          pack() on the supplied array. 
* Inputs: array is the array to be mapped through and map is the 
*         desired map function
* Outputs: N/A
* Side Effects: any side effects of the pack function.
* Error Conditons: CRE if provided array or map function are NULL
*/
void packWord(A2Methods_UArray2 array, A2Methods_mapfun *map)
{
    assert(array != NULL && map != NULL);
    map(array, pack, NULL);
}


/*
* Function name: unpack
* Summary: unpack is the apply function passed to a mapping function that 
*          unpacks data from the codeword in the supplied block and stores
*          the a, b, c, d, pb, and pr values in the provided block.
* Inputs: col and row is the current index in the passed array, 
          array is an array containing Pnm_blocks, block is the 
          current block, cl is the closure and is expected to be null
* Outputs: N/A
* Side Effects: the information in the supplied block is permanently updated
* Error Conditons: N/A no CRE called
*/
void unpack(int col, int row, A2Methods_UArray2 array, void *elem, void *cl)
{
    (void) col;
    (void) row;
    (void) array;
    (void) cl;
    Pnm_block curr = elem;

    unsigned prBit = Bitpack_getu(curr -> codeword, widthIndex, lsbPr);
    unsigned pbBit = Bitpack_getu(curr -> codeword, widthIndex, lsbPb);

    float pr = Arith40_chroma_of_index(prBit);
    float pb = Arith40_chroma_of_index(pbBit);

    uint64_t aBit = Bitpack_getu(curr -> codeword, widthA, lsbA);
    
    float a = inQuantA(aBit);
    
    
    int64_t bBit = Bitpack_gets(curr -> codeword, widthBCD, lsbB);
    int64_t cBit = Bitpack_gets(curr -> codeword, widthBCD, lsbC);
    int64_t dBit = Bitpack_gets(curr -> codeword, widthBCD, lsbD);

    float b = inQuantChroma(bBit);
    float c = inQuantChroma(cBit);
    float d = inQuantChroma(dBit);

    curr -> mpb = pb;
    curr -> mpr = pr;
    curr -> a = a;
    curr -> b = b;
    curr -> c = c;
    curr -> d = d;
}

/*
* Function name: unpackWord
* Summary: this is the function available to other files that include 
*          bitprep.h. it uses the supplied map function and calls
*          unpack() on the supplied array. 
* Inputs: array is the array to be mapped through and map is the 
*         desired map function
* Outputs: N/A
* Side Effects: any side effects of the unpack function.
* Error Conditons: CRE if provided array or map function are NULL
*/
void unpackWord(A2Methods_UArray2 array, A2Methods_mapfun *map)
{
    assert(array != NULL && map != NULL);
    map(array, unpack, NULL);
}