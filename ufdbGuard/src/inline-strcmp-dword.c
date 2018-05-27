/* inline-strcmp-dword.c */

#include "ufdb.h"

#include <emmintrin.h>


static __inline__ int __ufdb_strcmp_dword(
   const char * cs, 
   const char * ct )
{
        long __res, rcx = 0, anyR = 16;

	/*
		strcmploop:
	 		movl	eax, (%4)
			movl	ecx, (%5)
			subl    eax, ecx

			jne	strcmpmismatch:
			addq    $4, %4
			addq    $4, %5
							# the dword is equal but we must stop if it has a \0
			tstb    cl, cl
			je      strcmpequal
			tstb    ch, ch
			je      strcmpequal
			bswap   ecx
			tstb    cl, cl
			je      strcmpequal
			tstb    ch, ch
			je      strcmpequal
			jmp     strcmploop

		strcmpmismatch:
		strcmpequal:
	*/
        __asm__(
		"# inlined __ufdb_strcmp_dword      \n"
                ".align 16                          \n"     // 0 filler bytes :)
                "sub        %5, %4                  \n"
                "1:                                 \n"     // loop is 16 opcode bytes :)
                "add        %5, %4                  \n"
                "movdqu     (%4), %2                \n"     // Use any XMM, using register constraint "x"
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

