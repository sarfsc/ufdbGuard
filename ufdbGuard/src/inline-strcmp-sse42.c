/* inline-strcmp-sse42.c */

#include "ufdb.h"

#include <emmintrin.h>


static __inline__ int __ufdb_strcmp_sse42(
   const char * cs, 
   const char * ct )
{
        /* see http://www.strchr.com/strcmp_and_strlen_using_sse_4.2 */
        __m128i anyXmm;
        long __res, rcx = 0, anyR = 16;

        __asm__(
		"# inlined __ufdb_strcmp_sse42      \n"
                "sub        %5, %4                  \n"
                ".align 16                          \n"     // 0 filler bytes :)
                "1:                                 \n"     // loop is 16 opcode bytes :)
                "add        %5, %4                  \n"
                "movdqu     (%4), %2                \n"     // Use any XMM, using register constraint "x"
                "pcmpistri  $0x18, (%4,%0), %2      \n"     // EQUAL_EACH(0x08)+NEGATIVE_POLARITY(0x10) 
                                                            // clears upper bits of RCX
                "ja 1b                              \n"
                "jc 2f                              \n"
                "xor        %0, %0                  \n"
                "jmp 3f                             \n"     // XXX Extra jump could be avoided in pure asm
                "2:                                 \n"
                "add        %4, %0                  \n"
                "movzxb     (%0,%1), %0             \n"     // Note: uses full RCX(%1)
                "movzxb     (%4,%1), %4             \n"
                "sub        %4, %0                  \n"
                "3:                                 \n"
        : "=r"(__res), "+c"(rcx), "=x"(anyXmm) : "0"(cs-ct), "r"(ct), "r"(anyR) );

        return (int) __res;
}

