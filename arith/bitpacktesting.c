
#include <stdio.h>
#include <stdlib.h> 
#include <inttypes.h>
#include <assert.h>
#include "bitpack.h"
#include "quantization.h"


// void printbytes(void *p, unsigned int len)
// {
//    // printf("length: %u \n", len);
//  unsigned int i;
//  unsigned char *cp = (unsigned char *)p;
// cp += 3;
// for (i = 0; i < len; i++) {
//   printf("%02X", *cp--);
// // //  printf("%02X ", *cp);
// }
//  printf("\n");
// } 

int main()
{

    // fprintf(stderr, "quantChroma 3.0, expected: 15, returned: %i \n", quantChroma(3.0));    // fprintf(stderr, "quantChroma -3.0, expected: -15, returned: %i \n", quantChroma(-3.0));
    // fprintf(stderr, "quantChroma 0.156, expected: 8, returned: %i \n", quantChroma(0.156));
    // fprintf(stderr, "quantChroma 0.3, expected: 15, returned: %i \n", quantChroma(0.3));
    // fprintf(stderr, "quantChroma 0.002, expected: 0, returned: %i \n", quantChroma(0.002));
    // fprintf(stderr, "quantChroma 0.02, expected: 1, returned: %i \n", quantChroma(0.02));

    // /* checking inverse and forward returns*/
    // fprintf(stderr, "0.125, expected: 0.125, returned: %f \n", inQuantChroma(quantChroma(0.125)));
    // fprintf(stderr, "0.29, expected: 0.29, returned: %f \n", inQuantChroma(quantChroma(0.29)));

    /* unsigned get tests */

    
//     fprintf(stderr, "0x116, 5, 4 expected: 17 returned: %lu \n", Bitpack_getu(0x116, 5, 4));

//     printf("0x3f4, 6, 2 expected: -3 returned: %" PRId64 "\n", Bitpack_gets(0x3f4, 6, 2));

//     fprintf(stderr, "15, 4 returns: %i \n", Bitpack_fitsu(15,4));
//     fprintf(stderr, "16, 4 returns: %i \n", Bitpack_fitsu(16,4));
//    // fprintf(stderr, "5, 3 returns: %i \n", Bitpack_fitsu(5,65));
//     fprintf(stderr, "-1, 2 returns: %i \n", Bitpack_fitsu(-1,2));
//     fprintf(stderr, "2, 64 returns: %i \n", Bitpack_fitsu(2,64));
//    // uint64_t n = 9223372036854775808;
//     // fprintf(stderr, "9223372036854775808, 63 returns: %i \n", Bitpack_fitsu(n,63));

//     fprintf(stderr, "-8, 4 expected: 1 return: %i \n", Bitpack_fitss(-8, 4));

//     fprintf(stderr, "0xff00, expected 34816 returned %" PRIu64 "\n", Bitpack_newu(0xff00, 8, 8, 0x88));
//     fprintf(stderr, "0xff00, expected 34816 returned %" PRIu64 "\n", Bitpack_newu(0xff00, 8, 8, 0xff));
//     fprintf(stderr, "0xff00, expected 65280  returned %" PRIu64 "\n", Bitpack_news(0xff00, 8, 8, -1));
//     fprintf(stderr, "0x9f90, expecting 8 returned %" PRId64 "\n", Bitpack_gets(Bitpack_news(0x9f80, 8, 6, 8), 8, 6));


// printf("testing subtle law %i \n", (Bitpack_getu(Bitpack_newu(0xff00, 8, 8, 2), 4, 20) == Bitpack_getu(0xff00, 4, 20)));

    // printf("tesinig news with lsb = 0\n");
    // printf("%" PRId64 "\n", Bitpack_newu(0, 8, 0, 0xff));
    // int64_t i = -8;
    // printf("i is %ld\n",i );
    // printf("%" PRId64 "\n", Bitpack_gets(Bitpack_news(0, 4, 0, i), 4, 0));

    
    // for (uint64_t i = 0; i < 65536 ; i++)
    // {
    //     assert(Bitpack_fitsu(i, 16));
    // }
    // unsigned j = 33554432;
    // assert( ! Bitpack_fitsu(j, 25));

    // int lower = -1024;
    // int higher = 1024 - 1 ;
    // for (int64_t k = lower; k <= higher; k++)
    // {
    //     //printf("K is : %ld \n", k);
    //     assert(Bitpack_fitss(k, 11));
    // }

    // assert(Bitpack_fitsu(10000, 64));
    // assert(! Bitpack_fitsu(1000, 0));

    // assert(Bitpack_fitss(-1, 1));
    // assert(Bitpack_fitss(0, 1));
    // assert(! Bitpack_fitss(1, 1));
    // assert(! Bitpack_fitss(2, 2));
    // assert(Bitpack_fitss(1, 2));

   
    // printf("passed fit check\n");
    
    // printf("0xff, 5, 0 expected: 31 returned: %lu \n", Bitpack_getu(0xff, 5, 0));
    // printf("0xff, 5, 0 expected: -1 returned: %ld \n", Bitpack_gets(0xff, 5, 0));
    // printf("0xff, 5, 0 expected: -12 returned: %ld \n", Bitpack_gets(0xf4, 5, 0));
    // printf("0xff, 5, 0 expected: -8 returned: %ld \n", Bitpack_gets(0x86000, 5, 10));
    // printf("0xff, 5, 0 expected: 24 returned: %lu \n", Bitpack_getu(0x86000, 5, 10));

    // printf("passed get test\n");


    // int pos = 258;
    // int neg = (-2);
    // float float12 = 12.0;
    // float floatneg12 = (-12.0);

    // printf("The bytes in memory for signed integer %d are ", pos);
    // printbytes(&pos, sizeof(pos));
    // printf("The bytes in memory for signed integer %d are ", neg);
    // printbytes(&neg, sizeof(neg));
    // printf("The bytes in memory for float %f are ", float12);
    // printbytes(&float12, sizeof(float12));

    // printf("The bytes in memory for float %f are ", floatneg12);
    // printbytes(&floatneg12, sizeof(floatneg12)); 

 }



