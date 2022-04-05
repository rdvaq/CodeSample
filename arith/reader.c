/*
*     reader.h
*     by Matt Zhou(czhou06) and Violet McCabe (vmccab01), 2/28/2022
*     arith
*
*     Summary: Implementation of reader.h.
*/ 

#include <stdio.h>
#include <stdlib.h>

#include "pnm.h"
#include "reader.h"
#include "a2methods.h"
#include "DCT.h"
#include "bitpack.h"
#include "assert.h"

/* code word is 4 byte long*/
const unsigned bytelength = 4;
/* 8 bits in a byte*/
const unsigned bitInbyte = 8;

/*   printbytes
 *   Summary: helper functions that prints the codewords to stdout
 *   in big-endian order
 * 
 *   Expected Input: a void pointer pointing at the start of the value in 
 *   memory
 * 
 *   Error Conditions: CRE is thrown if p is null
 */
void printbytes(void *p)
{
    assert(p != NULL);
    unsigned int i;
    unsigned char *cp = (unsigned char *)p;
    /* move pointer to point at the last byte in memory*/
    cp += bytelength - 1;
    for (i = 0; i < bytelength; i++) {
        putchar(*cp--);
    }
}

/*   printMap
 *   Summary: Mapping function that maps through an array with Pnm_block 
 *   structs and calls printbytes to print the codewords stored at each block
 * 
 *   Expected Input: standard map function inputs. The array here is a UArray2
 *   of Pnm_block structs; block should be pointing at a Pnm_block that 
 *   contains a codeword; Nothing is passed through the closure 
 * 
 *   Error Conditions: CRE is thrown if block is null
 */
void printMap(int col, int row, A2Methods_UArray2 array, void *block, 
                                                                void *nullcl) 
{
    (void) col;
    (void) row;
    (void) array;
    (void) nullcl;

    assert(block != NULL);
    Pnm_block pixel = block;
    unsigned codeword = pixel -> codeword;

    printbytes(&codeword);
}

/*   writeout
 *   Summary: Function responsible for printing the compressed image to stdout;
 *   Prints out header first fllowed by compressed codewords in a row-major 
 *   order
 * 
 *   Expected Input: width and height of the original image, a UArray2 that
 *   contains the compressed codewords, and a specified mapping function(should
 *   be row-maj)
 * 
 *   Error Conditions: CRE is thrown if map or array is NULL
 */
void writeout(int width, int height, A2Methods_UArray2 array, 
                                                        A2Methods_mapfun *map)
{
    assert(array != NULL && map != NULL);
    printf("COMP40 Compressed image format 2\n%u %u", width, height);
    printf("\n");

    /* call printMap to print codewords */
    map(array, printMap, NULL);
}


/*   readCW
 *   Summary: mapping function responsible for parsing the codeword sequences 
 *   out of the file stream and populating a Pnm_block struct with the obtained
 *   code word.
 * 
 *   Expected Input: standard input of a mapping function. The array passed in 
 *   here should be a UArray2 of Pnm_block structs. block should be pointing at
 *   at a Pnm_block in the array, and the closure is the filestream that 
 *   contains the codewords
 * 
 *   Error Conditions: CRE is thrown if block or file is NULL; CRE is also
 *   thrown if the amount of codewords we get does not match the expected 
 *   number of codewords
 */
void readCW(int col, int row, A2Methods_UArray2 array, void *block, void *file)
{
    (void) col;
    (void) row;
    (void) array;

    assert(block != NULL && file != NULL);

    Pnm_block pixel = block;
    FILE *input = file;

    /* initialize the values in the struct*/
    pixel -> a = 0.0;
    pixel -> b = 0.0;
    pixel -> c = 0.0;
    pixel -> d = 0.0;
    pixel -> mpr = 0.0;
    pixel -> mpb = 0.0;
    pixel -> codeword = 0;

    uint32_t codeword = 0;

    for (int i = bytelength - 1; i >= 0; i--) {

        unsigned field = fgetc(input);

        /* check that each codeword is 32 bytes, and we get expected number of 
        codewords */
        assert((int) field != -1);
        
        /*use bitpack funcs to shift codeword back to little endian order*/
        codeword = Bitpack_newu(codeword, bitInbyte, i * bitInbyte, field);
    } 

    pixel -> codeword = codeword;

}

/*   readin
 *   Summary: Function responsible for reading in the compressed image. Returns
 *   a UArray2 that contains Pnm_block structs. Each block struct should be 
 *   populated with a codeword. Also returns the width and height of the 
 *   uncompressed image.
 * 
 *   Expected Input: File stream of the compressed image, a A2methods used for
 *   for initialiing UArray2s, and unsigned pointers to width and height
 *    
 *   Side Effects: width and height updated to the width and height read in 
 *   from the input image
 * 
 *   Error Conditions: CRE is thrown if methods, width and height are NULL;
 *  
 */
A2Methods_UArray2 readin(FILE *input, A2Methods_T methods, unsigned *width, 
                         unsigned *height)
{
    assert(methods != NULL && width != NULL && height != NULL);
    int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                                                                width, height);
    assert(*width % 2 == 0 && *height % 2 == 0);
    assert(read == 2);
    int c = getc(input);
    assert(c == '\n');

    /*declare new array that stores Pnm_blocks*/
    A2Methods_UArray2 array = methods -> new(*width/2, *height/2, 
                                                    sizeof(struct Pnm_block));
    
    /*read codewords and populate*/
    methods -> map_default(array, readCW, input);

    return array;
}

