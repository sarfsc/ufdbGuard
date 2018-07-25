	.file	"ufdbchkport.c"
	.text
	.p2align 4,,15
	.type	hostname2hash, @function
hostname2hash:
.LFB58:
	.cfi_startproc
# BLOCK 2 freq:900 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	movsbl	(%rdi), %edx
	movl	$85, %eax
	testb	%dl, %dl
# SUCC: 3 [91.0%]  (FALLTHRU,CAN_FALLTHRU) 5 [9.0%]  (CAN_FALLTHRU)
	je	.L5
# BLOCK 3 freq:9100 seq:1
# PRED: 3 [91.0%]  (DFS_BACK,CAN_FALLTHRU) 2 [91.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L4:
	sall	$4, %eax
	addq	$1, %rdi
	xorl	%eax, %edx
	leal	-17(%rdx), %eax
	movsbl	(%rdi), %edx
	testb	%dl, %dl
# SUCC: 3 [91.0%]  (DFS_BACK,CAN_FALLTHRU) 4 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L4
# BLOCK 4 freq:819 seq:2
# PRED: 3 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	andl	$2147483647, %eax
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 5 freq:81 seq:3
# PRED: 2 [9.0%]  (CAN_FALLTHRU)
.L5:
# SUCC: EXIT [100.0%] 
	rep ret
	.cfi_endproc
.LFE58:
	.size	hostname2hash, .-hostname2hash
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"   probing for Skype node on %s:%d ..."
	.align 8
.LC1:
	.string	"     socket open failed for %s:%d"
	.align 8
.LC2:
	.string	"      write failed for %s:%d: %s"
	.align 8
.LC3:
	.string	"      skype node probe reply byte %2d, 0x%02x  %c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC4:
	.string	"SSH"
	.section	.rodata.str1.8
	.align 8
.LC5:
	.string	"%s:%d responded with SSH so it has a TUNNEL."
	.align 8
.LC6:
	.string	"      received Skype Node Reply Message from %s:%d"
	.align 8
.LC7:
	.string	"      read failed from %s:%d: n=%d, %s"
	.align 8
.LC8:
	.string	"      got reply of %d bytes to Skype Node probe from %s:%d"
	.align 8
.LC9:
	.string	"      reply byte %2d, 0x%02x  %c"
	.text
	.p2align 4,,15
	.type	detectSkypeSN, @function
detectSkypeSN:
.LFB44:
	.cfi_startproc
# BLOCK 2 freq:5249 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	xorl	%eax, %eax
	movl	$12, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movl	%esi, %r13d
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$160, %rsp
	.cfi_offset 3, -56
	leaq	48(%rsp), %rbx
	movq	%rbx, %rdi
	rep stosq
	movb	$-128, 48(%rsp)
	movb	$70, 49(%rsp)
	movb	$1, 50(%rsp)
	movb	$3, 51(%rsp)
	movb	$1, 52(%rsp)
	movb	$45, 54(%rsp)
	movl	$0, (%rdi)
	movb	$16, 58(%rsp)
	movb	$5, 61(%rsp)
	movb	$4, 64(%rsp)
	movb	$10, 67(%rsp)
	movb	$9, 70(%rsp)
	movb	$100, 73(%rsp)
	movb	$98, 76(%rsp)
	movb	$8, 79(%rsp)
	movb	$3, 82(%rsp)
	movb	$6, 85(%rsp)
	movb	$1, 86(%rsp)
	movb	$-128, 88(%rsp)
	movb	$7, 89(%rsp)
	movb	$-64, 91(%rsp)
	movb	$3, 92(%rsp)
	movb	$-128, 94(%rsp)
	movb	$6, 95(%rsp)
	movb	$64, 97(%rsp)
	movb	$2, 98(%rsp)
	movl	UFDBglobalDebug(%rip), %eax
	movb	$-128, 100(%rsp)
	movb	$4, 101(%rsp)
	movb	$-128, 103(%rsp)
	testl	%eax, %eax
	movb	$22, 104(%rsp)
	movb	$3, 105(%rsp)
	movb	$1, 106(%rsp)
	movb	$6, 109(%rsp)
	movb	$-93, 110(%rsp)
	movb	$102, 111(%rsp)
	movb	$9, 112(%rsp)
	movb	$51, 113(%rsp)
	movb	$93, 114(%rsp)
	movb	$63, 115(%rsp)
	movb	$-14, 116(%rsp)
	movb	$-76, 117(%rsp)
	movb	$-54, 118(%rsp)
	movb	$68, 119(%rsp)
	movb	$22, 16(%rsp)
	movb	$3, 17(%rsp)
	movb	$1, 18(%rsp)
	movb	$0, 19(%rsp)
	movb	$74, 20(%rsp)
	movb	$2, 21(%rsp)
	movb	$0, 22(%rsp)
	movb	$0, 23(%rsp)
	movb	$70, 24(%rsp)
	movb	$3, 25(%rsp)
	movb	$1, 26(%rsp)
# SUCC: 11 [29.0%]  (CAN_FALLTHRU) 3 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L9
# BLOCK 3 freq:3727 seq:1
# PRED: 2 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugSkype(%rip), %eax
	testl	%eax, %eax
# SUCC: 11 [50.0%]  (CAN_FALLTHRU) 4 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L9
# BLOCK 4 freq:5249 seq:2
# PRED: 3 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 11 [100.0%] 
.L10:
	movl	%r13d, %esi
	movq	%r12, %rdi
	call	UFDBopenSocket
	testl	%eax, %eax
	movl	%eax, %r14d
# SUCC: 12 [47.5%]  (CAN_FALLTHRU) 5 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L38
# BLOCK 5 freq:2755 seq:3
# PRED: 4 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$16, %r8d
	movl	$20, %edx
	movl	$1, %esi
	movq	$3, 32(%rsp)
	leaq	32(%rsp), %rcx
	movl	%eax, %edi
	movq	$0, 40(%rsp)
	call	setsockopt
	movl	$16, %r8d
	movl	$21, %edx
	leaq	32(%rsp), %rcx
	movl	$1, %esi
	movl	%r14d, %edi
	movq	$3, 32(%rsp)
	movq	$0, 40(%rsp)
	call	setsockopt
	movl	$72, %edx
	movq	%rbx, %rsi
	movl	%r14d, %edi
	call	write
	cmpq	$72, %rax
# SUCC: 6 [92.7%]  (FALLTHRU,CAN_FALLTHRU) 14 [7.3%]  (CAN_FALLTHRU)
	je	.L13
# BLOCK 6 freq:2555 seq:4
# PRED: 5 [92.7%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %r15d
	testl	%r15d, %r15d
# SUCC: 8 [29.0%]  (CAN_FALLTHRU) 7 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L14
# BLOCK 7 freq:1814 seq:5
# PRED: 6 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugSkype(%rip), %ebx
	testl	%ebx, %ebx
# SUCC: 8 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 9 [39.0%]  (CAN_FALLTHRU)
	je	.L15
# BLOCK 8 freq:1847 seq:6
# PRED: 7 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [29.0%]  (CAN_FALLTHRU)
.L14:
	call	__errno_location
	movl	(%rax), %edi
	call	strerror
	movl	%r13d, %edx
	movq	%r12, %rsi
	movq	%rax, %rcx
	movl	$.LC2, %edi
	xorl	%eax, %eax
# SUCC: 9 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 9 freq:2555 seq:7
# PRED: 7 [39.0%]  (CAN_FALLTHRU) 8 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L15:
	movl	%r14d, %edi
	movl	$4, %ebx
# SUCC: 10 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	close
# BLOCK 10 freq:10498 seq:8
# PRED: 25 [100.0%]  12 [71.0%]  (CAN_FALLTHRU) 27 [100.0%]  9 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 13 [100.0%]  24 [50.0%]  (CAN_FALLTHRU) 36 [100.0%]  38 [71.0%]  (CAN_FALLTHRU) 29 [100.0%]  39 [100.0%] 
.L36:
	leaq	-40(%rbp), %rsp
	movl	%ebx, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 11 freq:3386 seq:9
# PRED: 3 [50.0%]  (CAN_FALLTHRU) 2 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L9:
	.cfi_restore_state
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC0, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 4 [100.0%] 
	jmp	.L10
# BLOCK 12 freq:2494 seq:10
# PRED: 4 [47.5%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L38:
	movl	UFDBglobalDebugSkype(%rip), %eax
	movl	$8, %ebx
	testl	%eax, %eax
# SUCC: 13 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 10 [71.0%]  (CAN_FALLTHRU)
	je	.L36
# BLOCK 13 freq:723 seq:11
# PRED: 12 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC1, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L36
# BLOCK 14 freq:200 seq:12
# PRED: 5 [7.3%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L13:
	movl	$50, %edx
	movq	%rbx, %rsi
	movl	%r14d, %edi
	call	read
	movl	%r14d, %edi
	movq	%rax, %r15
	movl	%eax, 12(%rsp)
	call	close
	movl	UFDBglobalDebugSkype(%rip), %r11d
	testl	%r11d, %r11d
# SUCC: 15 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 20 [50.0%]  (CAN_FALLTHRU)
	je	.L16
# BLOCK 15 freq:100 seq:13
# PRED: 14 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %r10d
	testl	%r10d, %r10d
# SUCC: 16 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 17 [50.0%]  (CAN_FALLTHRU)
	jne	.L17
# BLOCK 16 freq:50 seq:14
# PRED: 15 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$20, %esi
	cmpl	$20, %r15d
	movl	%r15d, %eax
	cmovg	%esi, %eax
# SUCC: 17 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%eax, 12(%rsp)
# BLOCK 17 freq:100 seq:15
# PRED: 15 [50.0%]  (CAN_FALLTHRU) 16 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L17:
	movl	12(%rsp), %eax
	testl	%eax, %eax
# SUCC: 18 [99.0%]  (FALLTHRU,CAN_FALLTHRU) 26 [1.0%]  (CAN_FALLTHRU)
	jle	.L18
# BLOCK 18 freq:99 seq:16
# PRED: 17 [99.0%]  (FALLTHRU,CAN_FALLTHRU)
	leal	-1(%rax), %r14d
	xorl	%r15d, %r15d
# SUCC: 19 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	addq	$1, %r14
# BLOCK 19 freq:9900 seq:17
# PRED: 19 [99.0%]  (DFS_BACK,CAN_FALLTHRU) 18 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L19:
	movzbl	(%rbx,%r15), %edx
	movl	%r15d, %esi
	xorl	%eax, %eax
	movl	$.LC3, %edi
	addq	$1, %r15
	movl	%edx, %ecx
	andl	$95, %ecx
	orl	$32, %ecx
	call	ufdbLogMessage
	cmpq	%r14, %r15
# SUCC: 19 [99.0%]  (DFS_BACK,CAN_FALLTHRU) 20 [1.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L19
# BLOCK 20 freq:200 seq:18
# PRED: 14 [50.0%]  (CAN_FALLTHRU) 19 [1.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
.L16:
	movl	$.LC4, %edi
	movl	$3, %ecx
	movq	%rbx, %rsi
	repz cmpsb
# SUCC: 27 [45.3%]  (CAN_FALLTHRU) 21 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L28
# BLOCK 21 freq:109 seq:19
# PRED: 20 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$10, 12(%rsp)
# SUCC: 22 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 37 [50.0%]  (CAN_FALLTHRU)
	jle	.L21
# BLOCK 22 freq:55 seq:20
# PRED: 21 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	16(%rsp), %rsi
	movl	$11, %edx
	movq	%rbx, %rdi
	call	memcmp
	testl	%eax, %eax
# SUCC: 23 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 28 [50.0%]  (CAN_FALLTHRU)
	jne	.L22
# BLOCK 23 freq:27 seq:21
# PRED: 22 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %r9d
	testl	%r9d, %r9d
# SUCC: 25 [45.3%]  (CAN_FALLTHRU) 24 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L23
# BLOCK 24 freq:15 seq:22
# PRED: 23 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugSkype(%rip), %r8d
	movl	$17, %ebx
	testl	%r8d, %r8d
# SUCC: 25 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 10 [50.0%]  (CAN_FALLTHRU)
	je	.L36
# BLOCK 25 freq:20 seq:23
# PRED: 24 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 23 [45.3%]  (CAN_FALLTHRU)
.L23:
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC6, %edi
	xorl	%eax, %eax
	movl	$17, %ebx
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L36
# BLOCK 26 seq:24
# PRED: 17 [1.0%]  (CAN_FALLTHRU)
.L18:
	movl	$3, %edx
	movl	$.LC4, %esi
	movq	%rbx, %rdi
	call	strncmp
	testl	%eax, %eax
# SUCC: 27 [45.3%]  (FALLTHRU,CAN_FALLTHRU) 37 [54.7%]  (CAN_FALLTHRU)
	jne	.L21
# BLOCK 27 freq:91 seq:25
# PRED: 20 [45.3%]  (CAN_FALLTHRU) 26 [45.3%]  (FALLTHRU,CAN_FALLTHRU)
.L28:
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC5, %edi
	xorl	%eax, %eax
	movl	$11, %ebx
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L36
# BLOCK 28 freq:60 seq:26
# PRED: 37 [59.0%]  (CAN_FALLTHRU) 22 [50.0%]  (CAN_FALLTHRU)
.L22:
	movl	UFDBglobalDebugSkype(%rip), %ecx
	testl	%ecx, %ecx
# SUCC: 30 [29.0%]  (CAN_FALLTHRU) 29 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L24
# BLOCK 29 freq:43 seq:27
# PRED: 28 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 30 (CAN_FALLTHRU) 33 [1.0%]  (CAN_FALLTHRU)
.L25:
	xorl	%ebx, %ebx
# SUCC: 10 [100.0%] 
	jmp	.L36
# BLOCK 30 freq:17 seq:28
# PRED: 28 [29.0%]  (CAN_FALLTHRU)
.L24:
	movl	12(%rsp), %esi
	movq	%r12, %rdx
	xorl	%eax, %eax
	movl	%r13d, %ecx
	movl	$.LC8, %edi
	call	ufdbLogMessage
	movl	UFDBglobalDebugSkype(%rip), %edx
	testl	%edx, %edx
# SUCC: 31 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 29 (CAN_FALLTHRU)
	je	.L25
# BLOCK 31 freq:37 seq:29
# PRED: 30 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %eax
	testl	%eax, %eax
# SUCC: 32 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 33 [50.0%]  (CAN_FALLTHRU)
	jne	.L26
# BLOCK 32 freq:18 seq:30
# PRED: 31 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	12(%rsp), %esi
	movl	$20, %eax
	cmpl	$20, %esi
	cmovle	%esi, %eax
# SUCC: 33 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%eax, 12(%rsp)
# BLOCK 33 freq:37 seq:31
# PRED: 31 [50.0%]  (CAN_FALLTHRU) 32 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L26:
	movl	12(%rsp), %eax
	testl	%eax, %eax
# SUCC: 34 [99.0%]  (FALLTHRU,CAN_FALLTHRU) 29 [1.0%]  (CAN_FALLTHRU)
	jle	.L25
# BLOCK 34 freq:37 seq:32
# PRED: 33 [99.0%]  (FALLTHRU,CAN_FALLTHRU)
	leal	-1(%rax), %r13d
	xorl	%r12d, %r12d
# SUCC: 35 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	addq	$1, %r13
# BLOCK 35 freq:3615 seq:33
# PRED: 35 [99.0%]  (DFS_BACK,CAN_FALLTHRU) 34 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L27:
	movzbl	(%rbx,%r12), %edx
	movl	%r12d, %esi
	xorl	%eax, %eax
	movl	$.LC9, %edi
	addq	$1, %r12
	movl	%edx, %ecx
	andl	$95, %ecx
	orl	$32, %ecx
	call	ufdbLogMessage
	cmpq	%r13, %r12
# SUCC: 35 [99.0%]  (DFS_BACK,CAN_FALLTHRU) 36 [1.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L27
# BLOCK 36 freq:36 seq:34
# PRED: 35 [1.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	xorl	%ebx, %ebx
# SUCC: 10 [100.0%] 
	jmp	.L36
# BLOCK 37 freq:54 seq:35
# PRED: 21 [50.0%]  (CAN_FALLTHRU) 26 [54.7%]  (CAN_FALLTHRU)
.L21:
	movl	12(%rsp), %edi
	testl	%edi, %edi
# SUCC: 38 [41.0%]  (FALLTHRU,CAN_FALLTHRU) 28 [59.0%]  (CAN_FALLTHRU)
	jns	.L22
# BLOCK 38 freq:22 seq:36
# PRED: 37 [41.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugSkype(%rip), %esi
	movl	$4, %ebx
	testl	%esi, %esi
# SUCC: 39 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 10 [71.0%]  (CAN_FALLTHRU)
	je	.L36
# BLOCK 39 freq:6 seq:37
# PRED: 38 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	__errno_location
	movl	(%rax), %edi
	call	strerror
	movl	12(%rsp), %ecx
	movl	%r13d, %edx
	movq	%rax, %r8
	movq	%r12, %rsi
	movl	$.LC7, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L36
	.cfi_endproc
.LFE44:
	.size	detectSkypeSN, .-detectSkypeSN
	.p2align 4,,15
	.type	ufdb_ssl_dyn_create, @function
ufdb_ssl_dyn_create:
.LFB63:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$40, %edi
	call	ufdbMalloc
	xorl	%esi, %esi
	movq	%rax, %rbx
	movq	%rax, %rdi
	call	pthread_mutex_init
	movq	%rbx, %rax
	popq	%rbx
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE63:
	.size	ufdb_ssl_dyn_create, .-ufdb_ssl_dyn_create
	.section	.rodata.str1.8
	.align 8
.LC10:
	.string	"ufdb_ssl_dyn_lock: no LOCK|UNLOCK in mode"
	.text
	.p2align 4,,15
	.type	ufdb_ssl_dyn_lock, @function
ufdb_ssl_dyn_lock:
.LFB64:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	testb	$1, %dil
# SUCC: 6 [20.7%]  (CAN_FALLTHRU) 3 [79.3%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L44
# BLOCK 3 freq:7929 seq:1
# PRED: 2 [79.3%]  (FALLTHRU,CAN_FALLTHRU)
	andl	$2, %edi
# SUCC: 5 [39.0%]  (CAN_FALLTHRU) 4 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L45
# BLOCK 4 freq:4837 seq:2
# PRED: 3 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC10, %edi
	xorl	%eax, %eax
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	ufdbLogError
# BLOCK 5 freq:3092 seq:3
# PRED: 3 [39.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L45:
	movq	%rsi, %rdi
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	pthread_mutex_unlock
# BLOCK 6 freq:2071 seq:4
# PRED: 2 [20.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L44:
	movq	%rsi, %rdi
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	pthread_mutex_lock
	.cfi_endproc
.LFE64:
	.size	ufdb_ssl_dyn_lock, .-ufdb_ssl_dyn_lock
	.p2align 4,,15
	.type	ufdb_ssl_dyn_destroy, @function
ufdb_ssl_dyn_destroy:
.LFB65:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	pthread_mutex_destroy
	.cfi_endproc
.LFE65:
	.size	ufdb_ssl_dyn_destroy, .-ufdb_ssl_dyn_destroy
	.p2align 4,,15
	.type	ufdb_pthread_id_callback, @function
ufdb_pthread_id_callback:
.LFB62:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	pthread_self
	.cfi_endproc
.LFE62:
	.size	ufdb_pthread_id_callback, .-ufdb_pthread_id_callback
	.section	.rodata.str1.1
.LC11:
	.string	"mystrcmp: a and/or b is NULL"
	.text
	.p2align 4,,15
	.type	mystrcmp, @function
mystrcmp:
.LFB59:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	testq	%rdi, %rdi
# SUCC: 5 [20.7%]  (CAN_FALLTHRU) 3 [79.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L52
# BLOCK 3 freq:7929 seq:1
# PRED: 2 [79.3%]  (FALLTHRU,CAN_FALLTHRU)
	testq	%rsi, %rsi
# SUCC: 4 [79.3%]  (FALLTHRU,CAN_FALLTHRU) 5 [20.7%]  (CAN_FALLTHRU)
	je	.L52
# BLOCK 4 freq:7929 seq:2
# PRED: 3 [79.3%]  (FALLTHRU,CAN_FALLTHRU)
	call	strcmp
	testl	%eax, %eax
	sete	%al
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	movzbl	%al, %eax
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 5 freq:2071 seq:3
# PRED: 3 [20.7%]  (CAN_FALLTHRU) 2 [20.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L52:
	.cfi_restore_state
	xorl	%eax, %eax
	movl	$.LC11, %edi
	call	ufdbLogError
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE59:
	.size	mystrcmp, .-mystrcmp
	.p2align 4,,15
	.type	matchHostname, @function
matchHostname:
.LFB80:
	.cfi_startproc
# BLOCK 2 freq:873 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rsi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	call	__ctype_tolower_loc
	movq	(%rax), %r12
# SUCC: 6 [100.0%] 
	jmp	.L56
# BLOCK 3 freq:9700 seq:1
# PRED: 6 [97.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L64:
	cmpb	$42, %al
# SUCC: 10 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 4 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L76
# BLOCK 4 freq:9409 seq:2
# PRED: 3 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	movsbq	(%rbx), %rdx
	movsbl	%al, %eax
	cmpl	(%r12,%rdx,4), %eax
# SUCC: 9 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 5 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L67
# BLOCK 5 freq:9127 seq:3
# PRED: 4 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	addq	$1, %rbp
# SUCC: 6 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU)
	addq	$1, %rbx
# BLOCK 6 freq:10000 seq:4
# PRED: 5 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 2 [100.0%] 
.L56:
	movsbq	0(%rbp), %rax
	movl	(%r12,%rax,4), %eax
	testb	%al, %al
# SUCC: 3 [97.0%]  (CAN_FALLTHRU) 7 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L64
# BLOCK 7 freq:300 seq:5
# PRED: 6 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	xorl	%eax, %eax
	cmpb	$0, (%rbx)
# SUCC: 8 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	sete	%al
# BLOCK 8 freq:318 seq:6
# PRED: 7 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 18 [100.0%] 
.L74:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 9 freq:416 seq:7
# PRED: 4 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 14 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L67:
	.cfi_restore_state
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 10 freq:291 seq:8
# PRED: 3 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L76:
	.cfi_restore_state
	movsbq	1(%rbp), %rax
	leaq	1(%rbp), %r13
	movl	(%r12,%rax,4), %eax
	cmpb	$42, %al
	movl	%eax, %r15d
# SUCC: 11 [91.0%]  (FALLTHRU,CAN_FALLTHRU) 12 [9.0%]  (CAN_FALLTHRU)
	jne	.L58
# BLOCK 11 freq:2942 seq:9
# PRED: 11 [91.0%]  (DFS_BACK,CAN_FALLTHRU) 10 [91.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L59:
	addq	$1, %r13
	movsbq	0(%r13), %rax
	movl	(%r12,%rax,4), %eax
	cmpb	$42, %al
	movl	%eax, %r15d
# SUCC: 11 [91.0%]  (DFS_BACK,CAN_FALLTHRU) 12 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	je	.L59
# BLOCK 12 freq:291 seq:10
# PRED: 11 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 10 [9.0%]  (CAN_FALLTHRU)
.L58:
	movzbl	(%rbx), %ebp
	testb	%bpl, %bpl
# SUCC: 13 [97.0%]  (FALLTHRU,CAN_FALLTHRU) 19 [3.0%]  (CAN_FALLTHRU)
	je	.L60
# BLOCK 13 freq:282 seq:11
# PRED: 12 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	movsbl	%r15b, %r14d
# SUCC: 16 [100.0%] 
	jmp	.L63
# BLOCK 14 freq:4464 seq:12
# PRED: 17 [97.0%]  (CAN_FALLTHRU) 16 [86.3%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L61:
	cmpb	$46, %bpl
# SUCC: 9 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 15 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L67
# BLOCK 15 freq:4330 seq:13
# PRED: 14 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	addq	$1, %rbx
	movzbl	(%rbx), %ebp
	testb	%bpl, %bpl
# SUCC: 16 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 19 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L60
# BLOCK 16 freq:4483 seq:14
# PRED: 15 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 13 [100.0%] 
.L63:
	movsbq	%bpl, %rax
	cmpl	%r14d, (%r12,%rax,4)
# SUCC: 17 [13.7%]  (FALLTHRU,CAN_FALLTHRU) 14 [86.3%]  (CAN_FALLTHRU)
	jne	.L61
# BLOCK 17 freq:615 seq:15
# PRED: 16 [13.7%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rsi
	movq	%r13, %rdi
	call	matchHostname
	testl	%eax, %eax
# SUCC: 18 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 14 [97.0%]  (CAN_FALLTHRU)
	je	.L61
# BLOCK 18 freq:18 seq:16
# PRED: 17 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	movl	$1, %eax
# SUCC: 8 [100.0%] 
	jmp	.L74
# BLOCK 19 freq:139 seq:17
# PRED: 15 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 12 [3.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L60:
	xorl	%eax, %eax
	testb	%r15b, %r15b
	sete	%al
	addq	$8, %rsp
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE80:
	.size	matchHostname, .-matchHostname
	.section	.rodata.str1.8
	.align 8
.LC12:
	.string	"CRYPTO lock type %d out of range (max=%d)  *****"
	.align 8
.LC13:
	.string	"ufdb_pthread_locking_callback: no LOCK|UNLOCK for type %d"
	.text
	.p2align 4,,15
	.type	ufdb_pthread_locking_callback, @function
ufdb_pthread_locking_callback:
.LFB61:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	testl	%esi, %esi
	movl	num_static_ssl_locks(%rip), %edx
# SUCC: 7 [8.8%]  (CAN_FALLTHRU) 3 [91.2%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L79
# BLOCK 3 freq:9119 seq:1
# PRED: 2 [91.2%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	%edx, %esi
# SUCC: 7 [39.0%]  (CAN_FALLTHRU) 4 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L79
# BLOCK 4 freq:5563 seq:2
# PRED: 3 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	testb	$1, %dil
# SUCC: 8 [20.7%]  (CAN_FALLTHRU) 5 [79.3%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L83
# BLOCK 5 freq:4411 seq:3
# PRED: 4 [79.3%]  (FALLTHRU,CAN_FALLTHRU)
	andl	$2, %edi
	.p2align 4,,2
# SUCC: 9 [39.0%]  (CAN_FALLTHRU) 6 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L84
# BLOCK 6 freq:2690 seq:4
# PRED: 5 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC13, %edi
	xorl	%eax, %eax
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	ufdbLogError
# BLOCK 7 freq:4437 seq:5
# PRED: 3 [39.0%]  (CAN_FALLTHRU) 2 [8.8%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L79:
	movl	$.LC12, %edi
	xorl	%eax, %eax
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	ufdbLogMessage
# BLOCK 8 freq:1152 seq:6
# PRED: 4 [20.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L83:
	movq	crypto_mutexes(%rip), %rax
	movslq	%esi, %rsi
	leaq	(%rsi,%rsi,4), %rdx
	leaq	(%rax,%rdx,8), %rdi
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	pthread_mutex_lock
# BLOCK 9 freq:1720 seq:7
# PRED: 5 [39.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L84:
	movq	crypto_mutexes(%rip), %rax
	movslq	%esi, %rsi
	leaq	(%rsi,%rsi,4), %rdx
	leaq	(%rax,%rdx,8), %rdi
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	pthread_mutex_unlock
	.cfi_endproc
.LFE61:
	.size	ufdb_pthread_locking_callback, .-ufdb_pthread_locking_callback
	.section	.rodata.str1.1
.LC14:
	.string	"      key %s purged"
	.text
	.p2align 4,,15
	.type	purgeHTTPSinfo, @function
purgeHTTPSinfo:
.LFB56:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	movq	OldestInHTTPScache(%rip), %rdx
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	xorl	%eax, %eax
	movq	%rsi, %rbx
	cmpq	%rdx, 8(%rsi)
# SUCC: 3 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [39.0%]  (CAN_FALLTHRU)
	jge	.L89
# BLOCK 3 freq:6100 seq:1
# PRED: 2 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 4 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 5 [47.5%]  (CAN_FALLTHRU)
	jle	.L87
# BLOCK 4 freq:3201 seq:2
# PRED: 3 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdi, %rsi
	movl	$.LC14, %edi
# SUCC: 5 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 5 freq:6100 seq:3
# PRED: 3 [47.5%]  (CAN_FALLTHRU) 4 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L87:
	movq	16(%rbx), %rdi
	call	ufdbFree
	movl	$1, %eax
# SUCC: 6 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
# BLOCK 6 freq:10000 seq:4
# PRED: 5 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 2 [39.0%]  (CAN_FALLTHRU)
.L89:
	popq	%rbx
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE56:
	.size	purgeHTTPSinfo, .-purgeHTTPSinfo
	.section	.rodata.str1.1
.LC15:
	.string	"not "
.LC16:
	.string	""
	.section	.rodata.str1.8
	.align 8
.LC17:
	.string	"waitForTLSinitilisation: tls_inited is %d  ssl_ctx is %sNULL ..."
	.align 8
.LC18:
	.string	"waitForTLSinitilisation: after waiting: tls_inited is %d, ssl_ctx is NULL  *****\nNo HTTPS connections can be set up yet."
	.text
	.p2align 4,,15
	.type	waitForTLSinitilisation, @function
waitForTLSinitilisation:
.LFB69:
	.cfi_startproc
# BLOCK 2 freq:2098 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	movl	didwait.21603(%rip), %eax
	testl	%eax, %eax
# SUCC: 12 [39.0%]  (CAN_FALLTHRU) 3 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L109
# BLOCK 3 freq:1280 seq:1
# PRED: 2 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$1, UFDBglobalDebug(%rip)
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
# SUCC: 4 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 5 [27.0%]  (CAN_FALLTHRU)
	jle	.L94
# BLOCK 4 freq:934 seq:2
# PRED: 3 [73.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	tls_inited(%rip), %esi
	movl	$.LC16, %eax
	movl	$.LC15, %edx
	cmpq	$0, ssl_ctx(%rip)
	movl	$.LC17, %edi
	cmove	%rax, %rdx
	xorl	%eax, %eax
# SUCC: 5 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 5 freq:1280 seq:3
# PRED: 3 [27.0%]  (CAN_FALLTHRU) 4 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L94:
	movl	tls_inited(%rip), %eax
	cmpl	$2, %eax
# SUCC: 14 [39.0%]  (CAN_FALLTHRU) 6 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L98
# BLOCK 6 freq:781 seq:4
# PRED: 5 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	tls_inited(%rip), %eax
	cmpl	$2, %eax
# SUCC: 13 [28.0%]  (CAN_FALLTHRU) 7 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L110
# BLOCK 7 freq:758 seq:5
# PRED: 13 [89.9%]  (CAN_FALLTHRU) 6 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
.L97:
	movl	$30, %ebx
# SUCC: 9 [100.0%] 
	jmp	.L100
# BLOCK 8 freq:9242 seq:6
# PRED: 9 [95.5%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L111:
	subl	$1, %ebx
# SUCC: 9 [96.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 10 [3.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L99
# BLOCK 9 freq:9677 seq:7
# PRED: 8 [96.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 7 [100.0%] 
.L100:
	movl	$1, %edi
	call	sleep
	movl	tls_inited(%rip), %eax
	cmpl	$2, %eax
# SUCC: 10 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 8 [95.5%]  (CAN_FALLTHRU)
	jle	.L111
# BLOCK 10 freq:758 seq:8
# PRED: 8 [3.5%]  (CAN_FALLTHRU,LOOP_EXIT) 9 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
.L99:
	cmpq	$0, ssl_ctx(%rip)
	movl	$1, didwait.21603(%rip)
# SUCC: 15 [10.1%]  (CAN_FALLTHRU) 11 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L112
# BLOCK 11 freq:682 seq:9
# PRED: 10 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	popq	%rbx
	.cfi_restore 3
# SUCC: 12 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	.cfi_def_cfa_offset 8
# BLOCK 12 freq:1203 seq:10
# PRED: 11 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 2 [39.0%]  (CAN_FALLTHRU) 14 [100.0%] 
.L109:
# SUCC: EXIT [100.0%] 
	rep ret
# BLOCK 13 freq:219 seq:11
# PRED: 6 [28.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L110:
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	cmpq	$0, ssl_ctx(%rip)
# SUCC: 14 [10.1%]  (FALLTHRU,CAN_FALLTHRU) 7 [89.9%]  (CAN_FALLTHRU)
	jne	.L97
# BLOCK 14 freq:521 seq:12
# PRED: 13 [10.1%]  (FALLTHRU,CAN_FALLTHRU) 5 [39.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L98:
	movl	$1, didwait.21603(%rip)
	popq	%rbx
	.cfi_remember_state
	.cfi_restore 3
	.cfi_def_cfa_offset 8
# SUCC: 12 [100.0%] 
	jmp	.L109
# BLOCK 15 freq:77 seq:13
# PRED: 10 [10.1%]  (CAN_FALLTHRU)
.L112:
	.cfi_restore_state
	movl	tls_inited(%rip), %esi
	movl	$.LC18, %edi
	xorl	%eax, %eax
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	ufdbLogError
	.cfi_endproc
.LFE69:
	.size	waitForTLSinitilisation, .-waitForTLSinitilisation
	.section	.rodata.str1.1
.LC19:
	.string	"%s:%d"
	.section	.rodata.str1.8
	.align 8
.LC20:
	.string	"      insertHTTPScache: HTTPS cache %08lx was purged: %d of %d entries were removed"
	.text
	.p2align 4,,15
	.type	insertHTTPScache.constprop.5, @function
insertHTTPScache.constprop.5:
.LFB94:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	movl	%esi, %ecx
	movq	%rdx, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rdi, %rdx
	movl	$.LC19, %esi
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	xorl	%eax, %eax
	subq	$1040, %rsp
	.cfi_def_cfa_offset 1072
	movq	%rsp, %rdi
	call	sprintf
	xorl	%edi, %edi
	call	time
	movl	$1112, %edi
	movq	%rax, %rbp
	call	ufdbMalloc
	movq	myht(%rip), %rdi
	movb	$0, 24(%rax)
	movq	%rax, %rbx
	movb	$0, 88(%rax)
	movq	%rbp, 8(%rax)
	movl	$31, (%rax)
	movq	$0, 16(%rax)
	movq	%rax, (%r12)
	movl	4(%rdi), %r12d
	movq	lastHTTPScachePurgeTime(%rip), %rax
	cmpl	$15000, %r12d
# SUCC: 3 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [50.0%]  (CAN_FALLTHRU)
	jbe	.L115
# BLOCK 3 freq:5000 seq:1
# PRED: 2 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	3600(%rax), %rdx
	cmpq	%rdx, %rbp
# SUCC: 4 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [50.0%]  (CAN_FALLTHRU)
	jle	.L115
# BLOCK 4 freq:7075 seq:2
# PRED: 6 [61.0%]  (CAN_FALLTHRU) 3 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
.L116:
	cmpl	$15001, %r12d
	movl	$purgeHTTPSinfo, %esi
	leaq	-10800(%rbp), %rdx
	leaq	-7200(%rbp), %rax
	cmovl	%rdx, %rax
	movq	%rax, OldestInHTTPScache(%rip)
	call	UFDBpurgeHashtable
	movl	UFDBglobalDebug(%rip), %eax
	movq	%rbp, lastHTTPScachePurgeTime(%rip)
	testl	%eax, %eax
# SUCC: 8 [39.0%]  (CAN_FALLTHRU) 5 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L122
# BLOCK 5 freq:10000 seq:3
# PRED: 4 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 7 [100.0%]  8 [100.0%] 
.L117:
	movq	%rsp, %rdi
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%rbx, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
	addq	$1040, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 6 freq:7500 seq:4
# PRED: 3 [50.0%]  (CAN_FALLTHRU) 2 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L115:
	.cfi_restore_state
	addq	$10800, %rax
	cmpq	%rax, %rbp
# SUCC: 4 [61.0%]  (CAN_FALLTHRU) 7 [39.0%]  (FALLTHRU)
	jg	.L116
# BLOCK 7 freq:2925 seq:5
# PRED: 6 [39.0%]  (FALLTHRU)
# SUCC: 5 [100.0%] 
	jmp	.L117
# BLOCK 8 freq:2758 seq:6
# PRED: 4 [39.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L122:
	movq	myht(%rip), %rsi
	movl	%r12d, %edx
	movl	%r12d, %ecx
	movl	$.LC20, %edi
	xorl	%eax, %eax
	subl	4(%rsi), %edx
	call	ufdbLogMessage
# SUCC: 5 [100.0%] 
	jmp	.L117
	.cfi_endproc
.LFE94:
	.size	insertHTTPScache.constprop.5, .-insertHTTPScache.constprop.5
	.section	.rodata.str1.1
.LC21:
	.string	"unknown"
	.section	.rodata.str1.8
	.align 8
.LC22:
	.string	"   probing for Gtalk on %s:%d ..."
	.align 8
.LC23:
	.string	"      write to %s:%d failed: %s"
	.align 8
.LC24:
	.string	"      received Gtalk Reply Message from %s:%d"
	.align 8
.LC25:
	.string	"received a TLS protocol alert %d (%s) from %s:%d "
	.align 8
.LC26:
	.string	"      read from %s:%d failed: n=%d, %s"
	.align 8
.LC27:
	.string	"      Got reply of %d bytes to Gtalk probe message from %s:%d."
	.section	.rodata.str1.1
.LC28:
	.string	"HTTP/"
	.section	.rodata.str1.8
	.align 8
.LC29:
	.string	"received a plain HTTP message from %s:%d"
	.section	.rodata.str1.1
.LC30:
	.string	"<?xml"
.LC31:
	.string	"<stream:stream"
.LC32:
	.string	"from=\"chat.facebook.com"
	.section	.rodata.str1.8
	.align 8
.LC33:
	.string	"received Facebook Chat Reply Message from %s:%d"
	.align 8
.LC34:
	.string	"      gtalk probe reply byte %2d, 0x%02x  %c"
	.text
	.p2align 4,,15
	.globl	UFDBdetectGtalk
	.type	UFDBdetectGtalk, @function
UFDBdetectGtalk:
.LFB46:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movl	%esi, %r13d
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$512, %rsp
	.cfi_offset 3, -56
	movb	$-128, 96(%rsp)
	movb	$70, 97(%rsp)
	movb	$1, 98(%rsp)
	movb	$3, 99(%rsp)
	movb	$1, 100(%rsp)
	movb	$0, 101(%rsp)
	movb	$45, 102(%rsp)
	movb	$0, 103(%rsp)
	movb	$0, 104(%rsp)
	movb	$0, 105(%rsp)
	movb	$16, 106(%rsp)
	movb	$1, 107(%rsp)
	movb	$0, 108(%rsp)
	movb	$-128, 109(%rsp)
	movb	$3, 110(%rsp)
	movb	$0, 111(%rsp)
	movb	$-128, 112(%rsp)
	movb	$7, 113(%rsp)
	movb	$0, 114(%rsp)
	movb	$-64, 115(%rsp)
	movb	$6, 116(%rsp)
	movb	$0, 117(%rsp)
	movb	$64, 118(%rsp)
	movb	$2, 119(%rsp)
	movb	$0, 120(%rsp)
	movb	$-128, 121(%rsp)
	movb	$4, 122(%rsp)
	movb	$0, 123(%rsp)
	movb	$-128, 124(%rsp)
	movb	$0, 125(%rsp)
	movb	$0, 126(%rsp)
	movb	$4, 127(%rsp)
	movb	$0, 128(%rsp)
	movb	$-2, 129(%rsp)
	movb	$-1, 130(%rsp)
	movb	$0, 131(%rsp)
	movb	$0, 132(%rsp)
	movb	$10, 133(%rsp)
	movb	$0, 134(%rsp)
	movb	$-2, 135(%rsp)
	movb	$-2, 136(%rsp)
	movb	$0, 137(%rsp)
	movb	$0, 138(%rsp)
	movb	$9, 139(%rsp)
	movb	$0, 140(%rsp)
	movb	$0, 141(%rsp)
	movb	$100, 142(%rsp)
	movb	$0, 143(%rsp)
	movb	$0, 144(%rsp)
	movb	$98, 145(%rsp)
	movb	$0, 146(%rsp)
	movb	$0, 147(%rsp)
	movb	$3, 148(%rsp)
	movb	$0, 149(%rsp)
	movb	$0, 150(%rsp)
	movb	$6, 151(%rsp)
	movb	$31, 152(%rsp)
	movb	$23, 153(%rsp)
	movb	$12, 154(%rsp)
	movb	$-90, 155(%rsp)
	movb	$47, 156(%rsp)
	movb	$0, 157(%rsp)
	movb	$120, 158(%rsp)
	movb	$-4, 159(%rsp)
	movb	$70, 160(%rsp)
	movb	$85, 161(%rsp)
	movb	$46, 162(%rsp)
	movb	$-79, 163(%rsp)
	movb	$-125, 164(%rsp)
	movb	$57, 165(%rsp)
	movb	$-15, 166(%rsp)
	movb	$-22, 167(%rsp)
	movb	$22, 64(%rsp)
	movb	$3, 65(%rsp)
	movb	$1, 66(%rsp)
	movb	$0, 67(%rsp)
	movb	$74, 68(%rsp)
	movb	$2, 69(%rsp)
	movb	$0, 70(%rsp)
	movb	$0, 71(%rsp)
	movb	$70, 72(%rsp)
	movb	$3, 73(%rsp)
	movb	$1, 74(%rsp)
	movb	$66, 75(%rsp)
	movb	$-123, 76(%rsp)
	movb	$69, 77(%rsp)
	movb	$-89, 78(%rsp)
	movb	$39, 79(%rsp)
	movb	$-87, 80(%rsp)
	movb	$93, 81(%rsp)
	movb	$-96, 82(%rsp)
	movb	$-77, 83(%rsp)
	movb	$-59, 84(%rsp)
	movb	$-25, 85(%rsp)
	movb	$83, 86(%rsp)
	movl	UFDBglobalDebug(%rip), %r15d
	movb	$-38, 87(%rsp)
	movb	$72, 88(%rsp)
	movb	$43, 89(%rsp)
	testl	%r15d, %r15d
	movb	$63, 90(%rsp)
	movb	$-58, 91(%rsp)
	movb	$90, 92(%rsp)
	movb	$-54, 93(%rsp)
	movb	$21, (%rsp)
	movb	$3, 1(%rsp)
	movb	$1, 2(%rsp)
	movb	$0, 3(%rsp)
	movb	$2, 4(%rsp)
	movb	$2, 5(%rsp)
# SUCC: 11 [29.0%]  (CAN_FALLTHRU) 3 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L124
# BLOCK 3 freq:7100 seq:1
# PRED: 2 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugGtalk(%rip), %r14d
	testl	%r14d, %r14d
# SUCC: 11 [50.0%]  (CAN_FALLTHRU) 4 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L124
# BLOCK 4 freq:10000 seq:2
# PRED: 3 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 11 [100.0%] 
.L125:
	movl	%r13d, %esi
	movq	%r12, %rdi
	call	UFDBopenSocket
	testl	%eax, %eax
	movl	%eax, %ebx
# SUCC: 12 [47.5%]  (CAN_FALLTHRU) 5 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L159
# BLOCK 5 freq:5248 seq:3
# PRED: 4 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$16, %r8d
	movl	$20, %edx
	movl	$1, %esi
	movq	$3, 32(%rsp)
	leaq	32(%rsp), %rcx
	movl	%eax, %edi
	movq	$0, 40(%rsp)
	call	setsockopt
	movl	$16, %r8d
	movl	$21, %edx
	leaq	32(%rsp), %rcx
	movl	$1, %esi
	movl	%ebx, %edi
	movq	$3, 32(%rsp)
	movq	$0, 40(%rsp)
	call	setsockopt
	movl	$72, %edx
	movl	%ebx, %edi
	leaq	96(%rsp), %rsi
	call	write
	cmpq	$72, %rax
# SUCC: 6 [92.7%]  (FALLTHRU,CAN_FALLTHRU) 15 [7.3%]  (CAN_FALLTHRU)
	je	.L129
# BLOCK 6 freq:4867 seq:4
# PRED: 5 [92.7%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %r10d
	testl	%r10d, %r10d
# SUCC: 8 [29.0%]  (CAN_FALLTHRU) 7 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L130
# BLOCK 7 freq:3456 seq:5
# PRED: 6 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugGtalk(%rip), %r9d
	testl	%r9d, %r9d
# SUCC: 8 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 9 [39.0%]  (CAN_FALLTHRU)
	je	.L131
# BLOCK 8 freq:3519 seq:6
# PRED: 7 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [29.0%]  (CAN_FALLTHRU)
.L130:
	call	__errno_location
	movl	(%rax), %edi
	call	strerror
	movl	%r13d, %edx
	movq	%r12, %rsi
	movq	%rax, %rcx
	movl	$.LC23, %edi
	xorl	%eax, %eax
# SUCC: 9 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 9 freq:4867 seq:7
# PRED: 7 [39.0%]  (CAN_FALLTHRU) 8 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L131:
	movl	%ebx, %edi
	movl	$8, %ebx
# SUCC: 10 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	close
# BLOCK 10 freq:10000 seq:8
# PRED: 14 [100.0%]  13 [50.0%]  (CAN_FALLTHRU) 31 [50.0%]  (CAN_FALLTHRU) 9 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 18 [50.0%]  (CAN_FALLTHRU) 44 [100.0%]  19 [100.0%]  37 [71.0%]  (CAN_FALLTHRU) 32 [100.0%]  39 [100.0%]  36 [100.0%]  38 [100.0%]  46 [100.0%] 
.L157:
	leaq	-40(%rbp), %rsp
	movl	%ebx, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 11 freq:6450 seq:9
# PRED: 3 [50.0%]  (CAN_FALLTHRU) 2 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L124:
	.cfi_restore_state
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC22, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 4 [100.0%] 
	jmp	.L125
# BLOCK 12 freq:4752 seq:10
# PRED: 4 [47.5%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L159:
	movl	UFDBglobalDebug(%rip), %ebx
	testl	%ebx, %ebx
# SUCC: 14 [45.3%]  (CAN_FALLTHRU) 13 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L127
# BLOCK 13 freq:2598 seq:11
# PRED: 12 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugGtalk(%rip), %r11d
	movl	$8, %ebx
	testl	%r11d, %r11d
# SUCC: 14 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 10 [50.0%]  (CAN_FALLTHRU)
	je	.L157
# BLOCK 14 freq:3453 seq:12
# PRED: 13 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 12 [45.3%]  (CAN_FALLTHRU)
.L127:
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC1, %edi
	xorl	%eax, %eax
	movl	$8, %ebx
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L157
# BLOCK 15 freq:381 seq:13
# PRED: 5 [7.3%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L129:
	leaq	192(%rsp), %r15
	movl	$300, %edx
	movl	%ebx, %edi
	movq	%r15, %rsi
	call	read
	movl	%ebx, %edi
	movq	%rax, %r14
	call	close
	cmpl	$29, %r14d
# SUCC: 16 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 20 [50.0%]  (CAN_FALLTHRU)
	jle	.L132
# BLOCK 16 freq:191 seq:14
# PRED: 15 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	64(%rsp), %rsi
	movl	$30, %edx
	movq	%r15, %rdi
	call	memcmp
	testl	%eax, %eax
# SUCC: 17 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 33 [50.0%]  (CAN_FALLTHRU)
	jne	.L133
# BLOCK 17 freq:95 seq:15
# PRED: 16 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %r8d
	testl	%r8d, %r8d
# SUCC: 19 [45.3%]  (CAN_FALLTHRU) 18 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L134
# BLOCK 18 freq:52 seq:16
# PRED: 17 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugGtalk(%rip), %edi
	movl	$20, %ebx
	testl	%edi, %edi
# SUCC: 19 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 10 [50.0%]  (CAN_FALLTHRU)
	je	.L157
# BLOCK 19 freq:69 seq:17
# PRED: 18 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 17 [45.3%]  (CAN_FALLTHRU)
.L134:
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC24, %edi
	xorl	%eax, %eax
	movl	$20, %ebx
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L157
# BLOCK 20 freq:190 seq:18
# PRED: 15 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L132:
	cmpl	$6, %r14d
# SUCC: 33 [24.7%]  (CAN_FALLTHRU) 21 [75.3%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L133
# BLOCK 21 freq:143 seq:19
# PRED: 20 [75.3%]  (FALLTHRU,CAN_FALLTHRU)
	testl	%r14d, %r14d
	.p2align 4,,4
# SUCC: 37 [35.9%]  (CAN_FALLTHRU) 22 [64.1%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L160
# BLOCK 22 freq:139 seq:20
# PRED: 21 [64.1%]  (FALLTHRU,CAN_FALLTHRU) 33 [33.0%]  (CAN_FALLTHRU)
.L136:
	movl	UFDBglobalDebugGtalk(%rip), %ecx
	testl	%ecx, %ecx
# SUCC: 45 [29.0%]  (CAN_FALLTHRU) 23 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L161
# BLOCK 23 freq:139 seq:21
# PRED: 22 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 45 [100.0%] 
.L138:
	movl	$.LC28, %edi
	movl	$5, %ecx
	movq	%r15, %rsi
	repz cmpsb
# SUCC: 24 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 25 [50.0%]  (CAN_FALLTHRU)
	jne	.L139
# BLOCK 24 freq:69 seq:22
# PRED: 23 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movzbl	197(%rsp), %eax
	subl	$48, %eax
	cmpb	$2, %al
# SUCC: 46 [45.3%]  (CAN_FALLTHRU) 25 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	jbe	.L162
# BLOCK 25 freq:107 seq:23
# PRED: 24 [54.7%]  (FALLTHRU,CAN_FALLTHRU) 23 [50.0%]  (CAN_FALLTHRU)
.L139:
	movl	$.LC4, %edi
	movl	$3, %ecx
	movq	%r15, %rsi
	repz cmpsb
# SUCC: 39 [45.3%]  (CAN_FALLTHRU) 26 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L163
# BLOCK 26 freq:59 seq:24
# PRED: 25 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$80, %r14d
# SUCC: 27 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 40 [50.0%]  (CAN_FALLTHRU)
	jle	.L141
# BLOCK 27 freq:29 seq:25
# PRED: 26 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$5, %edx
	movl	$.LC30, %esi
	movq	%r15, %rdi
	movb	$0, 491(%rsp)
	call	strncasecmp
	testl	%eax, %eax
# SUCC: 28 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 44 [39.0%]  (CAN_FALLTHRU)
	jne	.L143
# BLOCK 28 freq:18 seq:26
# PRED: 27 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC31, %esi
	movq	%r15, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 29 [89.9%]  (FALLTHRU,CAN_FALLTHRU) 44 [10.1%]  (CAN_FALLTHRU)
	je	.L143
# BLOCK 29 freq:16 seq:27
# PRED: 28 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC32, %esi
	movq	%r15, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 30 [89.9%]  (FALLTHRU,CAN_FALLTHRU) 44 [10.1%]  (CAN_FALLTHRU)
	je	.L143
# BLOCK 30 freq:14 seq:28
# PRED: 29 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %edx
	testl	%edx, %edx
# SUCC: 32 [45.3%]  (CAN_FALLTHRU) 31 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L144
# BLOCK 31 freq:8 seq:29
# PRED: 30 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugFBchat(%rip)
	movl	$23, %ebx
# SUCC: 32 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 10 [50.0%]  (CAN_FALLTHRU)
	je	.L157
# BLOCK 32 freq:10 seq:30
# PRED: 31 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 30 [45.3%]  (CAN_FALLTHRU)
.L144:
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC33, %edi
	xorl	%eax, %eax
	movl	$23, %ebx
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L157
# BLOCK 33 freq:143 seq:31
# PRED: 20 [24.7%]  (CAN_FALLTHRU) 16 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L133:
	movl	$6, %edx
	movq	%rsp, %rsi
	movq	%r15, %rdi
	call	memcmp
	testl	%eax, %eax
# SUCC: 34 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 22 [33.0%]  (CAN_FALLTHRU)
	jne	.L136
# BLOCK 34 freq:96 seq:32
# PRED: 33 [67.0%]  (FALLTHRU,CAN_FALLTHRU)
	movzbl	198(%rsp), %esi
	movl	$.LC21, %edx
	cmpl	$114, %esi
# SUCC: 36 [39.0%]  (CAN_FALLTHRU) 35 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	ja	.L137
# BLOCK 35 freq:58 seq:33
# PRED: 34 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%esi, %eax
# SUCC: 36 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	CSWTCH.216(,%rax,8), %rdx
# BLOCK 36 freq:96 seq:34
# PRED: 34 [39.0%]  (CAN_FALLTHRU) 35 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L137:
	movl	%r13d, %r8d
	movq	%r12, %rcx
	movl	$.LC25, %edi
	xorl	%eax, %eax
	movl	$30, %ebx
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L157
# BLOCK 37 freq:51 seq:35
# PRED: 21 [35.9%]  (CAN_FALLTHRU)
.L160:
	movl	UFDBglobalDebugGtalk(%rip), %esi
	movl	$4, %ebx
	testl	%esi, %esi
# SUCC: 38 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 10 [71.0%]  (CAN_FALLTHRU)
	je	.L157
# BLOCK 38 freq:15 seq:36
# PRED: 37 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	__errno_location
	movl	(%rax), %edi
	call	strerror
	movl	%r14d, %ecx
	movl	%r13d, %edx
	movq	%rax, %r8
	movq	%r12, %rsi
	movl	$.LC26, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L157
# BLOCK 39 freq:49 seq:37
# PRED: 25 [45.3%]  (CAN_FALLTHRU)
.L163:
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC5, %edi
	xorl	%eax, %eax
	movl	$11, %ebx
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L157
# BLOCK 40 freq:29 seq:38
# PRED: 26 [50.0%]  (CAN_FALLTHRU)
.L141:
	movl	UFDBglobalDebugGtalk(%rip), %eax
	testl	%eax, %eax
# SUCC: 41 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 44 [39.0%]  (CAN_FALLTHRU)
	je	.L143
# BLOCK 41 freq:18 seq:39
# PRED: 40 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	testl	%r14d, %r14d
# SUCC: 42 [99.0%]  (FALLTHRU,CAN_FALLTHRU) 44 [1.0%]  (CAN_FALLTHRU)
	jle	.L143
# BLOCK 42 freq:18 seq:40
# PRED: 41 [99.0%]  (FALLTHRU,CAN_FALLTHRU)
	leal	-1(%r14), %r12d
	xorl	%ebx, %ebx
# SUCC: 43 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	addq	$1, %r12
# BLOCK 43 freq:1771 seq:41
# PRED: 43 [99.0%]  (DFS_BACK,CAN_FALLTHRU) 42 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L145:
	movzbl	(%r15,%rbx), %edx
	movl	%ebx, %esi
	xorl	%eax, %eax
	movl	$.LC34, %edi
	addq	$1, %rbx
	movl	%edx, %ecx
	andl	$95, %ecx
	orl	$32, %ecx
	call	ufdbLogMessage
	cmpq	%r12, %rbx
# SUCC: 43 [99.0%]  (DFS_BACK,CAN_FALLTHRU) 44 [1.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L145
# BLOCK 44 freq:44 seq:42
# PRED: 27 [39.0%]  (CAN_FALLTHRU) 28 [10.1%]  (CAN_FALLTHRU) 29 [10.1%]  (CAN_FALLTHRU) 40 [39.0%]  (CAN_FALLTHRU) 41 [1.0%]  (CAN_FALLTHRU) 43 [1.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
.L143:
	xorl	%ebx, %ebx
# SUCC: 10 [100.0%] 
	jmp	.L157
# BLOCK 45 freq:40 seq:43
# PRED: 22 [29.0%]  (CAN_FALLTHRU)
.L161:
	movl	%r13d, %ecx
	movq	%r12, %rdx
	movl	%r14d, %esi
	movl	$.LC27, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 23 [100.0%] 
	jmp	.L138
# BLOCK 46 freq:31 seq:44
# PRED: 24 [45.3%]  (CAN_FALLTHRU)
.L162:
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC29, %edi
	xorl	%eax, %eax
	movl	$30, %ebx
	call	ufdbLogMessage
# SUCC: 10 [100.0%] 
	jmp	.L157
	.cfi_endproc
.LFE46:
	.size	UFDBdetectGtalk, .-UFDBdetectGtalk
	.p2align 4,,15
	.globl	UFDBdetectSkype
	.type	UFDBdetectSkype, @function
UFDBdetectSkype:
.LFB47:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	detectSkypeSN
	.cfi_endproc
.LFE47:
	.size	UFDBdetectSkype, .-UFDBdetectSkype
	.section	.rodata.str1.8
	.align 8
.LC35:
	.string	"   probing for SSH server on %s:%d ..."
	.align 8
.LC36:
	.string	"     opening of socket failed for %s:%d"
	.align 8
.LC37:
	.string	"      received SSH Server Message from %s:%d"
	.text
	.p2align 4,,15
	.globl	UFDBdetectSSH
	.type	UFDBdetectSSH, @function
UFDBdetectSSH:
.LFB48:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movl	%esi, %r13d
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$48, %rsp
	.cfi_offset 3, -56
	movl	UFDBglobalDebug(%rip), %ecx
	testl	%ecx, %ecx
# SUCC: 11 [29.0%]  (CAN_FALLTHRU) 3 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L175
# BLOCK 3 freq:10000 seq:1
# PRED: 2 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 11 [100.0%] 
.L166:
	movl	%r13d, %esi
	movq	%r12, %rdi
	call	UFDBopenSocket
	testl	%eax, %eax
	movl	%eax, %ebx
# SUCC: 9 [47.5%]  (CAN_FALLTHRU) 4 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L176
# BLOCK 4 freq:5248 seq:2
# PRED: 3 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$16, %r8d
	movl	$20, %edx
	movl	$1, %esi
	movq	$3, 16(%rsp)
	leaq	16(%rsp), %rcx
	movl	%eax, %edi
	movq	$0, 24(%rsp)
	call	setsockopt
	movl	$16, %r8d
	movl	%ebx, %edi
	leaq	16(%rsp), %rcx
	movl	$21, %edx
	movl	$1, %esi
	movq	$3, 16(%rsp)
	movq	$0, 24(%rsp)
	call	setsockopt
	movl	%ebx, %edi
	movl	$4, %edx
	movq	%rsp, %rsi
	call	read
	movl	%ebx, %edi
	xorl	%ebx, %ebx
	movq	%rax, %r14
	call	close
	cmpl	$2, %r14d
# SUCC: 5 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 8 [39.0%]  (CAN_FALLTHRU)
	jle	.L173
# BLOCK 5 freq:3201 seq:3
# PRED: 4 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC4, %edi
	movl	$3, %ecx
	movq	%rsp, %rsi
	repz cmpsb
# SUCC: 6 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 8 [39.0%]  (CAN_FALLTHRU)
	jne	.L173
# BLOCK 6 freq:1953 seq:4
# PRED: 5 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %eax
	movb	$11, %bl
	testl	%eax, %eax
# SUCC: 7 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 8 [71.0%]  (CAN_FALLTHRU)
	je	.L173
# BLOCK 7 freq:566 seq:5
# PRED: 6 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC37, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 8 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
# BLOCK 8 freq:10000 seq:6
# PRED: 7 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 9 [71.0%]  (CAN_FALLTHRU) 6 [71.0%]  (CAN_FALLTHRU) 4 [39.0%]  (CAN_FALLTHRU) 5 [39.0%]  (CAN_FALLTHRU) 10 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L173:
	leaq	-40(%rbp), %rsp
	movl	%ebx, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 9 freq:4752 seq:7
# PRED: 3 [47.5%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L176:
	.cfi_restore_state
	movl	UFDBglobalDebug(%rip), %edx
	movl	$8, %ebx
	testl	%edx, %edx
# SUCC: 10 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 8 [71.0%]  (CAN_FALLTHRU)
	je	.L173
# BLOCK 10 freq:1378 seq:8
# PRED: 9 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r13d, %edx
	movq	%r12, %rsi
	movl	$.LC36, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 8 [100.0%] 
	jmp	.L173
# BLOCK 11 freq:2900 seq:9
# PRED: 2 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L175:
	movl	%esi, %edx
	xorl	%eax, %eax
	movq	%rdi, %rsi
	movl	$.LC35, %edi
	call	ufdbLogMessage
# SUCC: 3 [100.0%] 
	jmp	.L166
	.cfi_endproc
.LFE48:
	.size	UFDBdetectSSH, .-UFDBdetectSSH
	.p2align 4,,15
	.globl	UFDBopenssl_read
	.type	UFDBopenssl_read, @function
UFDBopenssl_read:
.LFB51:
	.cfi_startproc
# BLOCK 2 freq:873 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rdi, %r14
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	movl	%esi, %r13d
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdx, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	call	__errno_location
	movq	%rax, %r12
	movl	$0, (%rax)
# SUCC: 5 [100.0%] 
	jmp	.L179
# BLOCK 3 freq:9700 seq:1
# PRED: 5 [97.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L182:
	movl	%eax, %esi
	movq	%rbp, %rdi
	call	SSL_get_error
	cmpl	$5, %eax
# SUCC: 4 [97.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L180
# BLOCK 4 freq:9409 seq:2
# PRED: 3 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$4, (%r12)
# SUCC: 5 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 6 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L180
# BLOCK 5 freq:10000 seq:3
# PRED: 4 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 2 [100.0%] 
.L179:
	movl	%r13d, %edx
	movq	%r14, %rsi
	movq	%rbp, %rdi
	call	SSL_read
	cmpl	$-1, %eax
	movl	%eax, %ebx
# SUCC: 3 [97.0%]  (CAN_FALLTHRU) 6 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	je	.L182
# BLOCK 6 freq:873 seq:4
# PRED: 4 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 5 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 3 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L180:
	movl	%ebx, %eax
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE51:
	.size	UFDBopenssl_read, .-UFDBopenssl_read
	.p2align 4,,15
	.globl	UFDBopenssl_write
	.type	UFDBopenssl_write, @function
UFDBopenssl_write:
.LFB52:
	.cfi_startproc
# BLOCK 2 freq:873 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rdi, %r14
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	movl	%esi, %r13d
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdx, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	call	__errno_location
	movq	%rax, %r12
	movl	$0, (%rax)
# SUCC: 5 [100.0%] 
	jmp	.L185
# BLOCK 3 freq:9700 seq:1
# PRED: 5 [97.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L188:
	movl	%eax, %esi
	movq	%rbp, %rdi
	call	SSL_get_error
	cmpl	$5, %eax
# SUCC: 4 [97.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L186
# BLOCK 4 freq:9409 seq:2
# PRED: 3 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$4, (%r12)
# SUCC: 5 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 6 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L186
# BLOCK 5 freq:10000 seq:3
# PRED: 4 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 2 [100.0%] 
.L185:
	movl	%r13d, %edx
	movq	%r14, %rsi
	movq	%rbp, %rdi
	call	SSL_write
	cmpl	$-1, %eax
	movl	%eax, %ebx
# SUCC: 3 [97.0%]  (CAN_FALLTHRU) 6 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	je	.L188
# BLOCK 6 freq:873 seq:4
# PRED: 4 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 5 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 3 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L186:
	movl	%ebx, %eax
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE52:
	.size	UFDBopenssl_write, .-UFDBopenssl_write
	.p2align 4,,15
	.globl	UFDBopenssl_close
	.type	UFDBopenssl_close, @function
UFDBopenssl_close:
.LFB53:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	testq	%rdi, %rdi
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
# SUCC: 5 [21.6%]  (CAN_FALLTHRU) 3 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L192
# BLOCK 3 freq:7836 seq:1
# PRED: 2 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 6 [13.1%]  (CAN_FALLTHRU) 4 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L194
# BLOCK 4 freq:6808 seq:2
# PRED: 3 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	SSL_free
# BLOCK 5 freq:2164 seq:3
# PRED: 2 [21.6%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L192:
	.cfi_restore_state
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 6 freq:1028 seq:4
# PRED: 3 [13.1%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L194:
	.cfi_restore_state
	call	ERR_clear_error
	movq	%rbx, %rdi
	popq	%rbx
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%]  (ABNORMAL,SIBCALL)
	jmp	SSL_free
	.cfi_endproc
.LFE53:
	.size	UFDBopenssl_close, .-UFDBopenssl_close
	.p2align 4,,15
	.globl	UFDBloadAllowedHTTPSsites
	.type	UFDBloadAllowedHTTPSsites, @function
UFDBloadAllowedHTTPSsites:
.LFB55:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	xorl	%eax, %eax
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE55:
	.size	UFDBloadAllowedHTTPSsites, .-UFDBloadAllowedHTTPSsites
	.p2align 4,,15
	.globl	UFDBread
	.type	UFDBread, @function
UFDBread:
.LFB57:
	.cfi_startproc
# BLOCK 2 freq:900 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	movl	%edi, %r13d
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	xorl	%r12d, %r12d
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	movq	%rsi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movl	%edx, %ebx
	subq	$8, %rsp
	.cfi_def_cfa_offset 48
# SUCC: 4 [100.0%] 
	jmp	.L197
# BLOCK 3 freq:9100 seq:1
# PRED: 4 [91.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L198:
	movslq	%eax, %rdx
	addl	%eax, %r12d
	subl	%eax, %ebx
# SUCC: 4 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU)
	addq	%rdx, %rbp
# BLOCK 4 freq:10000 seq:2
# PRED: 3 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 2 [100.0%] 
.L197:
	movslq	%ebx, %rdx
	movq	%rbp, %rsi
	movl	%r13d, %edi
	call	read
	testl	%eax, %eax
# SUCC: 3 [91.0%]  (CAN_FALLTHRU) 5 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jg	.L198
# BLOCK 5 freq:900 seq:3
# PRED: 4 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	testl	%r12d, %r12d
	cmovne	%r12d, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE57:
	.size	UFDBread, .-UFDBread
	.p2align 4,,15
	.globl	UFDBsetTunnelCheckMethod
	.type	UFDBsetTunnelCheckMethod, @function
UFDBsetTunnelCheckMethod:
.LFB60:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	movl	%edi, UFDBglobalTunnelCheckMethod(%rip)
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE60:
	.size	UFDBsetTunnelCheckMethod, .-UFDBsetTunnelCheckMethod
	.section	.rodata.str1.1
.LC38:
	.string	"/local/squid/blacklists"
.LC39:
	.string	"none"
.LC40:
	.string	"UFDBinitHTTPSchecker started"
	.section	.rodata.str1.8
	.align 8
.LC41:
	.string	"WARNING: TLS/SSL run-time library vs. compile-time header version mismatch:  *****\nOpenSSL %d.%d.%d may not be compatible with OpenSSL %d.%d.%d  %s"
	.align 8
.LC42:
	.string	"OpenSSL 1.0.2k-fips  26 Jan 2017"
	.section	.rodata.str1.1
.LC43:
	.string	"beta %d"
	.section	.rodata.str1.8
	.align 8
.LC44:
	.string	"using OpenSSL library %d.%d.%d%s %s (%s)"
	.align 8
.LC45:
	.string	"UFDBinitHTTPSchecker: tls_inited is %d"
	.align 8
.LC46:
	.string	"CA certificates are not defined, using default file"
	.section	.rodata.str1.1
.LC47:
	.string	"dbhome"
	.section	.rodata.str1.8
	.align 8
.LC48:
	.string	"UFDBinitHTTPSchecker: CA certficates are in file \"%s\" and/or directory \"%s\""
	.align 8
.LC49:
	.string	"SSL library initialisation failed because of a problem with the CA certificate file:  *****\nfile: \"%s\", error: %s"
	.align 8
.LC50:
	.string	"Cannot perform mandatory check of TLS/SSL certificates."
	.section	.rodata.str1.1
.LC51:
	.string	"Fix the problem  ***** "
	.section	.rodata.str1.8
	.align 8
.LC52:
	.string	"In case that there is no cacerts file, option enforce-https-official-certificate must be \"off\""
	.align 8
.LC53:
	.string	"Failure to load the CA database; TLS/SSL certificates cannot be verified  *****\nCA file is \"%s\" and CA directory is \"%s\""
	.align 8
.LC54:
	.string	"ABORTING.  Fix the problem and start ufdbguardd again."
	.align 8
.LC55:
	.string	"HTTPS/SSL verification with trusted certificates from file \"%s\" and directory \"%s\""
	.align 8
.LC56:
	.string	"UFDBinitHTTPSchecker finished: tls_inited is %d"
	.text
	.p2align 4,,15
	.globl	UFDBinitHTTPSchecker
	.type	UFDBinitHTTPSchecker, @function
UFDBinitHTTPSchecker:
.LFB70:
	.cfi_startproc
# BLOCK 2 freq:2912 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	.cfi_offset 3, -40
	xorl	%ebx, %ebx
	andq	$-32, %rsp
	subq	$176, %rsp
	movl	tls_inited(%rip), %eax
	testl	%eax, %eax
# SUCC: 4 [39.0%]  (CAN_FALLTHRU) 3 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L255
# BLOCK 3 freq:2298 seq:1
# PRED: 42 [100.0%]  2 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 41 [100.0%]  29 [100.0%]  46 [100.0%] 
.L253:
	leaq	-24(%rbp), %rsp
	movl	%ebx, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 4 freq:1135 seq:2
# PRED: 2 [39.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L255:
	.cfi_restore_state
	movl	$init_mutex, %edi
	call	pthread_mutex_lock
	movl	tls_inited(%rip), %eax
	testl	%eax, %eax
# SUCC: 29 [20.7%]  (CAN_FALLTHRU) 5 [79.3%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L256
# BLOCK 5 freq:900 seq:3
# PRED: 4 [79.3%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %r10d
	movl	$1, tls_inited(%rip)
	testl	%r10d, %r10d
# SUCC: 27 [29.0%]  (CAN_FALLTHRU) 6 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L257
# BLOCK 6 freq:900 seq:4
# PRED: 5 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 27 [100.0%] 
.L206:
	movl	$mystrcmp, %edx
	movl	$hostname2hash, %esi
	movl	$317, %edi
	call	UFDBcreateHashtable
	xorl	%edi, %edi
	movq	%rax, %rbx
	movq	%rax, myht(%rip)
	call	time
	sall	$26, %ebx
	movq	%rax, %r12
	movq	%rax, lastHTTPScachePurgeTime(%rip)
	call	getpid
	sall	$8, %eax
	xorl	%r12d, %eax
	leal	(%rbx,%rax), %edi
	call	srandom
	call	SSLeay
	cmpq	$2351, %rax
# SUCC: 26 [39.0%]  (CAN_FALLTHRU) 7 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jle	.L258
# BLOCK 7 freq:549 seq:5
# PRED: 6 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpq	$9459711, %rax
# SUCC: 33 [39.0%]  (CAN_FALLTHRU) 8 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jle	.L259
# BLOCK 8 freq:335 seq:6
# PRED: 7 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rax, %rdi
	movl	%eax, %r8d
	movq	%rax, %rcx
	sarq	$4, %rdi
	movq	%rax, %rdx
	sarq	$28, %rax
	sarq	$12, %rcx
	sarq	$20, %rdx
	movzbl	%dil, %ebx
	andl	$15, %r8d
	andl	$127, %edi
	cmpq	$9461760, %rax
	cmovl	%edi, %ebx
	movzbl	%cl, %ecx
	movzbl	%dl, %edx
# SUCC: 9 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movzbl	%al, %esi
# BLOCK 9 freq:686 seq:7
# PRED: 8 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 26 [100.0%] 
.L208:
	cmpl	$1, %esi
# SUCC: 11 [21.5%]  (CAN_FALLTHRU) 10 [78.5%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L210
# BLOCK 10 freq:538 seq:8
# PRED: 9 [78.5%]  (FALLTHRU,CAN_FALLTHRU)
	testl	%edx, %edx
# SUCC: 11 [62.2%]  (FALLTHRU,CAN_FALLTHRU) 34 [37.8%]  (CAN_FALLTHRU)
	je	.L260
# BLOCK 11 freq:823 seq:9
# PRED: 9 [21.5%]  (CAN_FALLTHRU) 10 [62.2%]  (FALLTHRU,CAN_FALLTHRU) 34 [62.2%]  (CAN_FALLTHRU) 33 [100.0%] 
.L210:
	movq	$.LC42, 8(%rsp)
	xorl	%r9d, %r9d
	xorl	%eax, %eax
	movl	$2, (%rsp)
	movl	$1, %r8d
	movl	$.LC41, %edi
# SUCC: 12 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 12 freq:900 seq:10
# PRED: 11 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 39 [100.0%] 
.L213:
	call	SSL_load_error_strings
	xorl	%ebx, %ebx
	call	SSL_library_init
	call	OPENSSL_add_all_algorithms_noconf
	xorl	%edi, %edi
	call	OPENSSL_config
	call	ENGINE_load_builtin_engines
	movl	$ufdb_pthread_id_callback, %edi
	call	CRYPTO_set_id_callback
	call	CRYPTO_num_locks
	movl	%eax, num_static_ssl_locks(%rip)
	addl	$1, %eax
	cltq
	leaq	(%rax,%rax,4), %rdi
	salq	$3, %rdi
	call	ufdbMalloc
	movl	num_static_ssl_locks(%rip), %r13d
	movq	%rax, crypto_mutexes(%rip)
	movq	%rax, %r12
	testl	%r13d, %r13d
# SUCC: 13 [91.0%]  (FALLTHRU,CAN_FALLTHRU) 14 [9.0%]  (CAN_FALLTHRU)
	js	.L220
# BLOCK 13 freq:9100 seq:11
# PRED: 12 [91.0%]  (FALLTHRU,CAN_FALLTHRU) 13 [91.0%]  (DFS_BACK,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L250:
	xorl	%esi, %esi
	movq	%r12, %rdi
	addl	$1, %ebx
	call	pthread_mutex_init
	addq	$40, %r12
	cmpl	%r13d, %ebx
# SUCC: 13 [91.0%]  (DFS_BACK,CAN_FALLTHRU) 14 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jle	.L250
# BLOCK 14 freq:900 seq:12
# PRED: 12 [9.0%]  (CAN_FALLTHRU) 13 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
.L220:
	movl	$ufdb_pthread_locking_callback, %edi
	call	CRYPTO_set_locking_callback
	movl	$ufdb_ssl_dyn_create, %edi
	call	CRYPTO_set_dynlock_create_callback
	movl	$ufdb_ssl_dyn_lock, %edi
	call	CRYPTO_set_dynlock_lock_callback
	movl	$ufdb_ssl_dyn_destroy, %edi
	call	CRYPTO_set_dynlock_destroy_callback
	call	SSLv23_client_method
	movq	%rax, %rdi
	call	SSL_CTX_new
	xorl	%ecx, %ecx
	movl	$16777216, %edx
	movl	$32, %esi
	movq	%rax, %rdi
	movq	%rax, ssl_ctx(%rip)
	call	SSL_CTX_ctrl
	movq	ssl_ctx(%rip), %rdi
	xorl	%ecx, %ecx
	movl	$33554432, %edx
	movl	$32, %esi
	call	SSL_CTX_ctrl
	movq	ssl_ctx(%rip), %rdi
	testq	%rdi, %rdi
# SUCC: 15 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 16 [30.2%]  (CAN_FALLTHRU)
	je	.L219
# BLOCK 15 freq:628 seq:13
# PRED: 14 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%ecx, %ecx
	movl	$2181041151, %edx
	movl	$32, %esi
	call	SSL_CTX_ctrl
	movq	ssl_ctx(%rip), %rdi
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movl	$44, %esi
	call	SSL_CTX_ctrl
	movq	ssl_ctx(%rip), %rdi
	movl	$120, %esi
	call	SSL_CTX_set_timeout
	movq	ssl_ctx(%rip), %rdi
	call	SSL_CTX_set_default_verify_paths
	movq	ssl_ctx(%rip), %rdi
	movl	$12, %esi
	call	SSL_CTX_set_verify_depth
# SUCC: 16 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$2, tls_inited(%rip)
# BLOCK 16 freq:900 seq:14
# PRED: 15 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 14 [30.2%]  (CAN_FALLTHRU)
.L219:
	movl	UFDBglobalDebug(%rip), %r8d
	testl	%r8d, %r8d
# SUCC: 28 [29.0%]  (CAN_FALLTHRU) 17 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L261
# BLOCK 17 freq:900 seq:15
# PRED: 16 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 28 [100.0%] 
.L222:
	cmpq	$0, ssl_ctx(%rip)
# SUCC: 42 [12.8%]  (CAN_FALLTHRU) 18 [87.2%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L262
# BLOCK 18 freq:785 seq:16
# PRED: 17 [87.2%]  (FALLTHRU,CAN_FALLTHRU)
	cmpb	$0, UFDBglobalCAcertsFile(%rip)
# SUCC: 19 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 20 [50.0%]  (CAN_FALLTHRU)
	jne	.L224
# BLOCK 19 freq:393 seq:17
# PRED: 18 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpb	$0, UFDBglobalCAcertsDir(%rip)
# SUCC: 43 [29.0%]  (CAN_FALLTHRU) 20 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L263
# BLOCK 20 freq:785 seq:18
# PRED: 19 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 18 [50.0%]  (CAN_FALLTHRU) 43 [100.0%] 
.L224:
	movl	UFDBglobalDebug(%rip), %esi
	testl	%esi, %esi
# SUCC: 32 [29.0%]  (CAN_FALLTHRU) 21 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L264
# BLOCK 21 freq:785 seq:19
# PRED: 20 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 32 [100.0%] 
.L226:
	movzbl	UFDBglobalCAcertsFile(%rip), %eax
	testb	%al, %al
# SUCC: 30 [29.0%]  (CAN_FALLTHRU) 22 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L265
# BLOCK 22 freq:732 seq:20
# PRED: 21 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 31 [100.0%] 
.L227:
	xorl	%esi, %esi
	cmpb	$0, UFDBglobalCAcertsDir(%rip)
	movl	$UFDBglobalCAcertsDir, %r12d
	movq	%rsi, %rdx
	movq	ssl_ctx(%rip), %rdi
	movl	$UFDBglobalCAcertsFile, %ebx
	cmovne	%r12, %rdx
	testb	%al, %al
	cmovne	%rbx, %rsi
	call	SSL_CTX_load_verify_locations
	cmpl	$1, %eax
	movl	%eax, ufdbCacertsLoaded(%rip)
# SUCC: 23 [83.9%]  (FALLTHRU,CAN_FALLTHRU) 40 [16.1%]  (CAN_FALLTHRU)
	je	.L232
# BLOCK 23 freq:614 seq:21
# PRED: 22 [83.9%]  (FALLTHRU,CAN_FALLTHRU)
	cmpb	$0, UFDBglobalCAcertsDir(%rip)
	movl	$.LC39, %esi
	movq	%r12, %rdx
	movl	$.LC53, %edi
	cmove	%rsi, %rdx
	cmpb	$0, UFDBglobalCAcertsFile(%rip)
	cmovne	%rbx, %rsi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	UFDBglobalHttpsOfficialCertificate(%rip), %edx
	testl	%edx, %edx
# SUCC: 24 [39.0%]  (FALLTHRU,CAN_FALLTHRU) 25 [61.0%]  (CAN_FALLTHRU)
	je	.L235
# BLOCK 24 freq:239 seq:22
# PRED: 23 [39.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC50, %edi
	xorl	%eax, %eax
	call	ufdbLogFatalError
	movl	$.LC54, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$.LC52, %edi
	xorl	%eax, %eax
# SUCC: 25 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 25 freq:614 seq:23
# PRED: 23 [61.0%]  (CAN_FALLTHRU) 24 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L235:
	movl	$init_mutex, %edi
	movl	$7, %ebx
	movl	$7, tls_inited(%rip)
	call	pthread_mutex_unlock
	call	__errno_location
	movl	$2, (%rax)
	leaq	-24(%rbp), %rsp
	movl	%ebx, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 26 freq:351 seq:24
# PRED: 6 [39.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L258:
	.cfi_restore_state
	movl	%eax, %ebx
	movq	%rax, %rcx
	movq	%rax, %rdx
	sarq	$12, %rax
	sarq	$4, %rcx
	andl	$15, %ebx
	sarq	$8, %rdx
	movl	%eax, %esi
	andl	$15, %ecx
	andl	$15, %edx
	andl	$15, %esi
	xorl	%r8d, %r8d
# SUCC: 9 [100.0%] 
	jmp	.L208
# BLOCK 27 freq:261 seq:25
# PRED: 5 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L257:
	movl	$.LC40, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 6 [100.0%] 
	jmp	.L206
# BLOCK 28 freq:261 seq:26
# PRED: 16 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L261:
	movl	tls_inited(%rip), %esi
	movl	$.LC45, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 17 [100.0%] 
	jmp	.L222
# BLOCK 29 freq:235 seq:27
# PRED: 4 [20.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L256:
	movl	$init_mutex, %edi
	call	pthread_mutex_unlock
	movl	$10000, %edi
	call	usleep
# SUCC: 3 [100.0%] 
	jmp	.L253
# BLOCK 30 freq:228 seq:28
# PRED: 21 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L265:
	leaq	32(%rsp), %rdx
	movl	$UFDBglobalCAcertsFile, %esi
	movl	$1, %edi
	call	__xstat
	testl	%eax, %eax
# SUCC: 44 [23.5%]  (CAN_FALLTHRU) 31 [76.5%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L228
# BLOCK 31 freq:174 seq:29
# PRED: 30 [76.5%]  (FALLTHRU,CAN_FALLTHRU)
	movzbl	UFDBglobalCAcertsFile(%rip), %eax
# SUCC: 22 [100.0%] 
	jmp	.L227
# BLOCK 32 freq:228 seq:30
# PRED: 20 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L264:
	movl	$UFDBglobalCAcertsDir, %edx
	movl	$UFDBglobalCAcertsFile, %esi
	movl	$.LC48, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 21 [100.0%] 
	jmp	.L226
# BLOCK 33 freq:214 seq:31
# PRED: 7 [39.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L259:
	movq	%rax, %rcx
	shrq	$20, %rax
	xorl	%esi, %esi
	sarq	$12, %rcx
	movq	%rax, %rdx
	movzbl	%cl, %ecx
# SUCC: 11 [100.0%] 
	jmp	.L210
# BLOCK 34 freq:204 seq:32
# PRED: 10 [37.8%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L260:
	cmpl	$2, %ecx
# SUCC: 11 [62.2%]  (CAN_FALLTHRU) 35 [37.8%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L210
# BLOCK 35 freq:77 seq:33
# PRED: 34 [37.8%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$15, %r8d
# SUCC: 48 [28.0%]  (CAN_FALLTHRU) 36 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L266
# BLOCK 36 freq:55 seq:34
# PRED: 35 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	32(%rsp), %r12
	movl	%r8d, %edx
	movl	$.LC43, %esi
	movq	%r12, %rdi
	xorl	%eax, %eax
# SUCC: 37 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	sprintf
# BLOCK 37 freq:77 seq:35
# PRED: 36 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 48 [100.0%] 
.L215:
	xorl	%eax, %eax
	testl	%ebx, %ebx
# SUCC: 39 [50.0%]  (CAN_FALLTHRU) 38 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L216
# BLOCK 38 freq:39 seq:36
# PRED: 37 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	leal	96(%rbx), %eax
# SUCC: 39 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movb	$0, 17(%rsp)
# BLOCK 39 freq:77 seq:37
# PRED: 37 [50.0%]  (CAN_FALLTHRU) 38 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L216:
	movb	%al, 16(%rsp)
	movq	%r12, %r9
	movl	$2, %ecx
	movq	$.LC42, (%rsp)
	leaq	16(%rsp), %r8
	xorl	%edx, %edx
	movl	$1, %esi
	movl	$.LC44, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 12 [100.0%] 
	jmp	.L213
# BLOCK 40 freq:118 seq:38
# PRED: 22 [16.1%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L232:
	cmpb	$0, UFDBglobalCAcertsDir(%rip)
	movl	$.LC39, %eax
	movq	%r12, %rdx
	movl	$.LC55, %edi
	cmove	%rax, %rdx
	cmpb	$0, UFDBglobalCAcertsFile(%rip)
	cmovne	%rbx, %rax
	movq	%rax, %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movq	ssl_ctx(%rip), %rdi
	xorl	%edx, %edx
	xorl	%esi, %esi
	call	SSL_CTX_set_verify
	movq	ssl_ctx(%rip), %rdi
	xorl	%ecx, %ecx
	movl	$4, %edx
	movl	$33, %esi
	call	SSL_CTX_ctrl
	movl	UFDBglobalDebug(%rip), %eax
	movl	$3, tls_inited(%rip)
	testl	%eax, %eax
# SUCC: 47 [29.0%]  (CAN_FALLTHRU) 41 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L267
# BLOCK 41 freq:118 seq:39
# PRED: 40 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 47 [100.0%] 
.L238:
	movl	$init_mutex, %edi
	xorl	%ebx, %ebx
	call	pthread_mutex_unlock
# SUCC: 3 [100.0%] 
	jmp	.L253
# BLOCK 42 freq:115 seq:40
# PRED: 17 [12.8%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L262:
	movl	$0, UFDBglobalHttpsOfficialCertificate(%rip)
	movl	$7, %ebx
	movl	$5, tls_inited(%rip)
	call	__errno_location
	movl	$init_mutex, %edi
	movl	$22, (%rax)
	call	pthread_mutex_unlock
# SUCC: 3 [100.0%] 
	jmp	.L253
# BLOCK 43 freq:114 seq:41
# PRED: 19 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L263:
	xorl	%eax, %eax
	movl	$.LC46, %edi
	call	ufdbLogMessage
	movl	$.LC47, %edi
	call	ufdbSettingGetValue
	movl	$.LC38, %edx
	movl	$UFDBglobalCAcertsFile, %edi
	testq	%rax, %rax
	cmove	%rdx, %rax
	movq	%rax, %rsi
	call	strcpy
	movl	$UFDBglobalCAcertsFile, %edi
	call	strlen
	movl	$115, %edi
	movabsq	$8388361629458592559, %rcx
	leaq	UFDBglobalCAcertsFile(%rax), %rdx
	movq	%rcx, UFDBglobalCAcertsFile(%rax)
	movabsq	$8390880533254713209, %rax
	movq	%rax, 8(%rdx)
	movw	%di, 16(%rdx)
# SUCC: 20 [100.0%] 
	jmp	.L224
# BLOCK 44 freq:53 seq:42
# PRED: 30 [23.5%]  (CAN_FALLTHRU)
.L228:
	call	__errno_location
	movl	(%rax), %edi
	movq	%rax, %rbx
	call	strerror
	movl	$UFDBglobalCAcertsFile, %esi
	movl	$.LC49, %edi
	movq	%rax, %rdx
	xorl	%eax, %eax
	call	ufdbLogError
	movl	UFDBglobalHttpsOfficialCertificate(%rip), %ecx
	testl	%ecx, %ecx
# SUCC: 45 [39.0%]  (FALLTHRU,CAN_FALLTHRU) 46 [61.0%]  (CAN_FALLTHRU)
	je	.L229
# BLOCK 45 freq:21 seq:43
# PRED: 44 [39.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC50, %edi
	xorl	%eax, %eax
	call	ufdbLogFatalError
	movl	$.LC51, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$.LC52, %edi
	xorl	%eax, %eax
# SUCC: 46 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 46 freq:53 seq:44
# PRED: 44 [61.0%]  (CAN_FALLTHRU) 45 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L229:
	movl	$6, tls_inited(%rip)
	movl	$init_mutex, %edi
	call	pthread_mutex_unlock
	movl	$2, (%rbx)
	movl	$7, %ebx
# SUCC: 3 [100.0%] 
	jmp	.L253
# BLOCK 47 freq:34 seq:45
# PRED: 40 [29.0%]  (CAN_FALLTHRU)
.L267:
	movl	tls_inited(%rip), %esi
	movl	$.LC56, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 41 [100.0%] 
	jmp	.L238
# BLOCK 48 freq:22 seq:46
# PRED: 35 [28.0%]  (CAN_FALLTHRU)
.L266:
	leaq	32(%rsp), %r12
	movl	$82, %r9d
	movw	%r9w, 32(%rsp)
# SUCC: 37 [100.0%] 
	jmp	.L215
	.cfi_endproc
.LFE70:
	.size	UFDBinitHTTPSchecker, .-UFDBinitHTTPSchecker
	.section	.rodata.str1.8
	.align 8
.LC57:
	.string	"UFDBopenssl_connect %s %d %d ..."
	.align 8
.LC58:
	.string	"UFDBopenssl_connect: SSL_new failed fd=%d"
	.align 8
.LC59:
	.string	"TLS/SSL connection failed with OpenSSL error:"
	.section	.rodata.str1.1
.LC60:
	.string	"   SSL error: %s"
	.section	.rodata.str1.8
	.align 8
.LC61:
	.string	"UFDBopenssl_connect: SSL_set_fd failed fd=%d"
	.align 8
.LC62:
	.string	"UFDBopenssl_connect: SSL_set_tlsext_host_name failed for '%s'"
	.align 8
.LC63:
	.string	"HTTPS/SSL connection for %s:%d has an insecure SSLv2 protocol. It is blocked.  *****"
	.align 8
.LC64:
	.string	"HTTPS/SSL connection for %s:%d has an insecure SSLv3 protocol. It is blocked.  *****"
	.align 8
.LC65:
	.string	"UFDBopenssl_connect: SSL_connect to %s:%d failed:\nret=%d  errno=%d(%s)  SSLerr=%d(%s)  state=0x%08lx  %s"
	.text
	.p2align 4,,15
	.globl	UFDBopenssl_connect
	.type	UFDBopenssl_connect, @function
UFDBopenssl_connect:
.LFB79:
	.cfi_startproc
# BLOCK 2 freq:1825 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movl	%edx, %r13d
	pushq	%r12
	.cfi_offset 12, -48
	movl	%esi, %r12d
	pushq	%rbx
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$960, %rsp
	movl	UFDBglobalDebug(%rip), %esi
	movq	%rcx, 56(%rsp)
	testl	%esi, %esi
# SUCC: 26 [29.0%]  (CAN_FALLTHRU) 3 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L324
# BLOCK 3 freq:1825 seq:1
# PRED: 2 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 26 [100.0%] 
.L269:
	call	waitForTLSinitilisation
	call	ERR_clear_error
	.p2align 4,,5
	call	__errno_location
	movq	ssl_ctx(%rip), %rdi
	movl	$0, (%rax)
	movq	%rax, %r14
	call	SSL_new
	movq	56(%rsp), %rdx
	movl	%r13d, %esi
	testq	%rax, %rax
	movq	%rax, (%rdx)
# SUCC: 27 [26.4%]  (CAN_FALLTHRU) 4 [73.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L325
# BLOCK 4 freq:1343 seq:2
# PRED: 3 [73.6%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rax, %rdi
	call	SSL_set_fd
	testl	%eax, %eax
# SUCC: 5 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 10 [33.0%]  (CAN_FALLTHRU)
	jne	.L275
# BLOCK 5 freq:900 seq:3
# PRED: 4 [67.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	160(%rsp), %r12
	movl	%r13d, %esi
	movl	$.LC61, %edi
	call	ufdbLogError
	leaq	448(%rsp), %r14
	xorl	%r13d, %r13d
	leaq	128(%rsp), %rbx
	leaq	192(%rsp), %r15
# SUCC: 7 [100.0%] 
	jmp	.L276
# BLOCK 6 freq:9100 seq:4
# PRED: 8 [71.0%]  (CAN_FALLTHRU) 9 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L277:
	movq	%rcx, %rdi
	movl	$256, %edx
	movl	$1, %r13d
	leaq	704(%rsp), %rsi
	call	ERR_error_string_n
	leaq	704(%rsp), %rsi
	xorl	%eax, %eax
	movl	$.LC60, %edi
# SUCC: 7 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 7 freq:10000 seq:5
# PRED: 6 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 5 [100.0%] 
.L276:
	movq	%r12, %rcx
	movq	%r14, %rdx
	movq	%rbx, %rsi
	movq	%r15, %rdi
	call	ERR_get_error_line_data
	testq	%rax, %rax
	movq	%rax, %rcx
# SUCC: 8 [91.0%]  (FALLTHRU,CAN_FALLTHRU) 21 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L326
# BLOCK 8 freq:9100 seq:6
# PRED: 7 [91.0%]  (FALLTHRU,CAN_FALLTHRU)
	testl	%r13d, %r13d
# SUCC: 9 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [71.0%]  (CAN_FALLTHRU)
	jne	.L277
# BLOCK 9 freq:2639 seq:7
# PRED: 8 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC59, %edi
	xorl	%eax, %eax
	movq	%rcx, 48(%rsp)
	call	ufdbLogError
	movq	48(%rsp), %rcx
# SUCC: 6 [100.0%] 
	jmp	.L277
# BLOCK 10 freq:443 seq:8
# PRED: 4 [33.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L275:
	movq	56(%rsp), %r15
	movq	(%r15), %rdi
	call	SSL_set_connect_state
	movq	(%r15), %rdi
	xorl	%edx, %edx
	movq	%rbx, %rcx
	movl	$55, %esi
	call	SSL_ctrl
	testq	%rax, %rax
# SUCC: 41 [29.0%]  (CAN_FALLTHRU) 11 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L327
# BLOCK 11 freq:443 seq:9
# PRED: 10 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 41 [100.0%] 
.L281:
	movq	56(%rsp), %r15
	movl	$0, (%r14)
	movq	(%r15), %rdi
	call	SSL_connect
	movq	(%r15), %rdi
	movl	%eax, %ecx
	movl	(%r14), %r13d
	testl	%ecx, %ecx
	movslq	72(%rdi), %r8
# SUCC: 12 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 32 [50.0%]  (CAN_FALLTHRU)
	jg	.L328
# BLOCK 12 freq:222 seq:10
# PRED: 32 [50.0%]  (CAN_FALLTHRU) 11 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
.L282:
	testl	%r13d, %r13d
# SUCC: 13 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 47 [50.0%]  (CAN_FALLTHRU)
	jne	.L288
# BLOCK 13 freq:111 seq:11
# PRED: 12 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movb	$0, 200(%rsp)
	leaq	192(%rsp), %r15
	movabsq	$8245935278387130222, %rax
# SUCC: 14 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rax, 192(%rsp)
# BLOCK 14 freq:222 seq:12
# PRED: 13 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 47 [100.0%] 
.L289:
	movq	%r8, 32(%rsp)
	leaq	448(%rsp), %r14
	movl	%ecx, %esi
	movl	%ecx, 40(%rsp)
	call	SSL_get_error
	movl	$256, %edx
	movq	%r14, %rsi
	movslq	%eax, %rdi
	movl	%eax, 48(%rsp)
	call	ERR_error_string_n
	movl	48(%rsp), %eax
	movl	$.LC21, %edx
	movl	40(%rsp), %ecx
	movq	32(%rsp), %r8
	cmpl	$8, %eax
# SUCC: 16 [50.0%]  (CAN_FALLTHRU) 15 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	ja	.L290
# BLOCK 15 freq:111 seq:13
# PRED: 14 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%eax, %edx
# SUCC: 16 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	CSWTCH.218(,%rdx,8), %rdx
# BLOCK 16 freq:222 seq:14
# PRED: 14 [50.0%]  (CAN_FALLTHRU) 15 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L290:
	movq	%r14, 24(%rsp)
	movq	%rbx, %rsi
	movq	%r15, %r9
	movq	%r8, 16(%rsp)
	movl	$.LC65, %edi
	movl	%r13d, %r8d
	movq	%rdx, 8(%rsp)
	movl	%r12d, %edx
	xorl	%r14d, %r14d
	movl	%eax, (%rsp)
	xorl	%eax, %eax
	call	ufdbLogError
	leaq	160(%rsp), %r12
	leaq	128(%rsp), %rbx
# SUCC: 18 [100.0%] 
	jmp	.L291
# BLOCK 17 freq:2241 seq:15
# PRED: 19 [71.0%]  (CAN_FALLTHRU) 20 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L292:
	movl	$256, %edx
	movq	%r15, %rdi
	movl	$1, %r14d
	leaq	704(%rsp), %rsi
	call	ERR_error_string_n
	leaq	704(%rsp), %rsi
	xorl	%eax, %eax
	movl	$.LC60, %edi
# SUCC: 18 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 18 freq:2463 seq:16
# PRED: 17 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 16 [100.0%] 
.L291:
	leaq	96(%rsp), %rcx
	movq	%r12, %rdx
	movq	%rbx, %rdi
	leaq	64(%rsp), %rsi
	call	ERR_get_error_line_data
	testq	%rax, %rax
	movq	%rax, %r15
# SUCC: 19 [91.0%]  (FALLTHRU,CAN_FALLTHRU) 42 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L329
# BLOCK 19 freq:2241 seq:17
# PRED: 18 [91.0%]  (FALLTHRU,CAN_FALLTHRU)
	testl	%r14d, %r14d
# SUCC: 20 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 17 [71.0%]  (CAN_FALLTHRU)
	jne	.L292
# BLOCK 20 freq:650 seq:18
# PRED: 19 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC59, %edi
	xorl	%eax, %eax
	call	ufdbLogError
# SUCC: 17 [100.0%] 
	jmp	.L292
# BLOCK 21 freq:900 seq:19
# PRED: 7 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L326:
	movq	56(%rsp), %rax
	movq	(%rax), %rbx
	testq	%rbx, %rbx
# SUCC: 24 [21.6%]  (CAN_FALLTHRU) 22 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L318
# BLOCK 22 freq:705 seq:20
# PRED: 21 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 48 [13.1%]  (CAN_FALLTHRU) 23 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L330
# BLOCK 23 freq:705 seq:21
# PRED: 22 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 48 [100.0%] 
.L280:
	movq	%rbx, %rdi
# SUCC: 24 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 24 freq:1443 seq:22
# PRED: 23 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 21 [21.6%]  (CAN_FALLTHRU) 29 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT) 45 [28.0%]  (CAN_FALLTHRU)
.L318:
# SUCC: 25 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$29, %eax
# BLOCK 25 freq:1825 seq:23
# PRED: 40 [100.0%]  34 [54.1%]  (CAN_FALLTHRU) 35 [54.7%]  (CAN_FALLTHRU) 24 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 49 [54.7%]  (CAN_FALLTHRU) 46 [100.0%] 
.L316:
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 26 freq:529 seq:24
# PRED: 2 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L324:
	.cfi_restore_state
	movl	%edx, %ecx
	movq	%rdi, %rsi
	movl	%r12d, %edx
	movl	$.LC57, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 3 [100.0%] 
	jmp	.L269
# BLOCK 27 freq:481 seq:25
# PRED: 3 [26.4%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L325:
	leaq	160(%rsp), %r12
	movl	$.LC58, %edi
	xorl	%r13d, %r13d
	call	ufdbLogError
	leaq	448(%rsp), %r14
	leaq	128(%rsp), %rbx
	leaq	192(%rsp), %r15
# SUCC: 29 [100.0%] 
	jmp	.L271
# BLOCK 28 freq:4867 seq:26
# PRED: 30 [71.0%]  (CAN_FALLTHRU) 31 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L272:
	movq	%rcx, %rdi
	movl	$256, %edx
	movl	$1, %r13d
	leaq	704(%rsp), %rsi
	call	ERR_error_string_n
	leaq	704(%rsp), %rsi
	xorl	%eax, %eax
	movl	$.LC60, %edi
# SUCC: 29 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 29 freq:5348 seq:27
# PRED: 28 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 27 [100.0%] 
.L271:
	movq	%r12, %rcx
	movq	%r14, %rdx
	movq	%rbx, %rsi
	movq	%r15, %rdi
	call	ERR_get_error_line_data
	testq	%rax, %rax
	movq	%rax, %rcx
# SUCC: 30 [91.0%]  (FALLTHRU,CAN_FALLTHRU) 24 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L318
# BLOCK 30 freq:4867 seq:28
# PRED: 29 [91.0%]  (FALLTHRU,CAN_FALLTHRU)
	testl	%r13d, %r13d
# SUCC: 31 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 28 [71.0%]  (CAN_FALLTHRU)
	jne	.L272
# BLOCK 31 freq:1411 seq:29
# PRED: 30 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC59, %edi
	xorl	%eax, %eax
	movq	%rcx, 56(%rsp)
	call	ufdbLogError
	movq	56(%rsp), %rcx
# SUCC: 28 [100.0%] 
	jmp	.L272
# BLOCK 32 freq:222 seq:30
# PRED: 11 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L328:
	cmpq	$3, %r8
# SUCC: 33 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 12 [50.0%]  (CAN_FALLTHRU)
	jne	.L282
# BLOCK 33 freq:222 seq:31
# PRED: 32 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	(%rdi), %edx
	xorl	%eax, %eax
	cmpl	$2, %edx
# SUCC: 49 [28.0%]  (CAN_FALLTHRU) 34 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L331
# BLOCK 34 freq:160 seq:32
# PRED: 33 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$768, %edx
# SUCC: 35 [45.9%]  (FALLTHRU,CAN_FALLTHRU) 25 [54.1%]  (CAN_FALLTHRU)
	jne	.L316
# BLOCK 35 freq:73 seq:33
# PRED: 34 [45.9%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalHttpsNoSSLv3(%rip), %edx
	testl	%edx, %edx
# SUCC: 36 [45.3%]  (FALLTHRU,CAN_FALLTHRU) 25 [54.7%]  (CAN_FALLTHRU)
	je	.L316
# BLOCK 36 freq:33 seq:34
# PRED: 35 [45.3%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r12d, %edx
	movq	%rbx, %rsi
# SUCC: 37 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC64, %edi
# BLOCK 37 freq:61 seq:35
# PRED: 36 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 50 [100.0%] 
.L323:
	call	ufdbLogError
	movq	56(%rsp), %rax
	movq	(%rax), %rbx
	testq	%rbx, %rbx
# SUCC: 40 [21.6%]  (CAN_FALLTHRU) 38 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L319
# BLOCK 38 freq:48 seq:36
# PRED: 37 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 52 [13.1%]  (CAN_FALLTHRU) 39 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L332
# BLOCK 39 freq:48 seq:37
# PRED: 38 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 52 [100.0%] 
.L287:
	movq	%rbx, %rdi
# SUCC: 40 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 40 freq:61 seq:38
# PRED: 39 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 37 [21.6%]  (CAN_FALLTHRU)
.L319:
	movl	$12, %eax
# SUCC: 25 [100.0%] 
	jmp	.L316
# BLOCK 41 freq:129 seq:39
# PRED: 10 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L327:
	movq	%rbx, %rsi
	movl	$.LC62, %edi
	call	ufdbLogMessage
# SUCC: 11 [100.0%] 
	jmp	.L281
# BLOCK 42 freq:222 seq:40
# PRED: 18 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L329:
	movq	56(%rsp), %rax
	movq	(%rax), %rbx
	testq	%rbx, %rbx
# SUCC: 45 [21.6%]  (CAN_FALLTHRU) 43 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L294
# BLOCK 43 freq:174 seq:41
# PRED: 42 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 51 [13.1%]  (CAN_FALLTHRU) 44 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L333
# BLOCK 44 freq:174 seq:42
# PRED: 43 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 51 [100.0%] 
.L295:
	movq	%rbx, %rdi
# SUCC: 45 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 45 freq:222 seq:43
# PRED: 44 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 42 [21.6%]  (CAN_FALLTHRU)
.L294:
	cmpl	$104, %r13d
# SUCC: 24 [28.0%]  (CAN_FALLTHRU) 46 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L318
# BLOCK 46 freq:160 seq:44
# PRED: 45 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$30, %eax
# SUCC: 25 [100.0%] 
	jmp	.L316
# BLOCK 47 freq:111 seq:45
# PRED: 12 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L288:
	leaq	192(%rsp), %r15
	movl	%r13d, %edi
	movl	$256, %edx
	movq	%r15, %rsi
	movq	%r8, 40(%rsp)
	movl	%ecx, 48(%rsp)
	call	__xpg_strerror_r
	movq	56(%rsp), %rax
	movq	40(%rsp), %r8
	movl	48(%rsp), %ecx
	movq	(%rax), %rdi
# SUCC: 14 [100.0%] 
	jmp	.L289
# BLOCK 48 freq:93 seq:46
# PRED: 22 [13.1%]  (CAN_FALLTHRU)
.L330:
	call	ERR_clear_error
# SUCC: 23 [100.0%] 
	jmp	.L280
# BLOCK 49 freq:62 seq:47
# PRED: 33 [28.0%]  (CAN_FALLTHRU)
.L331:
	movl	UFDBglobalHttpsNoSSLv2(%rip), %ecx
	testl	%ecx, %ecx
# SUCC: 50 [45.3%]  (FALLTHRU,CAN_FALLTHRU) 25 [54.7%]  (CAN_FALLTHRU)
	je	.L316
# BLOCK 50 freq:28 seq:48
# PRED: 49 [45.3%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC63, %edi
# SUCC: 37 [100.0%] 
	jmp	.L323
# BLOCK 51 freq:23 seq:49
# PRED: 43 [13.1%]  (CAN_FALLTHRU)
.L333:
	call	ERR_clear_error
# SUCC: 44 [100.0%] 
	jmp	.L295
# BLOCK 52 freq:6 seq:50
# PRED: 38 [13.1%]  (CAN_FALLTHRU)
.L332:
	.p2align 4,,8
	call	ERR_clear_error
	.p2align 4,,8
# SUCC: 39 [100.0%] 
	jmp	.L287
	.cfi_endproc
.LFE79:
	.size	UFDBopenssl_connect, .-UFDBopenssl_connect
	.section	.rodata.str1.1
.LC66:
	.string	"0.4.0.2042.1.4"
	.section	.rodata.str1.8
	.align 8
.LC67:
	.string	"cannot convert certificate OID"
	.align 8
.LC68:
	.string	"X.509 policy extension OID = %s"
	.align 8
.LC69:
	.string	"Certificate is EV. Issuer is %s"
	.text
	.p2align 4,,15
	.globl	certificateIsEV
	.type	certificateIsEV, @function
certificateIsEV:
.LFB81:
	.cfi_startproc
# BLOCK 2 freq:604 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	movl	$89, %esi
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	call	X509_get_ext_d2i
	testq	%rax, %rax
	movq	%rax, %r13
# SUCC: 19 [10.1%]  (CAN_FALLTHRU) 3 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L347
# BLOCK 3 freq:543 seq:1
# PRED: 2 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r13, %rdi
	xorl	%r12d, %r12d
	call	sk_num
	cmpl	%eax, %r12d
# SUCC: 4 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 17 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	jge	.L351
# BLOCK 4 freq:2401 seq:2
# PRED: 3 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 16 [95.5%]  (CAN_FALLTHRU)
.L345:
	movl	%r12d, %esi
	movq	%r13, %rdi
	call	sk_value
	testq	%rax, %rax
	movq	%rax, %rbx
# SUCC: 16 [30.2%]  (CAN_FALLTHRU) 5 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L338
# BLOCK 5 freq:1676 seq:3
# PRED: 4 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
	movq	(%rax), %rdx
	xorl	%esi, %esi
	xorl	%edi, %edi
	call	i2t_ASN1_OBJECT
	testl	%eax, %eax
	movl	%eax, %r14d
# SUCC: 16 [47.5%]  (CAN_FALLTHRU) 6 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	jle	.L338
# BLOCK 6 freq:880 seq:4
# PRED: 5 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	leal	1(%rax), %r15d
	movslq	%r15d, %rdi
	call	ufdbMalloc
	movq	(%rbx), %rdx
	movl	%r15d, %esi
	movq	%rax, %rdi
	movq	%rax, %rbp
	call	i2t_ASN1_OBJECT
	cmpl	%r14d, %eax
# SUCC: 7 [51.2%]  (FALLTHRU,CAN_FALLTHRU) 8 [48.8%]  (CAN_FALLTHRU)
	je	.L340
# BLOCK 7 freq:451 seq:5
# PRED: 6 [51.2%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC67, %edi
	xorl	%eax, %eax
# SUCC: 8 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogError
# BLOCK 8 freq:880 seq:6
# PRED: 6 [48.8%]  (CAN_FALLTHRU) 7 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L340:
	cmpl	$2, UFDBglobalDebug(%rip)
# SUCC: 18 [29.0%]  (CAN_FALLTHRU) 9 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L352
# BLOCK 9 freq:880 seq:7
# PRED: 8 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 18 [100.0%] 
.L341:
	movl	$knownEVcertIssuerOIDTable+8, %ebx
	movl	$.LC66, %esi
# SUCC: 11 [100.0%] 
	jmp	.L346
# BLOCK 10 freq:9120 seq:8
# PRED: 11 [95.5%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L342:
	addq	$8, %rbx
	movq	-8(%rbx), %rsi
	testq	%rsi, %rsi
# SUCC: 11 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 15 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L353
# BLOCK 11 freq:9550 seq:9
# PRED: 10 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 9 [100.0%] 
.L346:
	movq	%rbp, %rdi
	call	strcmp
	testl	%eax, %eax
# SUCC: 12 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 10 [95.5%]  (CAN_FALLTHRU)
	jne	.L342
# BLOCK 12 freq:430 seq:10
# PRED: 11 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 13 [63.3%]  (FALLTHRU,CAN_FALLTHRU) 14 [36.7%]  (CAN_FALLTHRU)
	jle	.L343
# BLOCK 13 freq:272 seq:11
# PRED: 12 [63.3%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbp, %rsi
	movl	$.LC69, %edi
# SUCC: 14 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 14 freq:430 seq:12
# PRED: 12 [36.7%]  (CAN_FALLTHRU) 13 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L343:
	movq	%rbp, %rdi
	call	ufdbFree
	movq	%r13, %rdi
	movl	$POLICYINFO_free, %esi
	call	sk_pop_free
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movl	$1, %eax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 15 freq:450 seq:13
# PRED: 10 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L353:
	.cfi_restore_state
	movq	%rbp, %rdi
# SUCC: 16 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbFree
# BLOCK 16 freq:1971 seq:14
# PRED: 4 [30.2%]  (CAN_FALLTHRU) 15 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 5 [47.5%]  (CAN_FALLTHRU)
.L338:
	movq	%r13, %rdi
	addl	$1, %r12d
	call	sk_num
	cmpl	%eax, %r12d
# SUCC: 4 [95.5%]  (CAN_FALLTHRU) 17 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jl	.L345
# BLOCK 17 freq:113 seq:15
# PRED: 3 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 16 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
.L351:
	movq	%r13, %rdi
	movl	$POLICYINFO_free, %esi
	call	sk_pop_free
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 18 freq:255 seq:16
# PRED: 8 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L352:
	.cfi_restore_state
	movq	%rbp, %rsi
	movl	$.LC68, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 9 [100.0%] 
	jmp	.L341
# BLOCK 19 freq:61 seq:17
# PRED: 2 [10.1%]  (CAN_FALLTHRU)
.L347:
	addq	$8, %rsp
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE81:
	.size	certificateIsEV, .-certificateIsEV
	.section	.rodata.str1.1
.LC70:
	.string	"has AIM"
.LC71:
	.string	"does not speak SSL+HTTP"
.LC72:
	.string	"has Google Talk"
.LC73:
	.string	"has Skype"
.LC74:
	.string	"has Yahoo IM"
.LC75:
	.string	"has Facebook Chat"
.LC76:
	.string	"has CitrixOnline"
.LC77:
	.string	"has AnyDesk"
.LC78:
	.string	"has Teamviewer"
.LC79:
	.string	"uses an unknown protocol"
.LC80:
	.string	"has a NULL connection"
.LC81:
	.string	"has a tunnel"
.LC82:
	.string	"has a connection error"
	.section	.rodata.str1.8
	.align 8
.LC83:
	.string	"does not accept a TLS connection"
	.section	.rodata.str1.1
.LC84:
	.string	"has a fatal connection error"
.LC85:
	.string	"has no FQDN but an IP address"
.LC86:
	.string	"SSLv2"
.LC87:
	.string	"SSLv3"
.LC88:
	.string	"TLSv1.0"
.LC89:
	.string	"TLSv1.1"
.LC90:
	.string	"TLSv1.2"
.LC91:
	.string	"DTLSv1"
.LC92:
	.string	"TLSv0"
.LC93:
	.string	"is NULL"
.LC94:
	.string	"has data"
.LC95:
	.string	"EV "
.LC96:
	.string	"chained "
.LC97:
	.string	"-"
.LC98:
	.string	"error-converting-name"
	.section	.rodata.str1.8
	.align 8
.LC99:
	.string	"httpsGETroot: hostname is NULL"
	.section	.rodata.str1.1
.LC100:
	.string	"httpsGETroot: ssl_ctx is NULL"
.LC101:
	.string	"   probePort %s:%d"
.LC102:
	.string	"Gtalk detected on %s:%d"
	.section	.rodata.str1.8
	.align 8
.LC103:
	.string	"Facebook Chat detected on %s:%d"
	.section	.rodata.str1.1
.LC104:
	.string	"Tunnel detected on %s:%d"
	.section	.rodata.str1.8
	.align 8
.LC105:
	.string	"aborting HTTPS probe because ufdbguardd is reloading its configuration"
	.section	.rodata.str1.1
.LC106:
	.string	"Skype detected on %s:%d"
.LC107:
	.string	"SSH detected on %s:%d"
	.section	.rodata.str1.8
	.align 8
.LC108:
	.string	"Unknown Protocol is used on %s:%d"
	.align 8
.LC109:
	.string	"forbidden Unknown Protocol used on %s:%d so it is flagged as a TUNNEL  *****"
	.align 8
.LC110:
	.string	"SSL connect failure to %s:%d.  The server %s."
	.align 8
.LC111:
	.string	"TLS/SSL connection to %s:%d established  method %s  cipher %s  %d-bits"
	.align 8
.LC112:
	.string	"certificate check of %s connection to %s:%d is aborted due to reconfiguration in process"
	.align 8
.LC113:
	.string	"site %s:%d has NO TLS/SSL certificate"
	.align 8
.LC114:
	.string	"site %s:%d certificate has no issuer"
	.align 8
.LC115:
	.string	"site %s:%d certificate has no subject"
	.align 8
.LC116:
	.string	"   CertificatesHasAIA: aia_data %s"
	.section	.rodata.str1.1
.LC117:
	.string	"   AIA CA Issuers URI: %s"
	.section	.rodata.str1.8
	.align 8
.LC118:
	.string	"/O=America Online Inc./CN=AOL Member CA"
	.align 8
.LC119:
	.string	"%s certificate on %s:%d is used by AOL or AIM since issuer is %s"
	.section	.rodata.str1.1
.LC120:
	.string	"/CN="
	.section	.rodata.str1.8
	.align 8
.LC121:
	.string	"   received %d certificates from %s"
	.align 8
.LC122:
	.string	"   %s%s%s certificate for %s has issuer '%s'"
	.align 8
.LC123:
	.string	"   %s%s%s certificate for %s has subject '%s' (category=%s)"
	.align 8
.LC124:
	.string	"   %s%s%s certificate for %s has subject '%s'"
	.align 8
.LC125:
	.string	"No CA certificates are loaded.  Cannot verify signature of %s%scertificate for %s:%d.  Marking it as invalid."
	.section	.rodata.str1.1
.LC126:
	.string	"*.uc.cn"
.LC127:
	.string	"*.ucweb.com"
	.section	.rodata.str1.8
	.align 8
.LC128:
	.string	"%s certificate for %s:%d: has a certificate with \"ucweb.com\" or \"uc.cn\" and is considered a proxy  *****\nissuer: %s\nsubject: %s"
	.section	.rodata.str1.1
.LC129:
	.string	".logme.in"
.LC130:
	.string	".logmein.com"
	.section	.rodata.str1.8
	.align 8
.LC131:
	.string	"%s certificate for %s:%d: has a certificate with \"logme.in\" and is considered a proxy  *****\nissuer: %s\nsubject: %s"
	.align 8
.LC132:
	.string	"%s%s%s certificate signature of %s:%d cannot be verified, assuming it is OK"
	.section	.rodata.str1.1
.LC133:
	.string	"/C="
.LC134:
	.string	"/O="
.LC135:
	.string	"/DC="
.LC136:
	.string	"/OU="
	.section	.rodata.str1.8
	.align 8
.LC137:
	.string	"%s certificate for %s:%d: has an UNRECOGNISED ISSUER and has the characteristics of a Tor proxy  *****\nissuer: %s\nsubject: %s"
	.align 8
.LC138:
	.string	"%s%s%s certificate for %s:%d: UNRECOGNISED ISSUER  (maybe a certificate chain issue)  *****\nissuer: %s\nsubject: %s"
	.align 8
.LC139:
	.string	"SSL_get_verify_result() for %s:%d is X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY (ignored)\nCheck the existence and file permissions of %s"
	.section	.rodata.str1.1
.LC140:
	.string	"/DC=microsoft/"
.LC141:
	.string	"skype.com"
	.section	.rodata.str1.8
	.align 8
.LC142:
	.string	"%s certificate for %s:%d: has a certificate for Skype  *****\nissuer: %s\nsubject: %s"
	.section	.rodata.str1.1
.LC143:
	.string	"citrixonline.com"
	.section	.rodata.str1.8
	.align 8
.LC144:
	.string	"%s certificate for %s:%d: has a certificate for CitrixOnline  *****\nissuer: %s\nsubject: %s"
	.section	.rodata.str1.1
.LC145:
	.string	"/CN=AnyNet Relay"
	.section	.rodata.str1.8
	.align 8
.LC146:
	.string	"%s certificate for %s:%d: has a certificate for AnyDesk  *****\nissuer: %s\nsubject: %s"
	.align 8
.LC147:
	.string	"%s certificate for %s:%d has a SELF-SIGNED certificate in chain  *****\nissuer: %s"
	.align 8
.LC148:
	.string	"/O=Yahoo/OU=Messenger/CN=undermine.corp/"
	.align 8
.LC149:
	.string	"%s certificate on %s:%d is used for Yahoo IM since issuer is %s"
	.align 8
.LC150:
	.string	"%s certificate for %s:%d has a DEPTH-ZERO SELF-SIGNED certificate  *****\nissuer: %s"
	.align 8
.LC151:
	.string	"%s certificate for %s:%d has a NOT YET VALID DATE  *****"
	.align 8
.LC152:
	.string	"%s certificate for %s:%d has EXPIRED  *****"
	.align 8
.LC153:
	.string	"%s certificate for %s:%d has an UNRECOGNISED ISSUER ?!?  *****\nissuer: %s"
	.align 8
.LC154:
	.string	"%s%s%s certificate for %s:%d cannot be verified."
	.align 8
.LC155:
	.string	"%s%s%s certificate for %s:%d CANNOT DECRYPT SIGNATURE."
	.align 8
.LC156:
	.string	"%s%s%s certificate for %s:%d has an unresolvable certificate signature failure.\nassuming that the signature is OK."
	.align 8
.LC157:
	.string	"%s%s%s certificate for %s:%d CERTIFICATE SIGNATURE FAILURE."
	.align 8
.LC158:
	.string	"%s certificate VERIFICATION ERROR for %s:%d %ld %s  *****"
	.align 8
.LC159:
	.string	"   %s%s%s certificate for %s:%d is signed by a CA (OK)"
	.align 8
.LC160:
	.string	"ssl_dns_name: invalid ASN1 value type in subjectAltName of TLS/SSL certificate"
	.align 8
.LC161:
	.string	"ssl_dns_name: internal NUL in subjectAltName of TLS/SSL certificate"
	.align 8
.LC162:
	.string	"      %s certificate for %s:%d has subjectAltName %s (category=%s)"
	.align 8
.LC163:
	.string	"      %s certificate for %s:%d has subjectAltName %s"
	.section	.rodata.str1.1
.LC164:
	.string	"/O=Google Inc/"
.LC165:
	.string	"google.com"
	.section	.rodata.str1.8
	.align 8
.LC166:
	.string	"%s certificate for %s:%d is used by %s (allowed)"
	.align 8
.LC167:
	.string	"%s certificate has subjectAltName \"%s\" which does NOT MATCH hostname \"%s\".  Certificate is invalid."
	.align 8
.LC168:
	.string	"%s certificate with subjectAltName \"%s\" matches hostname \"%s\""
	.align 8
.LC169:
	.string	"%s certificate for %s:%d has no subjectAltName"
	.align 8
.LC170:
	.string	"%s certificate for %s:%d has no CommonName (CN).  Certificate is invalid."
	.align 8
.LC171:
	.string	"%s certificate CommonName '%s' does NOT MATCH hostname `%s'.  Certificate is invalid."
	.align 8
.LC172:
	.string	"%s certificate matches hostname %s"
	.align 8
.LC173:
	.string	"%s certificate of %s:%d has an issue but enforce-https-official-certificate is off"
	.align 8
.LC174:
	.string	"%s connection to %s:%d has AIM"
	.align 8
.LC175:
	.string	"%s connection to %s:%d has Yahoo IM"
	.align 8
.LC176:
	.string	"%s connection to %s:%d has Skype"
	.align 8
.LC177:
	.string	"%s connection to %s:%d has Google Talk"
	.align 8
.LC178:
	.string	"%s connection to %s:%d has Facebook Chat"
	.align 8
.LC179:
	.string	"%s connection to %s:%d has CitrixOnline"
	.align 8
.LC180:
	.string	"%s connection to %s:%d has Anydesk"
	.align 8
.LC181:
	.string	"%s connection to %s:%d has Teamviewer"
	.align 8
.LC182:
	.string	"%s connection to %s:%d has a tunnel"
	.align 8
.LC183:
	.string	"%s connection to %s:%d has error code %d. It is marked as a TLS/SSL certificate issue"
	.align 8
.LC184:
	.string	"content check of %s connection to %s:%d is aborted due to reconfiguration in process"
	.align 8
.LC185:
	.ascii	"GET "
	.string	"/ HTTP/1.1\r\nUser-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; WOW64; Trident/6.0)\r\nHost: %s:%d\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nDNT: 1\r\nUpgrade-Insecure-Requests: 1\r\n\r\n"
	.section	.rodata.str1.1
.LC186:
	.string	" 200 "
.LC187:
	.string	"Content-Length: 0"
	.section	.rodata.str1.8
	.align 8
.LC188:
	.string	"httpsGETroot: read %d + %d bytes of content"
	.text
	.p2align 4,,15
	.type	httpsGETroot.constprop.7, @function
httpsGETroot.constprop.7:
.LFB92:
	.cfi_startproc
# BLOCK 2 freq:1367 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movq	%rsi, %r13
	pushq	%r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$37456, %rsp
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	testq	%rsi, %rsi
	movl	%edx, 108(%rsp)
	movq	%rcx, 48(%rsp)
	movq	%r8, 56(%rsp)
	movq	%r9, 80(%rsp)
# SUCC: 158 [1.9%]  (CAN_FALLTHRU) 3 [98.1%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L830
# BLOCK 3 freq:1341 seq:1
# PRED: 2 [98.1%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%edi, %ebx
	call	waitForTLSinitilisation
	cmpq	$0, ssl_ctx(%rip)
# SUCC: 159 [1.9%]  (CAN_FALLTHRU) 4 [98.1%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L831
# BLOCK 4 freq:1315 seq:2
# PRED: 3 [98.1%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %esi
	leaq	208(%rsp), %rcx
	movl	%ebx, %edx
	movq	%r13, %rdi
	movb	$0, 2608(%rsp)
	movb	$0, 4656(%rsp)
	movq	$0, 208(%rsp)
	call	UFDBopenssl_connect
	testl	%eax, %eax
	movl	%eax, 104(%rsp)
# SUCC: 100 [10.0%]  (CAN_FALLTHRU) 5 [90.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L832
# BLOCK 5 freq:1184 seq:3
# PRED: 4 [90.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	208(%rsp), %rdi
	movl	(%rdi), %eax
	cmpl	$769, %eax
# SUCC: 59 [14.3%]  (CAN_FALLTHRU) 6 [85.7%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L384
# BLOCK 6 freq:1015 seq:4
# PRED: 5 [85.7%]  (FALLTHRU,CAN_FALLTHRU)
# SUCC: 7 [58.3%]  (FALLTHRU,CAN_FALLTHRU) 33 [41.7%]  (CAN_FALLTHRU)
	jle	.L833
# BLOCK 7 freq:592 seq:5
# PRED: 6 [58.3%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$.LC90, 96(%rsp)
	cmpl	$771, %eax
# SUCC: 10 [28.6%]  (CAN_FALLTHRU) 8 [71.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L387
# BLOCK 8 freq:423 seq:6
# PRED: 7 [71.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$.LC89, 96(%rsp)
# SUCC: 10 [40.0%]  (CAN_FALLTHRU) 9 [60.0%]  (FALLTHRU,CAN_FALLTHRU)
	jl	.L387
# BLOCK 9 freq:254 seq:7
# PRED: 8 [60.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$.LC91, 96(%rsp)
	cmpl	$65279, %eax
# SUCC: 10 [66.7%]  (FALLTHRU,CAN_FALLTHRU) 35 [33.3%]  (CAN_FALLTHRU)
	jne	.L383
# BLOCK 10 freq:1015 seq:8
# PRED: 33 [40.0%]  (CAN_FALLTHRU) 34 [66.7%]  (CAN_FALLTHRU) 9 [66.7%]  (FALLTHRU,CAN_FALLTHRU) 8 [40.0%]  (CAN_FALLTHRU) 7 [28.6%]  (CAN_FALLTHRU) 59 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L387:
	movl	UFDBglobalDebug(%rip), %r14d
	testl	%r14d, %r14d
# SUCC: 36 [29.0%]  (CAN_FALLTHRU) 11 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L834
# BLOCK 11 freq:1184 seq:9
# PRED: 10 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 35 [71.0%]  (CAN_FALLTHRU) 36 [100.0%] 
.L391:
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 118 [10.0%]  (CAN_FALLTHRU) 12 [90.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L835
# BLOCK 12 freq:1066 seq:10
# PRED: 11 [90.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	208(%rsp), %rax
	movb	$0, 560(%rsp)
	movb	$0, 1584(%rsp)
	movq	%rax, 72(%rsp)
	movl	(%rax), %eax
	cmpl	$769, %eax
# SUCC: 60 [14.3%]  (CAN_FALLTHRU) 13 [85.7%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L396
# BLOCK 13 freq:914 seq:11
# PRED: 12 [85.7%]  (FALLTHRU,CAN_FALLTHRU)
# SUCC: 14 [58.3%]  (FALLTHRU,CAN_FALLTHRU) 49 [41.7%]  (CAN_FALLTHRU)
	jle	.L836
# BLOCK 14 freq:533 seq:12
# PRED: 13 [58.3%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$.LC90, 88(%rsp)
	cmpl	$771, %eax
# SUCC: 17 [28.6%]  (CAN_FALLTHRU) 15 [71.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L399
# BLOCK 15 freq:381 seq:13
# PRED: 14 [71.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$.LC89, 88(%rsp)
# SUCC: 17 [40.0%]  (CAN_FALLTHRU) 16 [60.0%]  (FALLTHRU,CAN_FALLTHRU)
	jl	.L399
# BLOCK 16 freq:229 seq:14
# PRED: 15 [60.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$.LC91, 88(%rsp)
	cmpl	$65279, %eax
# SUCC: 17 [66.7%]  (FALLTHRU,CAN_FALLTHRU) 51 [33.3%]  (CAN_FALLTHRU)
	jne	.L395
# BLOCK 17 freq:1066 seq:15
# PRED: 49 [40.0%]  (CAN_FALLTHRU) 50 [66.7%]  (CAN_FALLTHRU) 51 [100.0%]  15 [40.0%]  (CAN_FALLTHRU) 14 [28.6%]  (CAN_FALLTHRU) 16 [66.7%]  (FALLTHRU,CAN_FALLTHRU) 60 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L399:
	movq	72(%rsp), %rbx
	movq	%rbx, %rdi
	call	SSL_get_peer_certificate
	movq	%rbx, %rdi
	movq	%rax, %r14
	call	SSL_get_peer_cert_chain
	testq	%r14, %r14
	movq	%rax, 64(%rsp)
# SUCC: 61 [15.0%]  (CAN_FALLTHRU) 18 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L837
# BLOCK 18 freq:845 seq:16
# PRED: 17 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r14, %rdi
	call	X509_get_issuer_name
	movl	$1023, %edx
	leaq	560(%rsp), %rsi
	movq	%rax, %rdi
	call	X509_NAME_oneline
	cmpb	$0, 560(%rsp)
	movb	$0, 1583(%rsp)
	movl	$2, 44(%rsp)
# SUCC: 58 [29.0%]  (CAN_FALLTHRU) 19 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L838
# BLOCK 19 freq:845 seq:17
# PRED: 18 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 58 [100.0%] 
.L405:
	movq	%r14, %rdi
	call	X509_get_subject_name
	movl	$1023, %edx
	leaq	1584(%rsp), %rsi
	movq	%rax, %rdi
	call	X509_NAME_oneline
	cmpb	$0, 1584(%rsp)
	movb	$0, 2607(%rsp)
# SUCC: 57 [29.0%]  (CAN_FALLTHRU) 20 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L839
# BLOCK 20 freq:845 seq:18
# PRED: 19 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 57 [100.0%] 
.L406:
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 22 [52.5%]  (CAN_FALLTHRU) 21 [47.5%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L407
# BLOCK 21 freq:402 seq:19
# PRED: 20 [47.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalPeek(%rip), %ebx
	testl	%ebx, %ebx
# SUCC: 22 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 38 [50.0%]  (CAN_FALLTHRU)
	je	.L408
# BLOCK 22 freq:644 seq:20
# PRED: 21 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 20 [52.5%]  (CAN_FALLTHRU)
.L407:
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movl	$177, %esi
	movq	%r14, %rdi
	call	X509_get_ext_d2i
	cmpl	$2, UFDBglobalDebug(%rip)
	movq	%rax, %r12
# SUCC: 23 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 24 [50.0%]  (CAN_FALLTHRU)
	jle	.L409
# BLOCK 23 freq:322 seq:21
# PRED: 22 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	testq	%rax, %rax
	movl	$.LC93, %esi
	movl	$.LC94, %eax
	cmovne	%rax, %rsi
	movl	$.LC116, %edi
	xorl	%eax, %eax
# SUCC: 24 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 24 freq:644 seq:22
# PRED: 22 [50.0%]  (CAN_FALLTHRU) 23 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L409:
	testq	%r12, %r12
# SUCC: 38 [10.1%]  (CAN_FALLTHRU) 25 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L408
# BLOCK 25 freq:579 seq:23
# PRED: 24 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%ebx, %ebx
# SUCC: 27 [100.0%] 
	jmp	.L412
# BLOCK 26 freq:5854 seq:24
# PRED: 29 [72.0%]  (CAN_FALLTHRU) 31 [27.0%]  (CAN_FALLTHRU) 28 [72.0%]  (CAN_FALLTHRU) 32 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L413:
# SUCC: 27 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU)
	addl	$1, %ebx
# BLOCK 27 freq:6433 seq:25
# PRED: 26 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 25 [100.0%] 
.L412:
	movq	%r12, %rdi
	call	sk_num
	cmpl	%eax, %ebx
# SUCC: 28 [91.0%]  (FALLTHRU,CAN_FALLTHRU) 37 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jge	.L840
# BLOCK 28 freq:5854 seq:26
# PRED: 27 [91.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%ebx, %esi
	movq	%r12, %rdi
	call	sk_value
	movq	(%rax), %rdi
	movq	%rax, %r15
	call	OBJ_obj2nid
	cmpl	$179, %eax
# SUCC: 29 [28.0%]  (FALLTHRU,CAN_FALLTHRU) 26 [72.0%]  (CAN_FALLTHRU)
	jne	.L413
# BLOCK 29 freq:1639 seq:27
# PRED: 28 [28.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	8(%r15), %rax
	cmpl	$6, (%rax)
# SUCC: 30 [28.0%]  (FALLTHRU,CAN_FALLTHRU) 26 [72.0%]  (CAN_FALLTHRU)
	jne	.L413
# BLOCK 30 freq:459 seq:28
# PRED: 29 [28.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalPeek(%rip), %r11d
	testl	%r11d, %r11d
# SUCC: 32 [29.0%]  (CAN_FALLTHRU) 31 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L414
# BLOCK 31 freq:326 seq:29
# PRED: 30 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 32 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 26 [27.0%]  (CAN_FALLTHRU)
	jle	.L413
# BLOCK 32 freq:371 seq:30
# PRED: 31 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 30 [29.0%]  (CAN_FALLTHRU)
.L414:
	movq	8(%rax), %rax
	movl	$.LC117, %edi
	movq	8(%rax), %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 26 [100.0%] 
	jmp	.L413
# BLOCK 33 freq:423 seq:31
# PRED: 6 [41.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L833:
	movq	$.LC86, 96(%rsp)
	cmpl	$2, %eax
# SUCC: 10 [40.0%]  (CAN_FALLTHRU) 34 [60.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L387
# BLOCK 34 freq:254 seq:32
# PRED: 33 [60.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$.LC87, 96(%rsp)
	cmpl	$768, %eax
# SUCC: 10 [66.7%]  (CAN_FALLTHRU) 35 [33.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L387
# BLOCK 35 freq:169 seq:33
# PRED: 34 [33.3%]  (FALLTHRU,CAN_FALLTHRU) 9 [33.3%]  (CAN_FALLTHRU)
.L383:
	movq	$.LC92, 96(%rsp)
	movl	UFDBglobalDebug(%rip), %r14d
	testl	%r14d, %r14d
# SUCC: 36 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 11 [71.0%]  (CAN_FALLTHRU)
	je	.L391
# BLOCK 36 freq:343 seq:34
# PRED: 10 [29.0%]  (CAN_FALLTHRU) 35 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L834:
	call	SSL_get_current_cipher
	movq	%rax, %r12
	movq	%rax, %rdi
	call	SSL_CIPHER_get_name
	leaq	112(%rsp), %rsi
	movq	%r12, %rdi
	movq	%rax, %rbx
	call	SSL_CIPHER_get_bits
	movq	96(%rsp), %rcx
	movq	%rbx, %r8
	movl	108(%rsp), %edx
	movl	%eax, %r9d
	movq	%r13, %rsi
	movl	$.LC111, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 11 [100.0%] 
	jmp	.L391
# BLOCK 37 freq:579 seq:35
# PRED: 27 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L840:
	movl	$ACCESS_DESCRIPTION_free, %esi
	movq	%r12, %rdi
# SUCC: 38 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	sk_pop_free
# BLOCK 38 freq:845 seq:36
# PRED: 21 [50.0%]  (CAN_FALLTHRU) 37 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 24 [10.1%]  (CAN_FALLTHRU)
.L408:
	movq	%r14, %rdi
	call	certificateIsEV
	movq	64(%rsp), %rdi
	movl	%eax, 32(%rsp)
	call	sk_num
	movsbq	0(%r13), %rbx
	movl	%eax, 64(%rsp)
	testb	%bl, %bl
# SUCC: 39 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 43 [4.5%]  (CAN_FALLTHRU)
	je	.L821
# BLOCK 39 freq:807 seq:37
# PRED: 38 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r13, %r12
# SUCC: 40 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movsbq	%bl, %r15
# BLOCK 40 freq:9550 seq:38
# PRED: 42 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 39 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L421:
	cmpb	$46, %r15b
# SUCC: 41 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 42 [28.0%]  (CAN_FALLTHRU)
	je	.L418
# BLOCK 41 freq:6876 seq:39
# PRED: 40 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	__ctype_b_loc
	movq	(%rax), %rax
	testb	$8, 1(%rax,%r15,2)
# SUCC: 52 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 42 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L841
# BLOCK 42 freq:9241 seq:40
# PRED: 41 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 40 [28.0%]  (CAN_FALLTHRU)
.L418:
	addq	$1, %r12
	movsbq	(%r12), %r15
	testb	%r15b, %r15b
# SUCC: 40 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 43 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L421
# BLOCK 43 freq:623 seq:41
# PRED: 42 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 38 [4.5%]  (CAN_FALLTHRU) 56 [100.0%] 
.L821:
# SUCC: 44 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$1, %r15d
# BLOCK 44 freq:845 seq:42
# PRED: 43 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 117 [100.0%] 
.L417:
	leaq	560(%rsp), %rdi
	movl	$.LC118, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 45 [82.5%]  (FALLTHRU,CAN_FALLTHRU) 87 [17.6%]  (CAN_FALLTHRU)
	je	.L422
# BLOCK 45 freq:697 seq:43
# PRED: 44 [82.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %ebx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	leaq	560(%rsp), %r8
	movl	$.LC119, %edi
	movl	%ebx, %ecx
	call	ufdbLogMessage
	movq	%r14, %rdi
	call	X509_free
	movq	96(%rsp), %rsi
	movl	%ebx, %ecx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movl	$.LC174, %edi
	call	ufdbLogMessage
	movq	208(%rsp), %rbx
	testq	%rbx, %rbx
# SUCC: 48 [21.6%]  (CAN_FALLTHRU) 46 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L516
# BLOCK 46 freq:14 seq:44
# PRED: 45 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 130 [13.1%]  (CAN_FALLTHRU) 47 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L842
# BLOCK 47 freq:14 seq:45
# PRED: 46 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 130 [100.0%] 
.L517:
	movq	%rbx, %rdi
# SUCC: 48 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 48 freq:18 seq:46
# PRED: 47 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 45 [21.6%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L516:
	movq	80(%rsp), %rax
	movl	$22, (%rax)
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 49 freq:381 seq:47
# PRED: 13 [41.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L836:
	movq	$.LC86, 88(%rsp)
	cmpl	$2, %eax
# SUCC: 17 [40.0%]  (CAN_FALLTHRU) 50 [60.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L399
# BLOCK 50 freq:229 seq:48
# PRED: 49 [60.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$.LC87, 88(%rsp)
	cmpl	$768, %eax
# SUCC: 17 [66.7%]  (CAN_FALLTHRU) 51 [33.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L399
# BLOCK 51 freq:152 seq:49
# PRED: 50 [33.3%]  (FALLTHRU,CAN_FALLTHRU) 16 [33.3%]  (CAN_FALLTHRU)
.L395:
	movq	$.LC92, 88(%rsp)
# SUCC: 17 [100.0%] 
	jmp	.L399
# BLOCK 52 freq:309 seq:50
# PRED: 41 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L841:
# SUCC: 53 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r13, %rdx
# BLOCK 53 freq:3891 seq:51
# PRED: 55 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 52 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L420:
	cmpb	$58, %bl
# SUCC: 54 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 55 [28.0%]  (CAN_FALLTHRU)
	je	.L419
# BLOCK 54 freq:2802 seq:52
# PRED: 53 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	testb	$16, 1(%rax,%rbx,2)
# SUCC: 117 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 55 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L595
# BLOCK 55 freq:3765 seq:53
# PRED: 54 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 53 [28.0%]  (CAN_FALLTHRU)
.L419:
	addq	$1, %rdx
	movsbq	(%rdx), %rbx
	testb	%bl, %bl
# SUCC: 53 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 56 [4.5%]  (FALLTHRU)
	jne	.L420
# BLOCK 56 freq:169 seq:54
# PRED: 55 [4.5%]  (FALLTHRU)
# SUCC: 43 [100.0%] 
	jmp	.L821
# BLOCK 57 freq:245 seq:55
# PRED: 19 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L839:
	movl	108(%rsp), %edx
	movq	%r13, %rsi
	movl	$.LC115, %edi
	movabsq	$7094703426972298541, %rax
	movl	$2960741, 1616(%rsp)
	movq	%rax, 1584(%rsp)
	movabsq	$8462094570264683882, %rax
	movq	%rax, 1592(%rsp)
	movabsq	$7305732972759311470, %rax
	movq	%rax, 1600(%rsp)
	movabsq	$8386093285481477234, %rax
	movq	%rax, 1608(%rsp)
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$0, 44(%rsp)
# SUCC: 20 [100.0%] 
	jmp	.L406
# BLOCK 58 freq:245 seq:56
# PRED: 18 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L838:
	movl	108(%rsp), %edx
	movl	$11565, %r12d
	movq	%r13, %rsi
	movabsq	$8319108580547374381, %rax
	movl	$.LC114, %edi
	movw	%r12w, 592(%rsp)
	movq	%rax, 560(%rsp)
	movabsq	$7959390401087038837, %rax
	movq	%rax, 568(%rsp)
	movabsq	$8243103739748625764, %rax
	movq	%rax, 576(%rsp)
	movabsq	$7310575174727133556, %rax
	movq	%rax, 584(%rsp)
	xorl	%eax, %eax
	movb	$0, 594(%rsp)
	call	ufdbLogError
	movl	$0, 44(%rsp)
# SUCC: 19 [100.0%] 
	jmp	.L405
# BLOCK 59 freq:169 seq:57
# PRED: 5 [14.3%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L384:
	movq	$.LC88, 96(%rsp)
# SUCC: 10 [100.0%] 
	jmp	.L387
# BLOCK 60 freq:152 seq:58
# PRED: 12 [14.3%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L396:
	movq	$.LC88, 88(%rsp)
# SUCC: 17 [100.0%] 
	jmp	.L399
# BLOCK 61 freq:149 seq:59
# PRED: 17 [15.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L837:
	movl	108(%rsp), %edx
	movq	%r13, %rsi
	movl	$.LC113, %edi
	xorl	%eax, %eax
# SUCC: 62 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogError
# BLOCK 62 freq:210 seq:60
# PRED: 99 [100.0%]  61 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 280 [100.0%] 
.L404:
	movq	48(%rsp), %rax
	movl	$12, (%rax)
	movl	UFDBglobalHttpsOfficialCertificate(%rip), %edx
	testl	%edx, %edx
# SUCC: 86 [50.0%]  (CAN_FALLTHRU) 63 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L570
# BLOCK 63 freq:105 seq:61
# PRED: 62 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalHttpsEnforceContentPeek(%rip), %eax
	movl	$12, 104(%rsp)
	testl	%eax, %eax
# SUCC: 161 [4.3%]  (CAN_FALLTHRU) 64 [95.7%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L843
# BLOCK 64 freq:891 seq:62
# PRED: 279 [100.0%]  63 [95.7%]  (FALLTHRU,CAN_FALLTHRU) 86 [100.0%]  285 [100.0%] 
.L515:
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 123 [10.0%]  (CAN_FALLTHRU) 65 [90.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L844
# BLOCK 65 freq:802 seq:63
# PRED: 64 [90.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	80(%rsp), %rax
	leaq	2608(%rsp), %r12
	movq	%r13, %rdx
	movl	104(%rsp), %ecx
	movl	$.LC185, %esi
	movq	%r12, %rdi
	movq	%r12, %r14
	movl	%ecx, (%rax)
	movl	108(%rsp), %ecx
	xorl	%eax, %eax
	call	sprintf
# SUCC: 66 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	208(%rsp), %rbx
# BLOCK 66 freq:802 seq:64
# PRED: 65 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 66 [50.0%]  (DFS_BACK,CAN_FALLTHRU)
.L547:
	movl	(%r14), %edx
	addq	$4, %r14
	leal	-16843009(%rdx), %eax
	notl	%edx
	andl	%edx, %eax
	andl	$-2139062144, %eax
# SUCC: 66 [50.0%]  (DFS_BACK,CAN_FALLTHRU) 67 [50.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	je	.L547
# BLOCK 67 freq:401 seq:65
# PRED: 66 [50.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	movl	%eax, %edx
	shrl	$16, %edx
	testl	$32896, %eax
	cmove	%edx, %eax
	leaq	2(%r14), %rdx
	cmove	%rdx, %r14
	addb	%al, %al
	sbbq	$3, %r14
	call	__errno_location
	subl	%r12d, %r14d
	movq	%rax, %r13
	movl	$0, (%rax)
# SUCC: 70 [100.0%] 
	jmp	.L553
# BLOCK 68 freq:8909 seq:66
# PRED: 70 [97.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L845:
	movl	%eax, %esi
	movq	%rbx, %rdi
	call	SSL_get_error
	cmpl	$5, %eax
# SUCC: 69 [97.0%]  (FALLTHRU,CAN_FALLTHRU) 85 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L552
# BLOCK 69 freq:8642 seq:67
# PRED: 68 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$4, 0(%r13)
# SUCC: 70 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 85 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L552
# BLOCK 70 freq:9185 seq:68
# PRED: 69 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 67 [100.0%] 
.L553:
	movl	%r14d, %edx
	movq	%r12, %rsi
	movq	%rbx, %rdi
	call	SSL_write
	cmpl	$-1, %eax
# SUCC: 68 [97.0%]  (CAN_FALLTHRU) 71 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	je	.L845
# BLOCK 71 freq:276 seq:69
# PRED: 70 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	testl	%eax, %eax
# SUCC: 85 (CAN_FALLTHRU) 72 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L552
# BLOCK 72 freq:421 seq:70
# PRED: 71 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movb	$0, 4656(%rsp)
	movq	208(%rsp), %r12
	leaq	4656(%rsp), %rbx
	movl	$0, 0(%r13)
# SUCC: 75 [100.0%] 
	jmp	.L557
# BLOCK 73 freq:4676 seq:71
# PRED: 75 [97.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L846:
	movl	%eax, %esi
	movq	%r12, %rdi
	call	SSL_get_error
	cmpl	$5, %eax
# SUCC: 74 [97.0%]  (FALLTHRU,CAN_FALLTHRU) 80 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L551
# BLOCK 74 freq:4535 seq:72
# PRED: 73 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$4, 0(%r13)
# SUCC: 75 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 80 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L551
# BLOCK 75 freq:4820 seq:73
# PRED: 74 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 72 [100.0%] 
.L557:
	movl	$32768, %edx
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	SSL_read
	cmpl	$-1, %eax
# SUCC: 73 [97.0%]  (CAN_FALLTHRU) 76 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	je	.L846
# BLOCK 76 freq:145 seq:74
# PRED: 75 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	cmpl	$0, %eax
	movl	%eax, %r15d
# SUCC: 77 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 341 (CAN_FALLTHRU)
	jle	.L558
# BLOCK 77 freq:307 seq:75
# PRED: 76 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$1, UFDBglobalDebug(%rip)
	movslq	%eax, %r12
	movb	$0, 4656(%rsp,%r12)
# SUCC: 78 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 124 [27.0%]  (CAN_FALLTHRU)
	jle	.L847
# BLOCK 78 freq:266 seq:76
# PRED: 77 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 125 [100.0%] 
.L559:
	movl	$.LC186, %esi
	movq	%rbx, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 79 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 80 [15.0%]  (CAN_FALLTHRU)
	je	.L551
# BLOCK 79 freq:226 seq:77
# PRED: 78 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC187, %esi
	movq	%rbx, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 169 [6.7%]  (CAN_FALLTHRU) 80 [93.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L848
# BLOCK 80 freq:802 seq:78
# PRED: 341 (CAN_FALLTHRU) 73 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 174 [100.0%]  74 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 85 [100.0%]  124 [50.0%]  (CAN_FALLTHRU) 78 [15.0%]  (CAN_FALLTHRU) 79 [93.3%]  (FALLTHRU,CAN_FALLTHRU) 173 [27.0%]  (CAN_FALLTHRU) 342 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L551:
	movq	208(%rsp), %r12
	testq	%r12, %r12
# SUCC: 83 [21.6%]  (CAN_FALLTHRU) 81 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L562
# BLOCK 81 freq:629 seq:79
# PRED: 80 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r12, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 126 [13.1%]  (CAN_FALLTHRU) 82 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L849
# BLOCK 82 freq:629 seq:80
# PRED: 81 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 126 [100.0%] 
.L563:
	movq	%r12, %rdi
# SUCC: 83 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 83 freq:802 seq:81
# PRED: 82 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 80 [21.6%]  (CAN_FALLTHRU)
.L562:
	movq	%rbx, %rdi
# SUCC: 84 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbStrdup
# BLOCK 84 freq:2529 seq:82
# PRED: 83 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 116 [100.0%]  137 [100.0%]  321 [100.0%]  158 [100.0%]  48 [100.0%]  221 [100.0%]  159 [100.0%]  164 [100.0%]  214 [100.0%]  330 [100.0%] 
.L822:
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 85 freq:526 seq:83
# PRED: 68 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 71 (CAN_FALLTHRU) 69 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L552:
	.cfi_restore_state
	movabsq	$8367798494427701606, %rax
	movl	$76, %esi
	movq	%rax, 4656(%rsp)
	leaq	4656(%rsp), %rbx
	movabsq	$5125206801258848367, %rax
	movq	%rax, 4664(%rsp)
	movabsq	$8388366761067566149, %rax
	movq	%rax, 4672(%rsp)
	movabsq	$6004194763024375912, %rax
	movq	%rax, 4680(%rsp)
	movw	%si, 4688(%rsp)
# SUCC: 80 [100.0%] 
	jmp	.L551
# BLOCK 86 freq:178 seq:84
# PRED: 62 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L570:
	movl	108(%rsp), %ecx
	movq	%r13, %rdx
	movl	$.LC173, %edi
	movq	96(%rsp), %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 64 [100.0%] 
	jmp	.L515
# BLOCK 87 freq:148 seq:85
# PRED: 44 [17.6%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L422:
	leaq	1584(%rsp), %rdi
	movl	$.LC120, %esi
	call	strstr
	testq	%rax, %rax
	movq	%rax, 24(%rsp)
# SUCC: 88 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 138 [30.2%]  (CAN_FALLTHRU)
	je	.L424
# BLOCK 88 freq:104 seq:86
# PRED: 87 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
	movq	56(%rsp), %rdi
	xorl	%edx, %edx
	movl	$1023, %ecx
	leaq	4(%rax), %rsi
	call	memccpy
	testq	%rax, %rax
# SUCC: 168 [15.0%]  (CAN_FALLTHRU) 89 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L829
# BLOCK 89 freq:104 seq:87
# PRED: 88 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 167 [85.0%]  (CAN_FALLTHRU) 168 [100.0%] 
.L425:
	movq	56(%rsp), %rdi
	movl	$47, %esi
	call	strchr
	testq	%rax, %rax
# SUCC: 90 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 91 [15.0%]  (CAN_FALLTHRU)
	je	.L569
# BLOCK 90 freq:118 seq:88
# PRED: 89 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
# SUCC: 91 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movb	$0, (%rax)
# BLOCK 91 freq:148 seq:89
# PRED: 89 [15.0%]  (CAN_FALLTHRU) 90 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 139 [100.0%] 
.L569:
	cmpl	$1, 64(%rsp)
	movl	UFDBglobalDebug(%rip), %r10d
	setg	%al
	movzbl	%al, %ecx
	testl	%r10d, %r10d
	movl	%ecx, 40(%rsp)
# SUCC: 92 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 98 [50.0%]  (CAN_FALLTHRU)
	je	.L427
# BLOCK 92 freq:74 seq:90
# PRED: 91 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	testb	%al, %al
# SUCC: 93 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 94 [47.5%]  (CAN_FALLTHRU)
	je	.L428
# BLOCK 93 freq:39 seq:91
# PRED: 92 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	64(%rsp), %esi
	movq	%r13, %rdx
	movl	$.LC121, %edi
	xorl	%eax, %eax
# SUCC: 94 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 94 freq:74 seq:92
# PRED: 92 [47.5%]  (CAN_FALLTHRU) 93 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L428:
	movl	32(%rsp), %r8d
	movl	$.LC95, %r11d
	movl	$.LC16, %ebx
	movl	40(%rsp), %r9d
	movq	%r11, %rdx
	movl	$.LC96, %r12d
	movq	88(%rsp), %rcx
	movq	%r12, %rsi
	movl	$.LC122, %edi
	movq	%r11, 64(%rsp)
	testl	%r8d, %r8d
	movq	%r13, %r8
	cmove	%rbx, %rdx
	testl	%r9d, %r9d
	cmove	%rbx, %rsi
	xorl	%eax, %eax
	leaq	560(%rsp), %r9
	call	ufdbLogMessage
	cmpl	$1, UFDBglobalDebug(%rip)
	movq	64(%rsp), %r11
# SUCC: 95 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 129 [27.0%]  (CAN_FALLTHRU)
	jle	.L431
# BLOCK 95 freq:54 seq:93
# PRED: 94 [73.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpq	$0, 24(%rsp)
	movl	$.LC21, %eax
# SUCC: 96 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 97 [15.0%]  (CAN_FALLTHRU)
	je	.L432
# BLOCK 96 freq:46 seq:94
# PRED: 95 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	56(%rsp), %rcx
	cmpb	$0, (%rcx)
# SUCC: 175 [29.0%]  (CAN_FALLTHRU) 97 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L850
# BLOCK 97 freq:54 seq:95
# PRED: 96 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 95 [15.0%]  (CAN_FALLTHRU) 175 [100.0%] 
.L432:
	movl	32(%rsp), %esi
	movl	$.LC16, %ecx
	movl	$.LC95, %edx
	movl	40(%rsp), %edi
	movq	%rax, (%rsp)
	leaq	1584(%rsp), %r9
	movq	%r13, %r8
	testl	%esi, %esi
	movl	$.LC96, %esi
	cmove	%rcx, %rdx
	testl	%edi, %edi
	movl	$.LC123, %edi
	cmove	%rcx, %rsi
	movq	88(%rsp), %rcx
	xorl	%eax, %eax
# SUCC: 98 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 98 freq:148 seq:96
# PRED: 91 [50.0%]  (CAN_FALLTHRU) 97 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 129 [100.0%] 
.L427:
	movl	ufdbCacertsLoaded(%rip), %r11d
	testl	%r11d, %r11d
# SUCC: 99 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 131 [33.0%]  (CAN_FALLTHRU)
	jne	.L437
# BLOCK 99 freq:99 seq:97
# PRED: 98 [67.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	32(%rsp), %r9d
	movl	$.LC16, %eax
	movl	$.LC95, %edx
	movl	40(%rsp), %r10d
	movl	$.LC96, %esi
	movq	%r13, %rcx
	movl	108(%rsp), %r8d
	movl	$.LC125, %edi
	testl	%r9d, %r9d
	cmove	%rax, %rdx
	testl	%r10d, %r10d
	cmove	%rax, %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movq	%r14, %rdi
	call	X509_free
# SUCC: 62 [100.0%] 
	jmp	.L404
# BLOCK 100 freq:132 seq:98
# PRED: 4 [10.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L832:
	cmpl	$30, %eax
# SUCC: 140 [28.0%]  (CAN_FALLTHRU) 101 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L851
.L368:
# BLOCK 101 freq:101 seq:99
# PRED: 100 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	104(%rsp), %eax
	movl	$.LC70, %ecx
	cmpl	$22, %eax
# SUCC: 102 [74.0%]  (FALLTHRU,CAN_FALLTHRU) 116 [26.0%]  (CAN_FALLTHRU)
	je	.L360
# BLOCK 102 freq:105 seq:100
# PRED: 101 [74.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$20, %eax
# SUCC: 103 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 157 [28.0%]  (CAN_FALLTHRU)
	je	.L576
# BLOCK 103 freq:76 seq:101
# PRED: 102 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$17, %eax
# SUCC: 104 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 128 [28.0%]  (CAN_FALLTHRU)
	je	.L577
# BLOCK 104 freq:55 seq:102
# PRED: 103 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$21, %eax
	movl	$.LC74, %ecx
# SUCC: 105 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 116 [28.0%]  (CAN_FALLTHRU)
	je	.L360
# BLOCK 105 freq:39 seq:103
# PRED: 104 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$23, %eax
# SUCC: 106 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 177 [28.0%]  (CAN_FALLTHRU)
	je	.L579
# BLOCK 106 freq:28 seq:104
# PRED: 105 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$24, %eax
	movl	$.LC76, %ecx
# SUCC: 107 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 116 [28.0%]  (CAN_FALLTHRU)
	je	.L360
# BLOCK 107 freq:20 seq:105
# PRED: 106 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$25, %eax
	movl	$.LC77, %ecx
# SUCC: 108 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 116 [28.0%]  (CAN_FALLTHRU)
	je	.L360
# BLOCK 108 freq:15 seq:106
# PRED: 107 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$26, %eax
	movl	$.LC78, %ecx
# SUCC: 109 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 116 [28.0%]  (CAN_FALLTHRU)
	je	.L360
# BLOCK 109 freq:11 seq:107
# PRED: 108 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$19, %eax
# SUCC: 110 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 191 [28.0%]  (CAN_FALLTHRU)
	je	.L583
# BLOCK 110 freq:8 seq:108
# PRED: 109 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$2, %eax
	movl	$.LC80, %ecx
# SUCC: 111 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 116 [28.0%]  (CAN_FALLTHRU)
	je	.L360
# BLOCK 111 freq:5 seq:109
# PRED: 110 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$11, %eax
# SUCC: 112 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 193 [28.0%]  (CAN_FALLTHRU)
	je	.L585
# BLOCK 112 freq:4 seq:110
# PRED: 111 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$8, %eax
# SUCC: 113 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 155 [28.0%]  (CAN_FALLTHRU)
	je	.L586
# BLOCK 113 freq:3 seq:111
# PRED: 112 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$30, %eax
	.p2align 4,,2
# SUCC: 114 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 197 [28.0%]  (CAN_FALLTHRU)
	je	.L587
# BLOCK 114 freq:2 seq:112
# PRED: 113 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$29, %eax
	movl	$.LC84, %ecx
# SUCC: 115 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 116 [28.0%]  (CAN_FALLTHRU)
	je	.L360
# BLOCK 115 freq:1 seq:113
# PRED: 114 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$13, %eax
	movl	$.LC85, %ecx
	movl	$.LC71, %eax
# SUCC: 116 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmovne	%rax, %rcx
# BLOCK 116 freq:132 seq:114
# PRED: 140 [71.0%]  (CAN_FALLTHRU) 114 [28.0%]  (CAN_FALLTHRU) 101 [26.0%]  (CAN_FALLTHRU) 115 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 128 [100.0%]  104 [28.0%]  (CAN_FALLTHRU) 193 [100.0%]  106 [28.0%]  (CAN_FALLTHRU) 107 [28.0%]  (CAN_FALLTHRU) 108 [28.0%]  (CAN_FALLTHRU) 155 [100.0%]  110 [28.0%]  (CAN_FALLTHRU) 157 [100.0%]  177 [100.0%]  191 [100.0%]  197 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L360:
	movl	108(%rsp), %edx
	xorl	%eax, %eax
	movq	%r13, %rsi
	movl	$.LC110, %edi
	call	ufdbLogMessage
	movq	80(%rsp), %rax
	movl	104(%rsp), %ecx
	movl	%ecx, (%rax)
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 117 freq:126 seq:115
# PRED: 54 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L595:
	xorl	%r15d, %r15d
# SUCC: 44 [100.0%] 
	jmp	.L417
# BLOCK 118 freq:118 seq:116
# PRED: 11 [10.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L835:
	movl	108(%rsp), %ecx
	movq	%r13, %rdx
	movl	$.LC112, %edi
# SUCC: 119 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	96(%rsp), %rsi
# BLOCK 119 freq:207 seq:117
# PRED: 118 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 123 [100.0%] 
.L828:
	xorl	%eax, %eax
	call	ufdbLogMessage
	movq	208(%rsp), %rbx
	testq	%rbx, %rbx
# SUCC: 122 [21.6%]  (CAN_FALLTHRU) 120 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L545
# BLOCK 120 freq:163 seq:118
# PRED: 119 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 160 [13.1%]  (CAN_FALLTHRU) 121 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L852
# BLOCK 121 freq:163 seq:119
# PRED: 120 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 160 [100.0%] 
.L546:
	movq	%rbx, %rdi
# SUCC: 122 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 122 freq:207 seq:120
# PRED: 121 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 119 [21.6%]  (CAN_FALLTHRU)
.L545:
	movq	80(%rsp), %rax
	movl	$28, (%rax)
	leaq	-40(%rbp), %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 123 freq:89 seq:121
# PRED: 64 [10.0%]  (CAN_FALLTHRU)
.L844:
	.cfi_restore_state
	movl	108(%rsp), %ecx
	movq	%r13, %rdx
	movl	$.LC184, %edi
	movq	96(%rsp), %rsi
# SUCC: 119 [100.0%] 
	jmp	.L828
# BLOCK 124 freq:83 seq:122
# PRED: 77 [27.0%]  (CAN_FALLTHRU)
.L847:
	movl	UFDBglobalPeek(%rip), %ecx
	testl	%ecx, %ecx
# SUCC: 125 [50.0%]  (FALLTHRU) 80 [50.0%]  (CAN_FALLTHRU)
	je	.L551
# BLOCK 125 freq:42 seq:123
# PRED: 124 [50.0%]  (FALLTHRU)
# SUCC: 78 [100.0%] 
	jmp	.L559
# BLOCK 126 freq:82 seq:124
# PRED: 81 [13.1%]  (CAN_FALLTHRU)
.L849:
	call	ERR_clear_error
	.p2align 4,,8
# SUCC: 82 [100.0%] 
	jmp	.L563
# BLOCK 127 seq:125
# PRED: 289 [9.2%]  (CAN_FALLTHRU)
.L874:
	movl	108(%rsp), %edx
	movq	%r13, %rsi
	movl	$.LC106, %edi
	xorl	%eax, %eax
# SUCC: 128 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 128 freq:21 seq:126
# PRED: 103 [28.0%]  (CAN_FALLTHRU) 127 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L577:
	movl	$.LC73, %ecx
# SUCC: 116 [100.0%] 
	jmp	.L360
# BLOCK 129 freq:20 seq:127
# PRED: 94 [27.0%]  (CAN_FALLTHRU)
.L431:
	movl	32(%rsp), %eax
	movq	%r11, %rdx
	movq	%r12, %rsi
	movq	88(%rsp), %rcx
	leaq	1584(%rsp), %r9
	movq	%r13, %r8
	movl	$.LC124, %edi
	testl	%eax, %eax
	movl	40(%rsp), %eax
	cmove	%rbx, %rdx
	testl	%eax, %eax
	cmove	%rbx, %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 98 [100.0%] 
	jmp	.L427
# BLOCK 130 freq:2 seq:128
# PRED: 46 [13.1%]  (CAN_FALLTHRU)
.L842:
	call	ERR_clear_error
	.p2align 4,,8
# SUCC: 47 [100.0%] 
	jmp	.L517
# BLOCK 131 freq:49 seq:129
# PRED: 98 [33.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L437:
	movq	56(%rsp), %rsi
	movl	$.LC126, %edi
	movl	$8, %ecx
	repz cmpsb
# SUCC: 133 [45.3%]  (CAN_FALLTHRU) 132 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L440
# BLOCK 132 freq:27 seq:130
# PRED: 131 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	movq	56(%rsp), %rsi
	movl	$.LC127, %edi
	movl	$12, %ecx
	repz cmpsb
# SUCC: 133 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 178 [33.0%]  (CAN_FALLTHRU)
	jne	.L441
# BLOCK 133 freq:40 seq:131
# PRED: 132 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 131 [45.3%]  (CAN_FALLTHRU)
.L440:
	movl	108(%rsp), %ecx
	movl	$.LC128, %edi
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	leaq	1584(%rsp), %r9
	xorl	%eax, %eax
	leaq	560(%rsp), %r8
	call	ufdbLogMessage
	movq	%r14, %rdi
# SUCC: 134 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	X509_free
# BLOCK 134 freq:16 seq:132
# PRED: 133 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 180 [100.0%]  315 [100.0%] 
.L811:
	movl	108(%rsp), %ecx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movq	96(%rsp), %rsi
	movl	$.LC182, %edi
	call	ufdbLogMessage
	movq	208(%rsp), %rbx
	testq	%rbx, %rbx
# SUCC: 137 [21.6%]  (CAN_FALLTHRU) 135 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L540
# BLOCK 135 freq:13 seq:133
# PRED: 134 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 196 [13.1%]  (CAN_FALLTHRU) 136 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L853
# BLOCK 136 freq:13 seq:134
# PRED: 135 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 196 [100.0%] 
.L541:
	movq	%rbx, %rdi
# SUCC: 137 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 137 freq:16 seq:135
# PRED: 136 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 134 [21.6%]  (CAN_FALLTHRU)
.L540:
	movq	80(%rsp), %rax
	movl	$11, (%rax)
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 138 freq:45 seq:136
# PRED: 87 [30.2%]  (CAN_FALLTHRU)
.L424:
	cmpb	$67, 1584(%rsp)
# SUCC: 165 [28.0%]  (CAN_FALLTHRU) 139 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L854
# BLOCK 139 freq:44 seq:137
# PRED: 165 [72.0%]  (CAN_FALLTHRU) 138 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 166 [86.3%]  (CAN_FALLTHRU)
.L426:
	movabsq	$7163369285420737891, %rcx
	movq	56(%rsp), %rax
	movq	%rcx, (%rax)
	movabsq	$7665818653063214177, %rcx
	movq	%rcx, 8(%rax)
	movabsq	$2338319731384410981, %rcx
	movq	%rcx, 16(%rax)
	movabsq	$8029194035306458990, %rcx
	movq	%rcx, 24(%rax)
	movabsq	$3398001538849918574, %rcx
	movq	%rcx, 32(%rax)
	movl	$691883587, 40(%rax)
	movb	$0, 44(%rax)
# SUCC: 91 [100.0%] 
	jmp	.L569
# BLOCK 140 freq:37 seq:138
# PRED: 100 [28.0%]  (CAN_FALLTHRU)
.L851:
	movl	UFDBglobalReconfig(%rip), %eax
	movl	$.LC83, %ecx
	testl	%eax, %eax
# SUCC: 141 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 116 [71.0%]  (CAN_FALLTHRU)
	jne	.L360
# BLOCK 141 freq:11 seq:139
# PRED: 140 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %edx
	testl	%edx, %edx
# SUCC: 194 [29.0%]  (CAN_FALLTHRU) 142 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L855
# BLOCK 142 freq:11 seq:140
# PRED: 141 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 194 [100.0%] 
.L361:
	movsbq	0(%r13), %r12
	testb	%r12b, %r12b
# SUCC: 143 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 147 [4.5%]  (CAN_FALLTHRU)
	je	.L362
# BLOCK 143 freq:11 seq:141
# PRED: 142 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r13, %r14
# SUCC: 144 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movsbq	%r12b, %rbx
# BLOCK 144 freq:121 seq:142
# PRED: 146 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 143 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L367:
	cmpb	$46, %bl
# SUCC: 145 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 146 [28.0%]  (CAN_FALLTHRU)
	je	.L363
# BLOCK 145 freq:87 seq:143
# PRED: 144 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	__ctype_b_loc
	movq	(%rax), %rax
	testb	$8, 1(%rax,%rbx,2)
# SUCC: 181 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 146 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L856
# BLOCK 146 freq:117 seq:144
# PRED: 145 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 144 [28.0%]  (CAN_FALLTHRU)
.L363:
	addq	$1, %r14
	movsbq	(%r14), %rbx
	testb	%bl, %bl
# SUCC: 144 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 147 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L367
# BLOCK 147 freq:8 seq:145
# PRED: 182 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 142 [4.5%]  (CAN_FALLTHRU) 146 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
.L362:
	movl	108(%rsp), %esi
	movq	%r13, %rdi
	call	UFDBdetectGtalk
	cmpl	$20, %eax
	movl	%eax, 104(%rsp)
# SUCC: 156 [9.2%]  (CAN_FALLTHRU) 148 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L857
# BLOCK 148 freq:3 seq:146
# PRED: 147 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$23, 104(%rsp)
# SUCC: 176 [9.2%]  (CAN_FALLTHRU) 149 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L858
# BLOCK 149 freq:3 seq:147
# PRED: 148 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$11, 104(%rsp)
# SUCC: 288 [9.2%]  (CAN_FALLTHRU) 150 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L374
# BLOCK 150 freq:2 seq:148
# PRED: 149 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	movl	104(%rsp), %eax
	cmpl	$30, %eax
# SUCC: 197 [19.9%]  (CAN_FALLTHRU) 151 [80.1%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L587
# BLOCK 151 freq:2 seq:149
# PRED: 150 [80.1%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$8, %eax
# SUCC: 155 [19.9%]  (CAN_FALLTHRU) 152 [80.1%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L586
# BLOCK 152 freq:1 seq:150
# PRED: 151 [80.1%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 154 [45.3%]  (CAN_FALLTHRU) 153 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L371
# BLOCK 153 freq:1 seq:151
# PRED: 152 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalTerminating(%rip), %eax
	testl	%eax, %eax
# SUCC: 154 [83.2%]  (FALLTHRU,CAN_FALLTHRU) 289 [16.8%]  (CAN_FALLTHRU)
	je	.L372
# BLOCK 154 freq:1 seq:152
# PRED: 153 [83.2%]  (FALLTHRU,CAN_FALLTHRU) 152 [45.3%]  (CAN_FALLTHRU)
.L371:
	movl	$.LC105, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 155 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$8, 104(%rsp)
# BLOCK 155 freq:1 seq:153
# PRED: 112 [28.0%]  (CAN_FALLTHRU) 151 [19.9%]  (CAN_FALLTHRU) 154 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L586:
	movl	$.LC82, %ecx
# SUCC: 116 [100.0%] 
	jmp	.L360
# BLOCK 156 seq:154
# PRED: 147 [9.2%]  (CAN_FALLTHRU)
.L857:
	movl	108(%rsp), %edx
	movq	%r13, %rsi
	movl	$.LC102, %edi
	xorl	%eax, %eax
# SUCC: 157 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 157 freq:29 seq:155
# PRED: 102 [28.0%]  (CAN_FALLTHRU) 156 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L576:
	movl	$.LC72, %ecx
# SUCC: 116 [100.0%] 
	jmp	.L360
# BLOCK 158 freq:26 seq:156
# PRED: 2 [1.9%]  (CAN_FALLTHRU)
.L830:
	xorl	%eax, %eax
	movl	$29, (%r9)
	movl	$.LC99, %edi
	call	ufdbLogError
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 159 freq:26 seq:157
# PRED: 3 [1.9%]  (CAN_FALLTHRU)
.L831:
	movq	80(%rsp), %rax
	movl	$.LC100, %edi
	movl	$29, (%rax)
	xorl	%eax, %eax
	call	ufdbLogError
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 160 freq:21 seq:158
# PRED: 120 [13.1%]  (CAN_FALLTHRU)
.L852:
	call	ERR_clear_error
	.p2align 4,,6
# SUCC: 121 [100.0%] 
	jmp	.L546
# BLOCK 161 freq:20 seq:159
# PRED: 63 [4.3%]  (CAN_FALLTHRU)
.L843:
	movl	108(%rsp), %ecx
	xorl	%eax, %eax
	movl	$12, %r8d
	movq	96(%rsp), %rsi
	movq	%r13, %rdx
	movl	$.LC183, %edi
	call	ufdbLogMessage
	movq	208(%rsp), %rbx
	testq	%rbx, %rbx
# SUCC: 164 [21.6%]  (CAN_FALLTHRU) 162 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L542
# BLOCK 162 freq:15 seq:160
# PRED: 161 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 195 [13.1%]  (CAN_FALLTHRU) 163 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L859
# BLOCK 163 freq:15 seq:161
# PRED: 162 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 195 [100.0%] 
.L543:
	movq	%rbx, %rdi
# SUCC: 164 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 164 freq:20 seq:162
# PRED: 163 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 161 [21.6%]  (CAN_FALLTHRU)
.L542:
	movq	80(%rsp), %rax
	movl	$12, (%rax)
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 165 freq:13 seq:163
# PRED: 138 [28.0%]  (CAN_FALLTHRU)
.L854:
	cmpb	$78, 1585(%rsp)
# SUCC: 166 [28.0%]  (FALLTHRU,CAN_FALLTHRU) 139 [72.0%]  (CAN_FALLTHRU)
	jne	.L426
# BLOCK 166 freq:4 seq:164
# PRED: 165 [28.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpb	$61, 1586(%rsp)
# SUCC: 167 [13.7%]  (FALLTHRU,CAN_FALLTHRU) 139 [86.3%]  (CAN_FALLTHRU)
	jne	.L426
# BLOCK 167 seq:165
# PRED: 166 [13.7%]  (FALLTHRU,CAN_FALLTHRU)
	movq	56(%rsp), %rdi
	movl	$1023, %ecx
	xorl	%edx, %edx
	leaq	1587(%rsp), %rsi
	call	memccpy
	leaq	1584(%rsp), %rcx
	testq	%rax, %rax
	movq	%rcx, 24(%rsp)
# SUCC: 168 [15.0%]  (FALLTHRU,CAN_FALLTHRU) 89 [85.0%]  (CAN_FALLTHRU)
	jne	.L425
# BLOCK 168 freq:16 seq:166
# PRED: 167 [15.0%]  (FALLTHRU,CAN_FALLTHRU) 88 [15.0%]  (CAN_FALLTHRU)
.L829:
	movq	56(%rsp), %rax
	movb	$0, 1022(%rax)
# SUCC: 89 [100.0%] 
	jmp	.L425
# BLOCK 169 freq:15 seq:167
# PRED: 79 [6.7%]  (CAN_FALLTHRU)
.L848:
	movl	$150000, %edi
	call	usleep
	movl	$32768, %ecx
	movl	%r15d, 108(%rsp)
	subl	%r15d, %ecx
	movq	%rbx, 96(%rsp)
	movq	208(%rsp), %r14
	leaq	(%rbx,%r12), %rax
	movl	$0, 0(%r13)
	movl	%ecx, %r15d
	movq	%rax, %rbx
# SUCC: 172 [100.0%] 
	jmp	.L561
# BLOCK 170 freq:169 seq:168
# PRED: 172 [97.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L860:
	movl	%eax, %esi
	movq	%r14, %rdi
	call	SSL_get_error
	cmpl	$5, %eax
# SUCC: 171 [97.0%]  (FALLTHRU,CAN_FALLTHRU) 173 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L560
# BLOCK 171 freq:164 seq:169
# PRED: 170 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$4, 0(%r13)
# SUCC: 172 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 173 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L560
# BLOCK 172 freq:174 seq:170
# PRED: 171 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 169 [100.0%] 
.L561:
	movl	%r15d, %edx
	movq	%rbx, %rsi
	movq	%r14, %rdi
	call	SSL_read
	cmpl	$-1, %eax
	movl	%eax, %r12d
# SUCC: 170 [97.0%]  (CAN_FALLTHRU) 173 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	je	.L860
# BLOCK 173 freq:15 seq:171
# PRED: 172 [3.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 171 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 170 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L560:
	movl	108(%rsp), %r15d
	xorl	%eax, %eax
	movl	%r12d, %edx
	movl	$.LC188, %edi
	movq	96(%rsp), %rbx
	movl	%r12d, 112(%rsp)
	movl	%r15d, %esi
	call	ufdbLogMessage
	movl	112(%rsp), %eax
	testl	%eax, %eax
# SUCC: 174 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 80 [27.0%]  (CAN_FALLTHRU)
	jle	.L551
# BLOCK 174 freq:11 seq:172
# PRED: 173 [73.0%]  (FALLTHRU,CAN_FALLTHRU)
	addl	%r15d, %eax
	cltq
	movb	$0, 4656(%rsp,%rax)
# SUCC: 80 [100.0%] 
	jmp	.L551
# BLOCK 175 freq:13 seq:173
# PRED: 96 [29.0%]  (CAN_FALLTHRU)
.L850:
	movq	%rcx, %rdi
	call	ufdbCategoryName
# SUCC: 97 [100.0%] 
	jmp	.L432
# BLOCK 176 seq:174
# PRED: 148 [9.2%]  (CAN_FALLTHRU)
.L858:
	movl	108(%rsp), %edx
	movq	%r13, %rsi
	movl	$.LC103, %edi
	xorl	%eax, %eax
# SUCC: 177 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 177 freq:11 seq:175
# PRED: 105 [28.0%]  (CAN_FALLTHRU) 176 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L579:
	movl	$.LC75, %ecx
# SUCC: 116 [100.0%] 
	jmp	.L360
# BLOCK 178 freq:9 seq:176
# PRED: 132 [33.0%]  (CAN_FALLTHRU)
.L441:
	movq	24(%rsp), %rdi
	testq	%rdi, %rdi
# SUCC: 179 [93.3%]  (FALLTHRU,CAN_FALLTHRU) 207 [6.7%]  (CAN_FALLTHRU)
	je	.L442
# BLOCK 179 freq:8 seq:177
# PRED: 178 [93.3%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC129, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 180 [82.5%]  (FALLTHRU,CAN_FALLTHRU) 206 [17.6%]  (CAN_FALLTHRU)
	je	.L861
# BLOCK 180 freq:8 seq:178
# PRED: 206 [92.0%]  (CAN_FALLTHRU) 179 [82.5%]  (FALLTHRU,CAN_FALLTHRU)
.L443:
	movl	108(%rsp), %ecx
	leaq	1584(%rsp), %r9
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	leaq	560(%rsp), %r8
	xorl	%eax, %eax
	movl	$.LC131, %edi
	call	ufdbLogMessage
	movq	%r14, %rdi
	call	X509_free
# SUCC: 134 [100.0%] 
	jmp	.L811
# BLOCK 181 freq:4 seq:179
# PRED: 145 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
.L856:
	movq	%r13, %rcx
	movsbq	%r12b, %rdx
# SUCC: 183 [100.0%] 
	jmp	.L366
# BLOCK 182 freq:48 seq:180
# PRED: 184 [95.5%]  (CAN_FALLTHRU) 183 [28.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L364:
	addq	$1, %rcx
	movsbq	(%rcx), %rdx
	testb	%dl, %dl
# SUCC: 183 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 147 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L362
# BLOCK 183 freq:49 seq:181
# PRED: 182 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 181 [100.0%] 
.L366:
	cmpb	$58, %dl
# SUCC: 184 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 182 [28.0%]  (CAN_FALLTHRU)
	je	.L364
# BLOCK 184 freq:35 seq:182
# PRED: 183 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	testb	$16, 1(%rax,%rdx,2)
# SUCC: 185 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 182 [95.5%]  (CAN_FALLTHRU)
	jne	.L364
# BLOCK 185 freq:4 seq:183
# PRED: 184 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 292 [91.0%]  (CAN_FALLTHRU)
.L365:
	movq	%r13, %r14
# SUCC: 186 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movsbq	%r12b, %rbx
# BLOCK 186 freq:45 seq:184
# PRED: 188 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 185 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L381:
	cmpb	$46, %bl
# SUCC: 187 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 188 [28.0%]  (CAN_FALLTHRU)
	je	.L377
# BLOCK 187 freq:32 seq:185
# PRED: 186 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	__ctype_b_loc
	movq	(%rax), %rax
	testb	$8, 1(%rax,%rbx,2)
# SUCC: 198 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 188 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L862
# BLOCK 188 freq:43 seq:186
# PRED: 187 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 186 [28.0%]  (CAN_FALLTHRU)
.L377:
	addq	$1, %r14
	movsbq	(%r14), %rbx
	testb	%bl, %bl
# SUCC: 186 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 189 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L381
# BLOCK 189 freq:5 seq:187
# PRED: 188 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 202 [50.0%]  (CAN_FALLTHRU) 199 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 203 [71.0%]  (CAN_FALLTHRU) 204 [75.6%]  (CAN_FALLTHRU) 291 [48.8%]  (CAN_FALLTHRU) 293 [100.0%] 
.L376:
	movl	UFDBglobalUnknownProtocolOverHttps(%rip), %r15d
	movq	%r13, %rsi
	movl	108(%rsp), %edx
	testl	%r15d, %r15d
# SUCC: 190 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 192 [50.0%]  (CAN_FALLTHRU)
	je	.L382
# BLOCK 190 freq:4 seq:188
# PRED: 189 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC108, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 191 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$19, 104(%rsp)
# BLOCK 191 freq:3 seq:189
# PRED: 109 [28.0%]  (CAN_FALLTHRU) 190 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L583:
	movl	$.LC79, %ecx
# SUCC: 116 [100.0%] 
	jmp	.L360
# BLOCK 192 freq:4 seq:190
# PRED: 189 [50.0%]  (CAN_FALLTHRU)
.L382:
	movl	$.LC109, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 193 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$11, 104(%rsp)
# BLOCK 193 freq:1 seq:191
# PRED: 111 [28.0%]  (CAN_FALLTHRU) 192 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 205 [100.0%]  288 [100.0%] 
.L585:
	movl	$.LC81, %ecx
# SUCC: 116 [100.0%] 
	jmp	.L360
# BLOCK 194 freq:3 seq:192
# PRED: 141 [29.0%]  (CAN_FALLTHRU)
.L855:
	movl	108(%rsp), %edx
	movq	%r13, %rsi
	movl	$.LC101, %edi
	call	ufdbLogMessage
# SUCC: 142 [100.0%] 
	jmp	.L361
# BLOCK 195 freq:2 seq:193
# PRED: 162 [13.1%]  (CAN_FALLTHRU)
.L859:
	call	ERR_clear_error
	.p2align 4,,8
# SUCC: 163 [100.0%] 
	jmp	.L543
# BLOCK 196 freq:2 seq:194
# PRED: 135 [13.1%]  (CAN_FALLTHRU)
.L853:
	.p2align 4,,8
	call	ERR_clear_error
	.p2align 4,,8
# SUCC: 136 [100.0%] 
	jmp	.L541
# BLOCK 197 freq:1 seq:195
# PRED: 113 [28.0%]  (CAN_FALLTHRU) 150 [19.9%]  (CAN_FALLTHRU)
.L587:
	movl	$.LC83, %ecx
	.p2align 4,,3
# SUCC: 116 [100.0%] 
	jmp	.L360
# BLOCK 198 freq:1 seq:196
# PRED: 187 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
.L862:
	movq	%r13, %rdx
	.p2align 4,,3
# SUCC: 200 [100.0%] 
	jmp	.L380
# BLOCK 199 freq:18 seq:197
# PRED: 201 [95.5%]  (CAN_FALLTHRU) 200 [28.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L378:
	addq	$1, %rdx
	movsbq	(%rdx), %r12
	testb	%r12b, %r12b
# SUCC: 200 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 189 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L376
# BLOCK 200 freq:18 seq:198
# PRED: 199 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 198 [100.0%] 
.L380:
	cmpb	$58, %r12b
# SUCC: 201 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 199 [28.0%]  (CAN_FALLTHRU)
	je	.L378
# BLOCK 201 freq:13 seq:199
# PRED: 200 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	testb	$16, 1(%rax,%r12,2)
# SUCC: 202 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 199 [95.5%]  (CAN_FALLTHRU)
	jne	.L378
# BLOCK 202 freq:1 seq:200
# PRED: 201 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 203 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 189 [50.0%]  (CAN_FALLTHRU)
	jne	.L376
# BLOCK 203 freq:1 seq:201
# PRED: 202 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalTerminating(%rip), %eax
	testl	%eax, %eax
# SUCC: 204 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 189 [71.0%]  (CAN_FALLTHRU)
	jne	.L376
# BLOCK 204 seq:202
# PRED: 203 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %esi
	movq	%r13, %rdi
	call	UFDBdetectSSH
	cmpl	$11, %eax
# SUCC: 205 [24.4%]  (FALLTHRU,CAN_FALLTHRU) 189 [75.6%]  (CAN_FALLTHRU)
	jne	.L376
# BLOCK 205 seq:203
# PRED: 204 [24.4%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %edx
	movq	%r13, %rsi
	movl	$.LC107, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$11, 104(%rsp)
# SUCC: 193 [100.0%] 
	jmp	.L585
# BLOCK 206 freq:1 seq:204
# PRED: 179 [17.6%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L861:
	movq	24(%rsp), %rdi
	movl	$.LC130, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 180 [92.0%]  (CAN_FALLTHRU) 207 [8.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L443
# BLOCK 207 freq:1 seq:205
# PRED: 206 [8.0%]  (FALLTHRU,CAN_FALLTHRU) 178 [6.7%]  (CAN_FALLTHRU)
.L442:
	movq	72(%rsp), %rdi
	call	SSL_get_verify_result
	cmpq	$20, %rax
	movq	%rax, %rbx
# SUCC: 296 [28.0%]  (CAN_FALLTHRU) 208 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L863
# BLOCK 208 freq:1 seq:206
# PRED: 207 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	testq	%rax, %rax
# SUCC: 209 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 294 [50.0%]  (CAN_FALLTHRU)
	jne	.L454
# BLOCK 209 seq:207
# PRED: 208 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	560(%rsp), %rdi
	movl	$.LC140, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 210 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 216 [15.0%]  (CAN_FALLTHRU)
	je	.L455
# BLOCK 210 seq:208
# PRED: 209 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	1584(%rsp), %rdi
	movl	$.LC141, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 211 [82.5%]  (FALLTHRU,CAN_FALLTHRU) 216 [17.6%]  (CAN_FALLTHRU)
	je	.L455
# BLOCK 211 seq:209
# PRED: 210 [82.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %ebx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	leaq	1584(%rsp), %r9
	movl	$.LC142, %edi
	leaq	560(%rsp), %r8
	movl	%ebx, %ecx
	call	ufdbLogMessage
	movq	%r14, %rdi
	call	X509_free
	movq	96(%rsp), %rsi
	movl	%ebx, %ecx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movl	$.LC176, %edi
	call	ufdbLogMessage
	movq	208(%rsp), %rbx
	testq	%rbx, %rbx
# SUCC: 214 [21.6%]  (CAN_FALLTHRU) 212 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L522
# BLOCK 212 freq:14 seq:210
# PRED: 211 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 215 [13.1%]  (CAN_FALLTHRU) 213 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L864
# BLOCK 213 freq:14 seq:211
# PRED: 212 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 215 [100.0%] 
.L523:
	movq	%rbx, %rdi
# SUCC: 214 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 214 freq:18 seq:212
# PRED: 213 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 211 [21.6%]  (CAN_FALLTHRU)
.L522:
	movq	80(%rsp), %rax
	movl	$17, (%rax)
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 215 freq:2 seq:213
# PRED: 212 [13.1%]  (CAN_FALLTHRU)
.L864:
	call	ERR_clear_error
# SUCC: 213 [100.0%] 
	jmp	.L523
# BLOCK 216 seq:214
# PRED: 210 [17.6%]  (CAN_FALLTHRU) 209 [15.0%]  (CAN_FALLTHRU)
.L455:
	cmpl	$0, 40(%rsp)
	.p2align 4,,3
# SUCC: 217 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 224 [50.0%]  (CAN_FALLTHRU)
	je	.L453
# BLOCK 217 seq:215
# PRED: 216 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	1584(%rsp), %rdi
	movl	$.LC143, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 218 [82.5%]  (FALLTHRU,CAN_FALLTHRU) 224 [17.6%]  (CAN_FALLTHRU)
	je	.L453
# BLOCK 218 seq:216
# PRED: 324 [82.5%]  (CAN_FALLTHRU) 217 [82.5%]  (FALLTHRU,CAN_FALLTHRU)
.L824:
	movl	108(%rsp), %ecx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	leaq	1584(%rsp), %r9
	movl	$.LC144, %edi
	leaq	560(%rsp), %r8
	call	ufdbLogMessage
	movq	%r14, %rdi
	call	X509_free
	movl	108(%rsp), %ecx
	xorl	%eax, %eax
	movq	96(%rsp), %rsi
	movq	%r13, %rdx
	movl	$.LC179, %edi
	call	ufdbLogMessage
	movq	208(%rsp), %rbx
	testq	%rbx, %rbx
# SUCC: 221 [21.6%]  (CAN_FALLTHRU) 219 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L531
# BLOCK 219 freq:13 seq:217
# PRED: 218 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 222 [13.1%]  (CAN_FALLTHRU) 220 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L865
# BLOCK 220 freq:13 seq:218
# PRED: 219 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 222 [100.0%] 
.L532:
	movq	%rbx, %rdi
# SUCC: 221 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 221 freq:17 seq:219
# PRED: 220 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 218 [21.6%]  (CAN_FALLTHRU)
.L531:
	movq	80(%rsp), %rax
	movl	$24, (%rax)
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 222 freq:2 seq:220
# PRED: 219 [13.1%]  (CAN_FALLTHRU)
.L865:
	call	ERR_clear_error
# SUCC: 220 [100.0%] 
	jmp	.L532
# BLOCK 223 seq:221
# PRED: 296 [29.0%]  (CAN_FALLTHRU)
.L445:
	movl	108(%rsp), %edx
	movl	$UFDBglobalCAcertsFile, %ecx
	movq	%r13, %rsi
	movl	$.LC139, %edi
	xorl	%eax, %eax
# SUCC: 224 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogError
# BLOCK 224 seq:222
# PRED: 217 [17.6%]  (CAN_FALLTHRU) 216 [50.0%]  (CAN_FALLTHRU) 223 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L453:
	cmpl	$2, 44(%rsp)
# SUCC: 225 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 227 [27.0%]  (CAN_FALLTHRU)
	jne	.L476
# BLOCK 225 seq:223
# PRED: 224 [73.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebug(%rip)
# SUCC: 226 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 227 [50.0%]  (CAN_FALLTHRU)
	je	.L476
# BLOCK 226 seq:224
# PRED: 225 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, 32(%rsp)
	movl	$.LC16, %eax
	movl	$.LC95, %edx
	movl	108(%rsp), %r9d
	movl	$.LC96, %esi
	movq	%r13, %r8
	movq	88(%rsp), %rcx
	movl	$.LC159, %edi
	cmove	%rax, %rdx
	cmpl	$0, 40(%rsp)
	cmove	%rax, %rsi
	xorl	%eax, %eax
# SUCC: 227 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 227 seq:225
# PRED: 303 [50.0%]  (CAN_FALLTHRU) 225 [50.0%]  (CAN_FALLTHRU) 226 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 224 [27.0%]  (CAN_FALLTHRU) 299 [50.0%]  (CAN_FALLTHRU) 338 [100.0%]  339 [100.0%]  304 [100.0%]  316 [100.0%]  332 [100.0%]  325 [100.0%]  300 [100.0%]  337 [100.0%]  334 [100.0%]  333 [100.0%]  336 [100.0%]  340 [100.0%] 
.L476:
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r14, %rdi
	movl	$85, %esi
	call	X509_get_ext_d2i
	testq	%rax, %rax
	movq	%rax, %rdi
	movq	%rax, 72(%rsp)
# SUCC: 228 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 258 [30.2%]  (CAN_FALLTHRU)
	je	.L479
# BLOCK 228 seq:226
# PRED: 227 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
	call	sk_num
	testl	%eax, %eax
	movl	%eax, 24(%rsp)
# SUCC: 229 [97.0%]  (FALLTHRU,CAN_FALLTHRU) 286 [3.0%]  (CAN_FALLTHRU)
	jle	.L480
# BLOCK 229 seq:227
# PRED: 228 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%r12d, %r12d
	movl	$0, 40(%rsp)
	movl	$.LC97, %ebx
	movq	$0, 56(%rsp)
	movl	$0, 64(%rsp)
	movq	%r14, 32(%rsp)
# SUCC: 231 [100.0%] 
	jmp	.L493
# BLOCK 230 freq:9 seq:228
# PRED: 231 [86.3%]  (CAN_FALLTHRU) 250 [100.0%]  253 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L481:
	addl	$1, 64(%rsp)
	movl	24(%rsp), %eax
	cmpl	%eax, 64(%rsp)
# SUCC: 231 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 343 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L866
# BLOCK 231 freq:9 seq:229
# PRED: 230 [97.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 229 [100.0%] 
.L493:
	movl	64(%rsp), %esi
	movq	72(%rsp), %rdi
	call	sk_value
	cmpl	$2, (%rax)
	movq	%rax, %r14
# SUCC: 232 [13.7%]  (FALLTHRU,CAN_FALLTHRU) 230 [86.3%]  (CAN_FALLTHRU)
	jne	.L481
# BLOCK 232 freq:1 seq:230
# PRED: 231 [13.7%]  (FALLTHRU,CAN_FALLTHRU)
	movq	8(%rax), %rdi
	call	ASN1_STRING_type
	cmpl	$22, %eax
# SUCC: 287 [22.2%]  (CAN_FALLTHRU) 233 [77.8%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L867
# BLOCK 233 freq:1 seq:231
# PRED: 232 [77.8%]  (FALLTHRU,CAN_FALLTHRU)
	movq	8(%r14), %rdi
	call	ASN1_STRING_data
	movq	8(%r14), %rdi
	movq	%rax, %rbx
	call	ASN1_STRING_length
	testl	%eax, %eax
	movl	%eax, %r12d
# SUCC: 234 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 238 [4.5%]  (CAN_FALLTHRU)
	jle	.L484
# BLOCK 234 freq:1 seq:232
# PRED: 233 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	cltq
	cmpb	$0, -1(%rbx,%rax)
# SUCC: 235 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 238 [4.5%]  (CAN_FALLTHRU)
	jne	.L484
# BLOCK 235 freq:1 seq:233
# PRED: 234 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	-2(%rbx,%rax), %rax
# SUCC: 237 [100.0%] 
	jmp	.L485
# BLOCK 236 freq:9 seq:234
# PRED: 237 [95.5%]  (CAN_FALLTHRU)
.L868:
	subq	$1, %rax
	cmpb	$0, 1(%rax)
# SUCC: 237 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 238 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L484
# BLOCK 237 freq:10 seq:235
# PRED: 236 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 235 [100.0%] 
.L485:
	subl	$1, %r12d
# SUCC: 236 [95.5%]  (CAN_FALLTHRU) 238 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L868
# BLOCK 238 freq:1 seq:236
# PRED: 237 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 234 [4.5%]  (CAN_FALLTHRU) 233 [4.5%]  (CAN_FALLTHRU) 236 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
.L484:
	movq	%rbx, %rdi
	call	strlen
	cmpl	%eax, %r12d
# SUCC: 239 [62.2%]  (FALLTHRU,CAN_FALLTHRU) 240 [37.8%]  (CAN_FALLTHRU)
	je	.L483
# BLOCK 239 freq:1 seq:237
# PRED: 238 [62.2%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC161, %edi
	xorl	%eax, %eax
	movl	$.LC98, %ebx
# SUCC: 240 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogError
# BLOCK 240 freq:1 seq:238
# PRED: 239 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 238 [37.8%]  (CAN_FALLTHRU) 287 [100.0%] 
.L483:
	movl	UFDBglobalDebug(%rip), %eax
	cmpl	$1, %eax
# SUCC: 241 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 251 [47.5%]  (CAN_FALLTHRU)
	jle	.L486
# BLOCK 241 freq:1 seq:239
# PRED: 240 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	ufdbCategoryName
	movl	108(%rsp), %ecx
	movq	%rbx, %r8
	movq	88(%rsp), %rsi
	movq	%rax, %r9
	movq	%r13, %rdx
	movl	$.LC162, %edi
	xorl	%eax, %eax
# SUCC: 242 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 242 freq:1 seq:240
# PRED: 241 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 251 [71.0%]  (CAN_FALLTHRU) 252 [100.0%] 
.L487:
	testl	%r15d, %r15d
# SUCC: 243 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 248 [29.0%]  (CAN_FALLTHRU)
	je	.L488
# BLOCK 243 freq:1 seq:241
# PRED: 242 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalYoutubeEdufilter(%rip), %r8d
	testl	%r8d, %r8d
# SUCC: 245 [50.0%]  (CAN_FALLTHRU) 244 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L489
# BLOCK 244 seq:242
# PRED: 243 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalAllowGoogleHTTPSusingIP(%rip)
# SUCC: 245 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 248 [50.0%]  (CAN_FALLTHRU)
	je	.L488
# BLOCK 245 freq:1 seq:243
# PRED: 244 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 243 [50.0%]  (CAN_FALLTHRU)
.L489:
	leaq	560(%rsp), %rdi
	movl	$.LC164, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 246 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 248 [15.0%]  (CAN_FALLTHRU)
	je	.L488
# BLOCK 246 freq:1 seq:244
# PRED: 245 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC165, %esi
	movq	%rbx, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 247 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 248 [15.0%]  (CAN_FALLTHRU)
	je	.L488
# BLOCK 247 seq:245
# PRED: 246 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC165, %esi
	movq	%rax, %rdi
	call	strcmp
	testl	%eax, %eax
# SUCC: 283 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 248 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L869
# BLOCK 248 freq:1 seq:246
# PRED: 244 [50.0%]  (CAN_FALLTHRU) 242 [29.0%]  (CAN_FALLTHRU) 247 [97.0%]  (FALLTHRU,CAN_FALLTHRU) 246 [15.0%]  (CAN_FALLTHRU) 245 [15.0%]  (CAN_FALLTHRU)
.L488:
	movq	%r13, %rsi
	movq	%rbx, %rdi
	call	matchHostname
	testl	%eax, %eax
# SUCC: 249 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 253 [50.0%]  (CAN_FALLTHRU)
	je	.L621
# BLOCK 249 freq:1 seq:247
# PRED: 248 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %edi
	testl	%edi, %edi
# SUCC: 254 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT) 250 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L491
# BLOCK 250 freq:1 seq:248
# PRED: 249 [97.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	movl	$1, %r12d
	call	ufdbStrdup
	movl	$1, 40(%rsp)
	movq	%rax, 56(%rsp)
# SUCC: 230 [100.0%] 
	jmp	.L481
# BLOCK 251 freq:1 seq:249
# PRED: 240 [47.5%]  (CAN_FALLTHRU)
.L486:
	testl	%eax, %eax
# SUCC: 252 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 242 [71.0%]  (CAN_FALLTHRU)
	je	.L487
# BLOCK 252 seq:250
# PRED: 251 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %ecx
	movq	%rbx, %r8
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	movl	$.LC163, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 242 [100.0%] 
	jmp	.L487
# BLOCK 253 freq:1 seq:251
# PRED: 248 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L621:
	movl	$1, %r12d
# SUCC: 230 [100.0%] 
	jmp	.L481
# BLOCK 254 seq:252
# PRED: 249 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L491:
	movq	32(%rsp), %r14
# SUCC: 255 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$1, %r12d
# BLOCK 255 seq:253
# PRED: 347 [100.0%]  346 [50.0%]  (CAN_FALLTHRU) 343 [50.0%]  (CAN_FALLTHRU) 348 [71.0%]  (CAN_FALLTHRU) 254 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 349 [100.0%] 
.L494:
	movq	72(%rsp), %rdi
	movl	$GENERAL_NAME_free, %esi
	call	sk_pop_free
	movq	56(%rsp), %rdi
	testq	%rdi, %rdi
# SUCC: 256 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 257 [30.2%]  (CAN_FALLTHRU)
	je	.L497
# BLOCK 256 seq:254
# PRED: 255 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
# SUCC: 257 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbFree
# BLOCK 257 seq:255
# PRED: 255 [30.2%]  (CAN_FALLTHRU) 256 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L497:
	testl	%r12d, %r12d
# SUCC: 258 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 271 [50.0%]  (CAN_FALLTHRU)
	jne	.L498
# BLOCK 258 seq:256
# PRED: 257 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 227 [30.2%]  (CAN_FALLTHRU) 286 [100.0%] 
.L479:
	cmpl	$0, UFDBglobalDebug(%rip)
	movq	$.LC16, 304(%rsp)
# SUCC: 259 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 260 [71.0%]  (CAN_FALLTHRU)
	je	.L499
# BLOCK 259 seq:257
# PRED: 258 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %ecx
	movq	%r13, %rdx
	movl	$.LC169, %edi
	movq	88(%rsp), %rsi
	xorl	%eax, %eax
# SUCC: 260 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 260 seq:258
# PRED: 258 [71.0%]  (CAN_FALLTHRU) 259 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L499:
	movq	%r14, %rdi
	orl	$-1, %ebx
	call	X509_get_subject_name
	testq	%rax, %rax
	movq	%rax, %r12
# SUCC: 263 [85.0%]  (CAN_FALLTHRU) 261 [15.0%]  (FALLTHRU)
	jne	.L735
# BLOCK 261 seq:259
# PRED: 260 [15.0%]  (FALLTHRU)
# SUCC: 266 [100.0%] 
	jmp	.L501
# BLOCK 262 freq:2 seq:260
# PRED: 263 [91.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L870:
# SUCC: 263 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU)
	movl	%eax, %ebx
# BLOCK 263 freq:2 seq:261
# PRED: 260 [85.0%]  (CAN_FALLTHRU) 262 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU)
.L735:
	movl	%ebx, %edx
	movl	$13, %esi
	movq	%r12, %rdi
	call	X509_NAME_get_index_by_NID
	testl	%eax, %eax
# SUCC: 262 [91.0%]  (CAN_FALLTHRU) 264 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jns	.L870
# BLOCK 264 seq:262
# PRED: 263 [9.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	testl	%ebx, %ebx
# SUCC: 265 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 266 [47.5%]  (CAN_FALLTHRU)
	js	.L501
# BLOCK 265 seq:263
# PRED: 264 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%ebx, %esi
	movq	%r12, %rdi
	call	X509_NAME_get_entry
	movq	%rax, %rdi
	call	X509_NAME_ENTRY_get_data
	leaq	304(%rsp), %rdi
	movq	%rax, %rsi
# SUCC: 266 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ASN1_STRING_to_UTF8
# BLOCK 266 seq:264
# PRED: 264 [47.5%]  (CAN_FALLTHRU) 265 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 261 [100.0%] 
.L501:
	movq	304(%rsp), %rbx
	cmpq	$.LC16, %rbx
# SUCC: 281 [15.0%]  (CAN_FALLTHRU) 267 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L871
# BLOCK 267 seq:265
# PRED: 266 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	testq	%rbx, %rbx
# SUCC: 282 [15.0%]  (CAN_FALLTHRU) 268 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L506
# BLOCK 268 seq:266
# PRED: 267 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r13, %rsi
	movq	%rbx, %rdi
	call	matchHostname
	testl	%eax, %eax
# SUCC: 269 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 270 [71.0%]  (CAN_FALLTHRU)
	jne	.L509
# BLOCK 269 seq:267
# PRED: 268 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	88(%rsp), %rsi
	movq	%rbx, %rdx
	movq	%r13, %rcx
	movl	$.LC171, %edi
	call	ufdbLogError
	movq	304(%rsp), %rbx
# SUCC: 270 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$0, 44(%rsp)
# BLOCK 270 seq:268
# PRED: 268 [71.0%]  (CAN_FALLTHRU) 269 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L509:
	movq	%rbx, %rdi
# SUCC: 271 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	CRYPTO_free
# BLOCK 271 seq:269
# PRED: 257 [50.0%]  (CAN_FALLTHRU) 270 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 282 [100.0%] 
.L498:
	xorl	%ebx, %ebx
# SUCC: 272 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
# BLOCK 272 freq:5 seq:270
# PRED: 271 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 274 [100.0%] 
.L510:
	leaq	176(%rsp), %rcx
	leaq	272(%rsp), %rdx
	leaq	144(%rsp), %rsi
	leaq	240(%rsp), %rdi
	call	ERR_get_error_line_data
	testq	%rax, %rax
	movq	%rax, %r12
# SUCC: 273 [91.0%]  (FALLTHRU,CAN_FALLTHRU) 276 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L872
# BLOCK 273 freq:4 seq:271
# PRED: 272 [91.0%]  (FALLTHRU,CAN_FALLTHRU)
	testl	%ebx, %ebx
# SUCC: 275 [29.0%]  (CAN_FALLTHRU) 274 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L873
# BLOCK 274 freq:4 seq:272
# PRED: 273 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 275 [100.0%] 
.L511:
	movl	$256, %edx
	movq	%r12, %rdi
	movl	$1, %ebx
	leaq	304(%rsp), %rsi
	call	ERR_error_string_n
	leaq	304(%rsp), %rsi
	xorl	%eax, %eax
	movl	$.LC60, %edi
	call	ufdbLogMessage
# SUCC: 272 [100.0%] 
	jmp	.L510
# BLOCK 275 freq:1 seq:273
# PRED: 273 [29.0%]  (CAN_FALLTHRU)
.L873:
	movl	$.LC59, %edi
	xorl	%eax, %eax
	call	ufdbLogError
# SUCC: 274 [100.0%] 
	jmp	.L511
# BLOCK 276 seq:274
# PRED: 272 [9.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L872:
	cmpl	$0, 44(%rsp)
# SUCC: 277 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 280 [50.0%]  (CAN_FALLTHRU)
	je	.L513
# BLOCK 277 seq:275
# PRED: 276 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebug(%rip)
# SUCC: 278 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 279 [71.0%]  (CAN_FALLTHRU)
	je	.L514
# BLOCK 278 seq:276
# PRED: 277 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	88(%rsp), %rsi
	movq	%r13, %rdx
	movl	$.LC172, %edi
	xorl	%eax, %eax
# SUCC: 279 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 279 seq:277
# PRED: 277 [71.0%]  (CAN_FALLTHRU) 278 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L514:
	movq	%r14, %rdi
	call	X509_free
# SUCC: 64 [100.0%] 
	jmp	.L515
# BLOCK 280 seq:278
# PRED: 276 [50.0%]  (CAN_FALLTHRU)
.L513:
	movq	%r14, %rdi
	call	X509_free
	.p2align 4,,5
# SUCC: 62 [100.0%] 
	jmp	.L404
# BLOCK 281 seq:279
# PRED: 266 [15.0%]  (CAN_FALLTHRU)
.L871:
# SUCC: 282 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$0, 304(%rsp)
# BLOCK 282 seq:280
# PRED: 267 [15.0%]  (CAN_FALLTHRU) 281 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L506:
	movl	108(%rsp), %ecx
	movq	%r13, %rdx
	movl	$.LC170, %edi
	movq	88(%rsp), %rsi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$0, 44(%rsp)
# SUCC: 271 [100.0%] 
	jmp	.L498
# BLOCK 283 seq:281
# PRED: 247 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L869:
	cmpl	$0, UFDBglobalDebug(%rip)
	movq	32(%rsp), %r14
# SUCC: 284 [39.0%]  (FALLTHRU,CAN_FALLTHRU) 285 [61.0%]  (CAN_FALLTHRU)
	je	.L490
# BLOCK 284 seq:282
# PRED: 283 [39.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %ecx
	movq	%rbx, %r8
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	movl	$.LC166, %edi
# SUCC: 285 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 285 seq:283
# PRED: 283 [61.0%]  (CAN_FALLTHRU) 284 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L490:
	movq	72(%rsp), %rdi
	movl	$GENERAL_NAME_free, %esi
	call	sk_pop_free
	movq	%r14, %rdi
	call	X509_free
# SUCC: 64 [100.0%] 
	jmp	.L515
# BLOCK 286 seq:284
# PRED: 228 [3.0%]  (CAN_FALLTHRU)
.L480:
	movq	72(%rsp), %rdi
	movl	$GENERAL_NAME_free, %esi
	call	sk_pop_free
# SUCC: 258 [100.0%] 
	jmp	.L479
# BLOCK 287 seq:285
# PRED: 232 [22.2%]  (CAN_FALLTHRU)
.L867:
	movl	$.LC160, %edi
	xorl	%eax, %eax
	xorl	%ebx, %ebx
	call	ufdbLogError
# SUCC: 240 [100.0%] 
	jmp	.L483
# BLOCK 288 seq:286
# PRED: 149 [9.2%]  (CAN_FALLTHRU) 290 [9.2%]  (CAN_FALLTHRU)
.L374:
	movl	108(%rsp), %edx
	movq	%r13, %rsi
	movl	$.LC104, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$11, 104(%rsp)
# SUCC: 193 [100.0%] 
	jmp	.L585
# BLOCK 289 seq:287
# PRED: 153 [16.8%]  (CAN_FALLTHRU)
.L372:
	movl	108(%rsp), %esi
	movq	%r13, %rdi
	call	detectSkypeSN
	cmpl	$17, %eax
	movl	%eax, 104(%rsp)
# SUCC: 127 [9.2%]  (CAN_FALLTHRU) 290 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L874
# BLOCK 290 seq:288
# PRED: 289 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$11, 104(%rsp)
# SUCC: 288 [9.2%]  (CAN_FALLTHRU) 291 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L374
# BLOCK 291 seq:289
# PRED: 290 [90.8%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$8, 104(%rsp)
# SUCC: 292 [51.2%]  (FALLTHRU,CAN_FALLTHRU) 189 [48.8%]  (CAN_FALLTHRU)
	je	.L376
# BLOCK 292 freq:2 seq:290
# PRED: 291 [51.2%]  (FALLTHRU,CAN_FALLTHRU)
	movsbq	0(%r13), %r12
	testb	%r12b, %r12b
# SUCC: 185 [91.0%]  (CAN_FALLTHRU) 293 [9.0%]  (FALLTHRU)
	jne	.L365
# BLOCK 293 seq:291
# PRED: 292 [9.0%]  (FALLTHRU)
# SUCC: 189 [100.0%] 
	jmp	.L376
# BLOCK 294 seq:292
# PRED: 208 [50.0%]  (CAN_FALLTHRU)
.L454:
	leaq	-2(%rax), %rax
	cmpq	$18, %rax
# SUCC: 340 [5.5%]  (CAN_FALLTHRU) 295 [94.5%]  (FALLTHRU,CAN_FALLTHRU)
	ja	.L456
# BLOCK 295 seq:293
# PRED: 294 [94.5%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,3
# SUCC: 340 [5.9%]  339 [11.8%]  337 [11.8%]  335 [11.8%]  334 [11.8%]  333 [11.8%]  326 [11.8%]  317 [11.8%]  316 [11.8%] 
	jmp	*.L458(,%rax,8)
	.section	.rodata
	.align 8
	.align 4
.L458:
	.quad	.L457
	.quad	.L456
	.quad	.L459
	.quad	.L456
	.quad	.L456
	.quad	.L460
	.quad	.L456
	.quad	.L461
	.quad	.L462
	.quad	.L456
	.quad	.L456
	.quad	.L456
	.quad	.L456
	.quad	.L456
	.quad	.L456
	.quad	.L456
	.quad	.L463
	.quad	.L464
	.quad	.L465
	.text
# BLOCK 296 seq:294
# PRED: 207 [28.0%]  (CAN_FALLTHRU)
.L863:
	cmpl	$0, ufdbCacertsLoaded(%rip)
# SUCC: 297 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 223 [29.0%]  (CAN_FALLTHRU)
	je	.L445
# BLOCK 297 seq:295
# PRED: 296 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	40(%rsp), %eax
	orl	32(%rsp), %eax
# SUCC: 298 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 301 [50.0%]  (CAN_FALLTHRU)
	je	.L446
# BLOCK 298 seq:296
# PRED: 297 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebug(%rip)
# SUCC: 300 [50.0%]  (CAN_FALLTHRU) 299 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L447
# BLOCK 299 seq:297
# PRED: 298 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalHttpsOfficialCertificate(%rip)
	movl	$1, 44(%rsp)
# SUCC: 300 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 227 [50.0%]  (CAN_FALLTHRU)
	je	.L476
# BLOCK 300 seq:298
# PRED: 299 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 298 [50.0%]  (CAN_FALLTHRU)
.L447:
	cmpl	$0, 32(%rsp)
	movl	$.LC16, %eax
	movl	$.LC95, %edx
	movl	108(%rsp), %r9d
	movl	$.LC96, %esi
	movq	%r13, %r8
	movq	88(%rsp), %rcx
	movl	$.LC132, %edi
	cmove	%rax, %rdx
	cmpl	$0, 40(%rsp)
	cmove	%rax, %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$1, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 301 seq:299
# PRED: 297 [50.0%]  (CAN_FALLTHRU)
.L446:
	leaq	560(%rsp), %rdi
	movl	$.LC133, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 305 [15.0%]  (CAN_FALLTHRU) 302 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L875
# BLOCK 302 seq:300
# PRED: 305 [85.0%]  (CAN_FALLTHRU) 301 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 314 [17.6%]  (CAN_FALLTHRU) 313 [15.0%]  (CAN_FALLTHRU) 312 [28.0%]  (CAN_FALLTHRU) 311 [15.0%]  (CAN_FALLTHRU) 310 [15.0%]  (CAN_FALLTHRU) 309 [85.0%]  (CAN_FALLTHRU) 308 [85.0%]  (CAN_FALLTHRU) 307 [85.0%]  (CAN_FALLTHRU) 306 [85.0%]  (CAN_FALLTHRU)
.L451:
	cmpl	$0, UFDBglobalDebug(%rip)
# SUCC: 304 [50.0%]  (CAN_FALLTHRU) 303 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L452
# BLOCK 303 seq:301
# PRED: 302 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalHttpsOfficialCertificate(%rip)
	movl	$0, 44(%rsp)
# SUCC: 304 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 227 [50.0%]  (CAN_FALLTHRU)
	je	.L476
# BLOCK 304 seq:302
# PRED: 303 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [50.0%]  (CAN_FALLTHRU)
.L452:
	movl	108(%rsp), %r9d
	leaq	1584(%rsp), %rax
	movq	%r13, %r8
	movq	88(%rsp), %rcx
	movq	%rax, 8(%rsp)
	movl	$.LC16, %edx
	leaq	560(%rsp), %rax
	movq	%rdx, %rsi
	movl	$.LC138, %edi
	movq	%rax, (%rsp)
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 305 seq:303
# PRED: 301 [15.0%]  (CAN_FALLTHRU)
.L875:
	leaq	560(%rsp), %rdi
	movl	$.LC134, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 306 [15.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [85.0%]  (CAN_FALLTHRU)
	jne	.L451
# BLOCK 306 seq:304
# PRED: 305 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	560(%rsp), %rdi
	movl	$.LC135, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 307 [15.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [85.0%]  (CAN_FALLTHRU)
	jne	.L451
# BLOCK 307 seq:305
# PRED: 306 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	1584(%rsp), %rdi
	movl	$.LC133, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 308 [15.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [85.0%]  (CAN_FALLTHRU)
	jne	.L451
# BLOCK 308 seq:306
# PRED: 307 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	1584(%rsp), %rdi
	movl	$.LC136, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 309 [15.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [85.0%]  (CAN_FALLTHRU)
	jne	.L451
# BLOCK 309 seq:307
# PRED: 308 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	1584(%rsp), %rdi
	movl	$.LC134, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 310 [15.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [85.0%]  (CAN_FALLTHRU)
	jne	.L451
# BLOCK 310 seq:308
# PRED: 309 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	560(%rsp), %rdi
	movl	$.LC120, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 311 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [15.0%]  (CAN_FALLTHRU)
	je	.L451
# BLOCK 311 seq:309
# PRED: 310 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	1584(%rsp), %rdi
	movl	$.LC120, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 312 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [15.0%]  (CAN_FALLTHRU)
	je	.L451
# BLOCK 312 seq:310
# PRED: 311 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpb	$45, 560(%rsp)
# SUCC: 313 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [28.0%]  (CAN_FALLTHRU)
	je	.L451
# BLOCK 313 seq:311
# PRED: 312 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	1584(%rsp), %rdi
	movl	$46, %esi
	call	strchr
	testq	%rax, %rax
# SUCC: 314 [85.0%]  (FALLTHRU,CAN_FALLTHRU) 302 [15.0%]  (CAN_FALLTHRU)
	je	.L451
# BLOCK 314 seq:312
# PRED: 313 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	560(%rsp), %rdi
	movl	$46, %esi
	call	strchr
	testq	%rax, %rax
# SUCC: 315 [82.5%]  (FALLTHRU,CAN_FALLTHRU) 302 [17.6%]  (CAN_FALLTHRU)
	je	.L451
# BLOCK 315 seq:313
# PRED: 314 [82.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %ecx
	leaq	1584(%rsp), %r9
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	leaq	560(%rsp), %r8
	xorl	%eax, %eax
	movl	$.LC137, %edi
	call	ufdbLogMessage
	movq	%r14, %rdi
	call	X509_free
# SUCC: 134 [100.0%] 
	jmp	.L811
# BLOCK 316 seq:314
# PRED: 295 [11.8%] 
.L465:
	movl	108(%rsp), %ecx
	leaq	560(%rsp), %r8
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	movl	$.LC153, %edi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 317 seq:315
# PRED: 295 [11.8%] 
.L464:
	leaq	1584(%rsp), %rdi
	movl	$.LC145, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 318 [82.5%]  (FALLTHRU,CAN_FALLTHRU) 323 [17.6%]  (CAN_FALLTHRU)
	je	.L466
# BLOCK 318 seq:316
# PRED: 317 [82.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %ebx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	leaq	1584(%rsp), %r9
	movl	$.LC146, %edi
	leaq	560(%rsp), %r8
	movl	%ebx, %ecx
	call	ufdbLogMessage
	movq	%r14, %rdi
	call	X509_free
	movq	96(%rsp), %rsi
	movl	%ebx, %ecx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movl	$.LC180, %edi
	call	ufdbLogMessage
	movq	208(%rsp), %rbx
	testq	%rbx, %rbx
# SUCC: 321 [21.6%]  (CAN_FALLTHRU) 319 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L534
# BLOCK 319 freq:13 seq:317
# PRED: 318 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 322 [13.1%]  (CAN_FALLTHRU) 320 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L876
# BLOCK 320 freq:13 seq:318
# PRED: 319 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 322 [100.0%] 
.L535:
	movq	%rbx, %rdi
# SUCC: 321 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 321 freq:17 seq:319
# PRED: 320 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 318 [21.6%]  (CAN_FALLTHRU)
.L534:
	movq	80(%rsp), %rax
	movl	$25, (%rax)
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 322 freq:2 seq:320
# PRED: 319 [13.1%]  (CAN_FALLTHRU)
.L876:
	call	ERR_clear_error
# SUCC: 320 [100.0%] 
	jmp	.L535
# BLOCK 323 seq:321
# PRED: 317 [17.6%]  (CAN_FALLTHRU)
.L466:
	cmpl	$0, 40(%rsp)
	.p2align 4,,3
# SUCC: 324 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 325 [29.0%]  (CAN_FALLTHRU)
	je	.L467
# BLOCK 324 seq:322
# PRED: 323 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	1584(%rsp), %rdi
	movl	$.LC143, %esi
	call	strstr
	testq	%rax, %rax
# SUCC: 218 [82.5%]  (CAN_FALLTHRU) 325 [17.6%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L824
# BLOCK 325 seq:323
# PRED: 324 [17.6%]  (FALLTHRU,CAN_FALLTHRU) 323 [29.0%]  (CAN_FALLTHRU)
.L467:
	movl	108(%rsp), %ecx
	leaq	560(%rsp), %r8
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	movl	$.LC147, %edi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 326 seq:324
# PRED: 295 [11.8%] 
.L463:
	leaq	560(%rsp), %rdi
	movl	$.LC148, %esi
	call	strstr
	leaq	560(%rsp), %r8
	testq	%rax, %rax
# SUCC: 327 [92.0%]  (FALLTHRU,CAN_FALLTHRU) 332 [8.0%]  (CAN_FALLTHRU)
	je	.L468
# BLOCK 327 seq:325
# PRED: 326 [92.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	108(%rsp), %ebx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movq	88(%rsp), %rsi
	movl	$.LC149, %edi
	movl	%ebx, %ecx
	call	ufdbLogMessage
	movq	%r14, %rdi
	call	X509_free
	movq	96(%rsp), %rsi
	movl	%ebx, %ecx
	xorl	%eax, %eax
	movq	%r13, %rdx
	movl	$.LC175, %edi
	call	ufdbLogMessage
	movq	208(%rsp), %rbx
	testq	%rbx, %rbx
# SUCC: 330 [21.6%]  (CAN_FALLTHRU) 328 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L519
# BLOCK 328 freq:14 seq:326
# PRED: 327 [78.4%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %rdi
	call	SSL_shutdown
	testl	%eax, %eax
# SUCC: 331 [13.1%]  (CAN_FALLTHRU) 329 [86.9%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L877
# BLOCK 329 freq:14 seq:327
# PRED: 328 [86.9%]  (FALLTHRU,CAN_FALLTHRU) 331 [100.0%] 
.L520:
	movq	%rbx, %rdi
# SUCC: 330 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	SSL_free
# BLOCK 330 freq:18 seq:328
# PRED: 329 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 327 [21.6%]  (CAN_FALLTHRU)
.L519:
	movq	80(%rsp), %rax
	movl	$21, (%rax)
	xorl	%eax, %eax
# SUCC: 84 [100.0%] 
	jmp	.L822
# BLOCK 331 freq:2 seq:329
# PRED: 328 [13.1%]  (CAN_FALLTHRU)
.L877:
	call	ERR_clear_error
# SUCC: 329 [100.0%] 
	jmp	.L520
# BLOCK 332 seq:330
# PRED: 326 [8.0%]  (CAN_FALLTHRU)
.L468:
	movl	108(%rsp), %ecx
	movq	%r13, %rdx
	movl	$.LC150, %edi
	movq	88(%rsp), %rsi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 333 seq:331
# PRED: 295 [11.8%] 
.L462:
	movl	108(%rsp), %ecx
	movq	%r13, %rdx
	movl	$.LC152, %edi
	movq	88(%rsp), %rsi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 334 seq:332
# PRED: 295 [11.8%] 
.L461:
	movl	108(%rsp), %ecx
	movq	%r13, %rdx
	movl	$.LC151, %edi
	movq	88(%rsp), %rsi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 335 seq:333
# PRED: 295 [11.8%] 
.L460:
	movl	40(%rsp), %eax
	orl	32(%rsp), %eax
# SUCC: 336 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 338 [50.0%]  (CAN_FALLTHRU)
	je	.L473
# BLOCK 336 seq:334
# PRED: 335 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, 32(%rsp)
	movl	$.LC16, %eax
	movl	$.LC95, %edx
	movl	108(%rsp), %r9d
	movl	$.LC96, %esi
	movq	%r13, %r8
	movq	88(%rsp), %rcx
	movl	$.LC156, %edi
	cmove	%rax, %rdx
	cmpl	$0, 40(%rsp)
	cmove	%rax, %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$1, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 337 seq:335
# PRED: 295 [11.8%] 
.L459:
	cmpl	$0, 32(%rsp)
	movl	$.LC16, %eax
	movl	$.LC95, %edx
	movl	108(%rsp), %r9d
	movl	$.LC96, %esi
	movq	%r13, %r8
	movq	88(%rsp), %rcx
	movl	$.LC155, %edi
	cmove	%rax, %rdx
	cmpl	$0, 40(%rsp)
	cmove	%rax, %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 338 seq:336
# PRED: 335 [50.0%]  (CAN_FALLTHRU)
.L473:
	movl	108(%rsp), %r9d
	movl	$.LC16, %edx
	movq	%r13, %r8
	movq	88(%rsp), %rcx
	movq	%rdx, %rsi
	movl	$.LC157, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 339 seq:337
# PRED: 295 [11.8%] 
.L457:
	cmpl	$0, 32(%rsp)
	movl	$.LC16, %eax
	movl	$.LC95, %edx
	movl	108(%rsp), %r9d
	movl	$.LC96, %esi
	movq	%r13, %r8
	movq	88(%rsp), %rcx
	movl	$.LC154, %edi
	cmove	%rax, %rdx
	cmpl	$0, 40(%rsp)
	cmove	%rax, %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 340 seq:338
# PRED: 295 [5.9%]  294 [5.5%]  (CAN_FALLTHRU)
.L456:
	movq	%rbx, %rdi
	call	X509_verify_cert_error_string
	movl	108(%rsp), %ecx
	movq	%rbx, %r8
	movq	88(%rsp), %rsi
	movq	%rax, %r9
	movq	%r13, %rdx
	movl	$.LC158, %edi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$0, 44(%rsp)
# SUCC: 227 [100.0%] 
	jmp	.L476
# BLOCK 341 seq:339
# PRED: 76 (CAN_FALLTHRU)
.L558:
# SUCC: 342 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 80 (CAN_FALLTHRU)
	jne	.L551
# BLOCK 342 freq:57 seq:340
# PRED: 341 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	80(%rsp), %rax
	movl	$8, (%rax)
# SUCC: 80 [100.0%] 
	jmp	.L551
# BLOCK 343 seq:341
# PRED: 230 [3.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L866:
	testl	%r12d, %r12d
	movq	32(%rsp), %r14
# SUCC: 344 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 255 [50.0%]  (CAN_FALLTHRU)
	je	.L494
# BLOCK 344 seq:342
# PRED: 343 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, 40(%rsp)
# SUCC: 345 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 348 [50.0%]  (CAN_FALLTHRU)
	jne	.L495
# BLOCK 345 seq:343
# PRED: 344 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebug(%rip)
# SUCC: 347 [29.0%]  (CAN_FALLTHRU) 346 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L496
# BLOCK 346 seq:344
# PRED: 345 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalHttpsOfficialCertificate(%rip)
	movl	$0, 44(%rsp)
# SUCC: 347 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 255 [50.0%]  (CAN_FALLTHRU)
	je	.L494
# BLOCK 347 seq:345
# PRED: 346 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 345 [29.0%]  (CAN_FALLTHRU)
.L496:
	movq	88(%rsp), %rsi
	movq	%r13, %rcx
	movq	%rbx, %rdx
	movl	$.LC167, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$0, 44(%rsp)
# SUCC: 255 [100.0%] 
	jmp	.L494
# BLOCK 348 seq:346
# PRED: 344 [50.0%]  (CAN_FALLTHRU)
.L495:
	cmpl	$0, UFDBglobalDebug(%rip)
# SUCC: 349 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 255 [71.0%]  (CAN_FALLTHRU)
	je	.L494
# BLOCK 349 seq:347
# PRED: 348 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	56(%rsp), %rdx
	movq	%r13, %rcx
	movl	$.LC168, %edi
	movq	88(%rsp), %rsi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 255 [100.0%] 
	jmp	.L494
	.cfi_endproc
.LFE92:
	.size	httpsGETroot.constprop.7, .-httpsGETroot.constprop.7
	.section	.rodata.str1.8
	.align 8
.LC189:
	.string	"and an unknown protocol is allowed"
	.align 8
.LC190:
	.string	"and is considered a PROXY TUNNEL because allow-unknown-protocol-over-https is OFF"
	.align 8
.LC191:
	.string	"UFDBverifyPortHasHTTPS( %s, %d )"
	.align 8
.LC192:
	.string	"   UFDBverifyPortHasHTTPS: waiting for other thread on status for %s:%d"
	.align 8
.LC193:
	.string	"   UFDBverifyPortHasHTTPS: finished waiting %d intervals for other thread on %s:%d"
	.align 8
.LC194:
	.string	"   UFDBverifyPortHasHTTPS: waited too long for other thread on status for %s:%d"
	.align 8
.LC195:
	.string	"HTTPS protocol verification for %s:%d FAILED: cannot open communication socket"
	.align 8
.LC196:
	.string	"UFDBverifyPortHasHTTPS: socket to %s:%d is opened successfully. fd=%d"
	.align 8
.LC197:
	.string	"   httpsGETroot for %s:%d returned status %s, content is %sNULL"
	.align 8
.LC198:
	.string	"HTTPS server %s:%d has an empty or no response."
	.align 8
.LC199:
	.string	"   HTTPS protocol reply for %s:%d:\n   %s"
	.section	.rodata.str1.1
.LC200:
	.string	"logmein.com/"
.LC201:
	.string	"hamachi.cc/"
	.section	.rodata.str1.8
	.align 8
.LC202:
	.string	"HTTPS protocol on %s:%d uses a hamachi/logmein TUNNEL"
	.section	.rodata.str1.1
.LC203:
	.string	"Set-Cookie: SSLX_SSESHID="
	.section	.rodata.str1.8
	.align 8
.LC204:
	.string	"HTTPS protocol on %s:%d uses an SSL-Explorer TUNNEL"
	.section	.rodata.str1.1
.LC205:
	.string	"BarracudaServer.com"
.LC206:
	.string	"barracudaserver.com"
.LC207:
	.string	"BarracudaDrive"
	.section	.rodata.str1.8
	.align 8
.LC208:
	.string	"HTTPS protocol on %s:%d uses a BARRACUDA proxy TUNNEL"
	.section	.rodata.str1.1
.LC209:
	.string	"  index.vnc -"
.LC210:
	.string	"  used with Xvnc"
.LC211:
	.string	"TightVNC Java viewer applet"
	.section	.rodata.str1.8
	.align 8
.LC212:
	.string	"HTTPS protocol on %s:%d uses a VNC proxy TUNNEL"
	.section	.rodata.str1.1
.LC213:
	.string	"X-Webtunnel-Status"
.LC214:
	.string	"X-webtunnel-status"
.LC215:
	.string	"X-webtunnel-version"
.LC216:
	.string	"X-Webtunnel-Version"
	.section	.rodata.str1.8
	.align 8
.LC217:
	.string	"HTTPS protocol on %s:%d uses Webtunnel TUNNEL"
	.align 8
.LC218:
	.string	"HTTPS protocol on %s:%d encapsulates a non-HTTP protocol %s"
	.align 8
.LC219:
	.string	"HTTPS protocol on %s:%d has been verified and status is %s"
	.align 8
.LC220:
	.string	"IP address in URL is not allowed for the HTTPS protocol. IP=%s"
	.text
	.p2align 4,,15
	.type	UFDBverifyPortHasHTTPS, @function
UFDBverifyPortHasHTTPS:
.LFB75:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movl	%edx, %r13d
	pushq	%r12
	.cfi_offset 12, -48
	movl	%esi, %r12d
	pushq	%rbx
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$3760, %rsp
	movl	UFDBglobalDebug(%rip), %r10d
	testl	%r10d, %r10d
# SUCC: 9 [29.0%]  (CAN_FALLTHRU) 3 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L879
# BLOCK 3 freq:7100 seq:1
# PRED: 2 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	testb	$2, %dl
# SUCC: 9 [50.0%]  (CAN_FALLTHRU) 4 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L879
# BLOCK 4 freq:10000 seq:2
# PRED: 3 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 9 [100.0%] 
.L880:
	movl	UFDBglobalHttpsWithHostname(%rip), %r9d
	testl	%r9d, %r9d
# SUCC: 5 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [50.0%]  (CAN_FALLTHRU)
	je	.L881
# BLOCK 5 freq:5000 seq:3
# PRED: 4 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalSquidUsesSSLbump(%rip), %r8d
	testl	%r8d, %r8d
# SUCC: 10 [50.0%]  (CAN_FALLTHRU) 6 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1056
# BLOCK 6 freq:9998 seq:4
# PRED: 5 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 4 [50.0%]  (CAN_FALLTHRU) 10 [50.0%]  (CAN_FALLTHRU) 17 [10.1%]  (CAN_FALLTHRU) 16 [71.0%]  (CAN_FALLTHRU) 15 [50.0%]  (CAN_FALLTHRU) 14 [50.0%]  (CAN_FALLTHRU) 13 [50.0%]  (CAN_FALLTHRU) 12 [50.0%]  (CAN_FALLTHRU) 11 [50.0%]  (CAN_FALLTHRU) 157 [100.0%] 
.L881:
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 7 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 26 [33.0%]  (CAN_FALLTHRU)
	je	.L889
# BLOCK 7 freq:9527 seq:5
# PRED: 128 [100.0%]  6 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 121 [100.0%]  26 [83.2%]  (CAN_FALLTHRU) 97 [2.2%]  (CAN_FALLTHRU,LOOP_EXIT) 167 [100.0%] 
.L1050:
# SUCC: 8 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$8, %eax
# BLOCK 8 freq:20000 seq:6
# PRED: 7 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 25 [100.0%]  112 [100.0%]  72 [62.2%]  (CAN_FALLTHRU) 118 [30.2%]  (CAN_FALLTHRU) 116 [100.0%]  76 [100.0%]  119 [100.0%] 
.L1048:
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 9 freq:6450 seq:7
# PRED: 3 [50.0%]  (CAN_FALLTHRU) 2 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L879:
	.cfi_restore_state
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC191, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 4 [100.0%] 
	jmp	.L880
# BLOCK 10 freq:2500 seq:8
# PRED: 5 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1056:
	movl	UFDBglobalUnknownProtocolOverHttps(%rip), %edi
	testl	%edi, %edi
# SUCC: 11 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [50.0%]  (CAN_FALLTHRU)
	jne	.L881
# BLOCK 11 freq:1250 seq:9
# PRED: 10 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalSkypeOverHttps(%rip), %esi
	testl	%esi, %esi
# SUCC: 12 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [50.0%]  (CAN_FALLTHRU)
	jne	.L881
# BLOCK 12 freq:625 seq:10
# PRED: 11 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalGtalkOverHttps(%rip), %ecx
	testl	%ecx, %ecx
# SUCC: 13 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [50.0%]  (CAN_FALLTHRU)
	jne	.L881
# BLOCK 13 freq:312 seq:11
# PRED: 12 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalFBchatOverHttps(%rip), %edx
	testl	%edx, %edx
# SUCC: 14 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [50.0%]  (CAN_FALLTHRU)
	jne	.L881
# BLOCK 14 freq:156 seq:12
# PRED: 13 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalYahooMsgOverHttps(%rip), %eax
	testl	%eax, %eax
# SUCC: 15 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [50.0%]  (CAN_FALLTHRU)
	jne	.L881
# BLOCK 15 freq:78 seq:13
# PRED: 14 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalAimOverHttps(%rip), %eax
	testl	%eax, %eax
# SUCC: 16 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [50.0%]  (CAN_FALLTHRU)
	jne	.L881
# BLOCK 16 freq:39 seq:14
# PRED: 15 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalCitrixOnlineOverHttps(%rip), %eax
	testl	%eax, %eax
# SUCC: 17 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 6 [71.0%]  (CAN_FALLTHRU)
	jne	.L881
# BLOCK 17 freq:11 seq:15
# PRED: 16 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	testq	%rbx, %rbx
# SUCC: 6 [10.1%]  (CAN_FALLTHRU) 18 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L881
# BLOCK 18 freq:10 seq:16
# PRED: 17 [89.9%]  (FALLTHRU,CAN_FALLTHRU)
	movzbl	(%rbx), %edx
	testb	%dl, %dl
# SUCC: 19 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 23 [4.5%]  (CAN_FALLTHRU)
	je	.L882
# BLOCK 19 freq:10 seq:17
# PRED: 18 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, %r15
# SUCC: 20 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movsbq	%dl, %r14
# BLOCK 20 freq:128 seq:18
# PRED: 22 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 19 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L886:
	cmpb	$46, %r14b
# SUCC: 21 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 22 [28.0%]  (CAN_FALLTHRU)
	je	.L883
# BLOCK 21 freq:92 seq:19
# PRED: 20 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%edx, 12(%rsp)
	call	__ctype_b_loc
	movl	12(%rsp), %edx
	movq	(%rax), %rax
	testb	$8, 1(%rax,%r14,2)
# SUCC: 153 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 22 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1057
# BLOCK 22 freq:124 seq:20
# PRED: 21 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 20 [28.0%]  (CAN_FALLTHRU)
.L883:
	addq	$1, %r15
	movsbq	(%r15), %r14
	testb	%r14b, %r14b
# SUCC: 20 [95.5%]  (DFS_BACK,CAN_FALLTHRU) 23 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L886
# BLOCK 23 freq:8 seq:21
# PRED: 154 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 18 [4.5%]  (CAN_FALLTHRU) 22 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
.L882:
	xorl	%eax, %eax
	movq	%rbx, %rsi
	movl	$.LC220, %edi
	call	ufdbLogError
	movl	%r12d, %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	xorl	%eax, %eax
	movl	$13, 16(%rsp)
	leaq	1152(%rsp), %rdi
	call	sprintf
	xorl	%edi, %edi
	call	time
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	1152(%rsp), %rsi
	movq	%rax, %r12
	call	UFDBsearchHashtable
	testq	%rax, %rax
# SUCC: 170 [4.4%]  (CAN_FALLTHRU) 24 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1058
# BLOCK 24 freq:2 seq:22
# PRED: 23 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$13, (%rax)
# SUCC: 25 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r12, 8(%rax)
# BLOCK 25 freq:101 seq:23
# PRED: 104 [39.0%]  (CAN_FALLTHRU) 105 [100.0%]  94 [100.0%]  24 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 102 [39.0%]  (CAN_FALLTHRU) 170 [100.0%] 
.L1052:
	movl	16(%rsp), %eax
# SUCC: 8 [100.0%] 
	jmp	.L1048
# BLOCK 26 freq:3299 seq:24
# PRED: 6 [33.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L889:
	movl	UFDBglobalTerminating(%rip), %eax
	testl	%eax, %eax
# SUCC: 7 [83.2%]  (CAN_FALLTHRU) 27 [16.8%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1050
# BLOCK 27 freq:553 seq:25
# PRED: 26 [16.8%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r12d, %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	leaq	1152(%rsp), %rdi
	call	sprintf
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	1152(%rsp), %rsi
	call	UFDBsearchHashtable
	testq	%rax, %rax
	movq	%rax, %r14
# SUCC: 66 [46.7%]  (CAN_FALLTHRU) 28 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L891
# BLOCK 28 freq:294 seq:26
# PRED: 27 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%edi, %edi
	call	time
	movq	8(%r14), %rdx
	movq	%rax, %r15
	movl	(%r14), %eax
	movq	%r14, 80(%rsp)
	cmpq	UFDBglobalDatabaseLoadTime(%rip), %rdx
# SUCC: 29 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 68 [33.0%]  (CAN_FALLTHRU)
	jge	.L1059
# BLOCK 29 freq:216 seq:27
# PRED: 28 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 126 [67.0%]  (CAN_FALLTHRU) 165 [83.9%]  (CAN_FALLTHRU)
.L892:
# SUCC: 30 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$28, 16(%rsp)
# BLOCK 30 freq:132 seq:28
# PRED: 71 [13.7%]  (CAN_FALLTHRU) 69 (CAN_FALLTHRU) 29 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1055:
	movl	%r12d, %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	leaq	1152(%rsp), %rdi
	xorl	%eax, %eax
	call	sprintf
	xorl	%edi, %edi
	call	time
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	1152(%rsp), %rsi
	movq	%rax, %r15
	call	UFDBsearchHashtable
	testq	%rax, %rax
	movq	%rax, %r14
# SUCC: 152 [4.4%]  (CAN_FALLTHRU) 31 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1060
# BLOCK 31 freq:126 seq:29
# PRED: 30 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$31, (%rax)
# SUCC: 32 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r15, 8(%rax)
# BLOCK 32 freq:132 seq:30
# PRED: 31 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 152 [100.0%] 
.L902:
# SUCC: 33 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r14, 80(%rsp)
# BLOCK 33 freq:220 seq:31
# PRED: 96 [2.2%]  (CAN_FALLTHRU,LOOP_EXIT) 32 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 67 [100.0%] 
.L898:
	movl	%r12d, %esi
	movq	%rbx, %rdi
	call	UFDBopenSocket
	testl	%eax, %eax
	movl	%eax, %r15d
# SUCC: 120 [23.5%]  (CAN_FALLTHRU) 34 [76.5%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L1061
# BLOCK 34 freq:169 seq:32
# PRED: 33 [76.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 116 [45.3%]  (CAN_FALLTHRU) 35 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1062
# BLOCK 35 freq:92 seq:33
# PRED: 34 [54.7%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %eax
	testl	%eax, %eax
# SUCC: 43 [29.0%]  (CAN_FALLTHRU) 36 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L925
# BLOCK 36 freq:65 seq:34
# PRED: 35 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	testb	$2, %r13b
# SUCC: 43 [50.0%]  (CAN_FALLTHRU) 37 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L925
# BLOCK 37 freq:33 seq:35
# PRED: 36 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugAim(%rip), %eax
	testl	%eax, %eax
# SUCC: 43 [50.0%]  (CAN_FALLTHRU) 38 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L925
# BLOCK 38 freq:16 seq:36
# PRED: 37 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugGtalk(%rip), %r14d
	testl	%r14d, %r14d
# SUCC: 43 [50.0%]  (CAN_FALLTHRU) 39 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L925
# BLOCK 39 freq:8 seq:37
# PRED: 38 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugSkype(%rip)
# SUCC: 43 [50.0%]  (CAN_FALLTHRU) 40 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L925
# BLOCK 40 freq:4 seq:38
# PRED: 39 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugYahooMsg(%rip)
# SUCC: 43 [50.0%]  (CAN_FALLTHRU) 41 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L925
# BLOCK 41 freq:2 seq:39
# PRED: 40 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugFBchat(%rip)
# SUCC: 43 [50.0%]  (CAN_FALLTHRU) 42 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L925
# BLOCK 42 freq:1 seq:40
# PRED: 41 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugCitrixOnline(%rip)
# SUCC: 43 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 44 [50.0%]  (CAN_FALLTHRU)
	je	.L926
# BLOCK 43 freq:92 seq:41
# PRED: 36 [50.0%]  (CAN_FALLTHRU) 35 [29.0%]  (CAN_FALLTHRU) 42 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 41 [50.0%]  (CAN_FALLTHRU) 40 [50.0%]  (CAN_FALLTHRU) 39 [50.0%]  (CAN_FALLTHRU) 38 [50.0%]  (CAN_FALLTHRU) 37 [50.0%]  (CAN_FALLTHRU)
.L925:
	movl	%r15d, %ecx
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC196, %edi
	xorl	%eax, %eax
# SUCC: 44 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 44 freq:92 seq:42
# PRED: 42 [50.0%]  (CAN_FALLTHRU) 43 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L926:
	movl	$16, %r8d
	movl	$20, %edx
	movl	$1, %esi
	leaq	112(%rsp), %rcx
	movl	%r15d, %edi
	movl	$0, 16(%rsp)
	movq	$3, 112(%rsp)
	movq	$500000, 120(%rsp)
	call	setsockopt
	movl	$16, %r8d
	movl	$21, %edx
	leaq	112(%rsp), %rcx
	movl	$1, %esi
	movl	%r15d, %edi
	call	setsockopt
	leaq	16(%rsp), %r9
	movl	%r12d, %edx
	leaq	128(%rsp), %r8
	movq	%rbx, %rsi
	movl	%r15d, %edi
	leaq	48(%rsp), %rcx
	movl	$0, 16(%rsp)
	call	httpsGETroot.constprop.7
	movl	UFDBglobalDebug(%rip), %r11d
	movq	%rax, %r14
	testl	%r11d, %r11d
# SUCC: 52 [50.0%]  (CAN_FALLTHRU) 45 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L927
# BLOCK 45 freq:46 seq:43
# PRED: 44 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	andl	$2, %r13d
# SUCC: 52 [50.0%]  (CAN_FALLTHRU) 46 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L927
# BLOCK 46 freq:23 seq:44
# PRED: 45 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugAim(%rip), %r10d
	testl	%r10d, %r10d
# SUCC: 52 [50.0%]  (CAN_FALLTHRU) 47 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L927
# BLOCK 47 freq:12 seq:45
# PRED: 46 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugGtalk(%rip), %r9d
	testl	%r9d, %r9d
# SUCC: 52 [50.0%]  (CAN_FALLTHRU) 48 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L927
# BLOCK 48 freq:6 seq:46
# PRED: 47 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugSkype(%rip)
# SUCC: 52 [50.0%]  (CAN_FALLTHRU) 49 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L927
# BLOCK 49 freq:3 seq:47
# PRED: 48 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugYahooMsg(%rip)
# SUCC: 52 [50.0%]  (CAN_FALLTHRU) 50 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L927
# BLOCK 50 freq:1 seq:48
# PRED: 49 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugFBchat(%rip)
# SUCC: 52 [50.0%]  (CAN_FALLTHRU) 51 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L927
# BLOCK 51 freq:1 seq:49
# PRED: 50 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugCitrixOnline(%rip)
# SUCC: 52 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 53 [50.0%]  (CAN_FALLTHRU)
	je	.L928
# BLOCK 52 freq:92 seq:50
# PRED: 45 [50.0%]  (CAN_FALLTHRU) 44 [50.0%]  (CAN_FALLTHRU) 51 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 50 [50.0%]  (CAN_FALLTHRU) 49 [50.0%]  (CAN_FALLTHRU) 48 [50.0%]  (CAN_FALLTHRU) 47 [50.0%]  (CAN_FALLTHRU) 46 [50.0%]  (CAN_FALLTHRU)
.L927:
	movl	16(%rsp), %edi
	testq	%r14, %r14
	movl	$.LC15, %eax
	movl	$.LC16, %r13d
	cmovne	%rax, %r13
	call	ufdbAPIstatusString
	movq	%r13, %r8
	movl	%r12d, %edx
	movq	%rax, %rcx
	movq	%rbx, %rsi
	movl	$.LC197, %edi
	xorl	%eax, %eax
# SUCC: 53 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 53 freq:92 seq:51
# PRED: 51 [50.0%]  (CAN_FALLTHRU) 52 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L928:
	testq	%r14, %r14
# SUCC: 117 [15.0%]  (CAN_FALLTHRU) 54 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L930
# BLOCK 54 freq:78 seq:52
# PRED: 53 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	movzbl	(%r14), %r13d
	testb	%r13b, %r13b
# SUCC: 117 [50.0%]  (CAN_FALLTHRU) 55 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L930
# BLOCK 55 freq:39 seq:53
# PRED: 54 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	16(%rsp), %edi
	testl	%edi, %edi
# SUCC: 81 [50.0%]  (CAN_FALLTHRU) 56 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L933
# BLOCK 56 freq:20 seq:54
# PRED: 55 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC28, %edi
	movl	$5, %ecx
	movq	%r14, %rsi
	repz cmpsb
# SUCC: 57 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 133 [50.0%]  (CAN_FALLTHRU)
	jne	.L1063
# BLOCK 57 seq:55
# PRED: 56 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 133 [50.0%]  (CAN_FALLTHRU)
.L935:
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 58 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 131 [27.0%]  (CAN_FALLTHRU)
	jle	.L971
# BLOCK 58 freq:11 seq:56
# PRED: 57 [73.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$1, %ecx
	xorl	%edx, %edx
# SUCC: 62 [100.0%] 
	jmp	.L942
# BLOCK 59 freq:93 seq:57
# PRED: 63 [50.0%]  (CAN_FALLTHRU) 62 [50.0%]  (CAN_FALLTHRU)
.L979:
	movslq	%ecx, %rax
# SUCC: 60 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movb	$95, 1152(%rsp,%rdx)
# BLOCK 60 freq:186 seq:58
# PRED: 65 [100.0%]  59 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 125 [100.0%] 
.L939:
	movzbl	1(%r14,%rdx), %r13d
	testb	%r13b, %r13b
# SUCC: 61 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 130 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L941
# BLOCK 61 freq:178 seq:59
# PRED: 60 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	addq	$1, %rdx
	addl	$1, %ecx
	cmpq	$2599, %rdx
# SUCC: 62 [99.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 130 [1.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L941
# BLOCK 62 freq:186 seq:60
# PRED: 61 [99.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 58 [100.0%] 
.L942:
	cmpb	$10, %r13b
# SUCC: 59 [50.0%]  (CAN_FALLTHRU) 63 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L979
# BLOCK 63 freq:93 seq:61
# PRED: 62 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpb	$13, %r13b
# SUCC: 64 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 59 [50.0%]  (CAN_FALLTHRU)
	je	.L979
# BLOCK 64 freq:93 seq:62
# PRED: 63 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdx, (%rsp)
	movl	%ecx, 12(%rsp)
	call	__ctype_b_loc
	movsbq	%r13b, %rsi
	movl	12(%rsp), %ecx
	movq	(%rax), %rax
	movq	(%rsp), %rdx
	testb	$64, 1(%rax,%rsi,2)
	movslq	%ecx, %rax
# SUCC: 65 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 125 [50.0%]  (CAN_FALLTHRU)
	je	.L940
# BLOCK 65 freq:46 seq:63
# PRED: 64 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movb	%r13b, 1152(%rsp,%rdx)
# SUCC: 60 [100.0%] 
	jmp	.L939
# BLOCK 66 freq:258 seq:64
# PRED: 27 [46.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L891:
# SUCC: 67 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$2, 16(%rsp)
# BLOCK 67 freq:65 seq:65
# PRED: 70 (CAN_FALLTHRU) 66 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L968:
	leaq	80(%rsp), %rdx
	movl	%r12d, %esi
	movq	%rbx, %rdi
	call	insertHTTPScache.constprop.5
# SUCC: 33 [100.0%] 
	jmp	.L898
# BLOCK 68 freq:97 seq:66
# PRED: 28 [33.0%]  (CAN_FALLTHRU)
.L1059:
	cmpl	$8, %eax
# SUCC: 126 [28.0%]  (CAN_FALLTHRU) 69 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1064
# BLOCK 69 freq:70 seq:67
# PRED: 68 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$28, %eax
	movq	%r15, 8(%r14)
	movl	%eax, 16(%rsp)
# SUCC: 30 (CAN_FALLTHRU) 70 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1055
# BLOCK 70 freq:211 seq:68
# PRED: 69 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$2, %eax
# SUCC: 67 (CAN_FALLTHRU) 71 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L968
# BLOCK 71 freq:411 seq:69
# PRED: 70 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$10, %eax
# SUCC: 30 [13.7%]  (CAN_FALLTHRU) 72 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1055
# BLOCK 72 freq:355 seq:70
# PRED: 71 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$31, %eax
	.p2align 4,,2
# SUCC: 8 [62.2%]  (CAN_FALLTHRU) 73 [37.8%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1048
# BLOCK 73 freq:134 seq:71
# PRED: 72 [37.8%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalTunnelCheckMethod(%rip), %eax
	cmpl	$2, %eax
# SUCC: 74 [86.3%]  (FALLTHRU,CAN_FALLTHRU) 95 [13.7%]  (CAN_FALLTHRU)
	je	.L903
# BLOCK 74 freq:116 seq:72
# PRED: 73 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	testb	$1, %r13b
# SUCC: 75 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 95 [50.0%]  (CAN_FALLTHRU)
	je	.L903
# BLOCK 75 freq:58 seq:73
# PRED: 74 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$1, %eax
# SUCC: 145 [13.7%]  (CAN_FALLTHRU) 76 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L904
# BLOCK 76 freq:60 seq:74
# PRED: 161 [50.0%]  (CAN_FALLTHRU) 75 [86.3%]  (FALLTHRU,CAN_FALLTHRU) 145 [100.0%] 
.L1051:
	movl	$31, %eax
# SUCC: 8 [100.0%] 
	jmp	.L1048
# BLOCK 77 seq:75
# PRED: 168 [30.2%]  (CAN_FALLTHRU)
.L1074:
	movl	$.LC206, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 169 [85.0%]  (CAN_FALLTHRU) 78 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L946
# BLOCK 78 seq:76
# PRED: 77 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC207, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 169 [85.0%]  (CAN_FALLTHRU) 79 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L946
# BLOCK 79 seq:77
# PRED: 78 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC209, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 80 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 171 [30.2%]  (CAN_FALLTHRU)
	je	.L1065
# BLOCK 80 seq:78
# PRED: 171 [85.0%]  (CAN_FALLTHRU) 79 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 172 [85.0%]  (CAN_FALLTHRU)
.L948:
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC212, %edi
	xorl	%eax, %eax
	call	ufdbLogError
# SUCC: 81 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$11, 16(%rsp)
# BLOCK 81 freq:77 seq:79
# PRED: 132 [100.0%]  117 [71.0%]  (CAN_FALLTHRU) 151 [100.0%]  160 [100.0%]  174 [100.0%]  169 [100.0%]  80 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 55 [50.0%]  (CAN_FALLTHRU) 178 [100.0%] 
.L933:
	movl	%r15d, %edi
	call	close
	movl	UFDBglobalDebug(%rip), %esi
	testl	%esi, %esi
# SUCC: 89 [29.0%]  (CAN_FALLTHRU) 82 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L960
# BLOCK 82 freq:55 seq:80
# PRED: 81 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalLogAllRequests(%rip), %ecx
	testl	%ecx, %ecx
# SUCC: 89 [50.0%]  (CAN_FALLTHRU) 83 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L960
# BLOCK 83 freq:27 seq:81
# PRED: 82 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugAim(%rip), %edx
	testl	%edx, %edx
# SUCC: 89 [50.0%]  (CAN_FALLTHRU) 84 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L960
# BLOCK 84 freq:14 seq:82
# PRED: 83 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugGtalk(%rip), %eax
	testl	%eax, %eax
# SUCC: 89 [50.0%]  (CAN_FALLTHRU) 85 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L960
# BLOCK 85 freq:7 seq:83
# PRED: 84 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugSkype(%rip)
# SUCC: 89 [50.0%]  (CAN_FALLTHRU) 86 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L960
# BLOCK 86 freq:3 seq:84
# PRED: 85 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugYahooMsg(%rip)
# SUCC: 89 [50.0%]  (CAN_FALLTHRU) 87 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L960
# BLOCK 87 freq:2 seq:85
# PRED: 86 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugFBchat(%rip)
# SUCC: 89 [50.0%]  (CAN_FALLTHRU) 88 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L960
# BLOCK 88 freq:1 seq:86
# PRED: 87 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugCitrixOnline(%rip)
# SUCC: 89 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 90 [50.0%]  (CAN_FALLTHRU)
	je	.L961
# BLOCK 89 freq:76 seq:87
# PRED: 82 [50.0%]  (CAN_FALLTHRU) 81 [29.0%]  (CAN_FALLTHRU) 88 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 87 [50.0%]  (CAN_FALLTHRU) 86 [50.0%]  (CAN_FALLTHRU) 85 [50.0%]  (CAN_FALLTHRU) 84 [50.0%]  (CAN_FALLTHRU) 83 [50.0%]  (CAN_FALLTHRU)
.L960:
	movl	16(%rsp), %edi
	call	ufdbAPIstatusString
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movq	%rax, %rcx
	movl	$.LC219, %edi
	xorl	%eax, %eax
# SUCC: 90 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 90 freq:77 seq:88
# PRED: 88 [50.0%]  (CAN_FALLTHRU) 89 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L961:
	cmpl	$3, UFDBglobalTunnelCheckMethod(%rip)
# SUCC: 129 [28.0%]  (CAN_FALLTHRU) 91 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L962
# BLOCK 91 freq:55 seq:89
# PRED: 90 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
# SUCC: 92 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	16(%rsp), %r13d
# BLOCK 92 freq:77 seq:90
# PRED: 91 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 129 [100.0%] 
.L963:
	movl	%r12d, %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	leaq	1152(%rsp), %rdi
	xorl	%eax, %eax
	call	sprintf
	xorl	%edi, %edi
	call	time
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	1152(%rsp), %rsi
	movq	%rax, %r12
	call	UFDBsearchHashtable
	testq	%rax, %rax
	movq	%rax, %rbx
# SUCC: 163 [4.4%]  (CAN_FALLTHRU) 93 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1066
# BLOCK 93 freq:73 seq:91
# PRED: 92 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r13d, (%rax)
# SUCC: 94 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r12, 8(%rax)
# BLOCK 94 freq:77 seq:92
# PRED: 93 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 163 [100.0%] 
.L965:
	movq	%r14, %rdi
	movq	%rbx, 80(%rsp)
	call	ufdbFree
# SUCC: 25 [100.0%] 
	jmp	.L1052
# BLOCK 95 freq:76 seq:93
# PRED: 74 [50.0%]  (CAN_FALLTHRU) 73 [13.7%]  (CAN_FALLTHRU)
.L903:
	call	random
	xorl	%r14d, %r14d
	movabsq	$3778082090400236183, %rdx
	movq	%rax, %rcx
	imulq	%rdx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$12, %rdx
	subq	%rax, %rdx
	imulq	$19999, %rdx, %rdx
	subq	%rdx, %rcx
	leal	35000(%rcx), %edi
	call	usleep
	movl	%r13d, %eax
	andl	$2, %eax
	movl	%eax, 12(%rsp)
# SUCC: 96 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
# BLOCK 96 freq:1013 seq:94
# PRED: 110 [98.6%]  (DFS_BACK,CAN_FALLTHRU) 95 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 114 [50.0%]  (DFS_BACK,CAN_FALLTHRU) 115 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L905:
	cmpl	$31, 16(%rsp)
# SUCC: 97 [97.8%]  (FALLTHRU,CAN_FALLTHRU) 33 [2.2%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L898
# BLOCK 97 freq:991 seq:95
# PRED: 96 [97.8%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 7 [2.2%]  (CAN_FALLTHRU,LOOP_EXIT) 98 [97.8%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1050
# BLOCK 98 freq:968 seq:96
# PRED: 97 [97.8%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r12d, %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	leaq	1152(%rsp), %rdi
	call	sprintf
	movq	myht(%rip), %rdi
	xorl	%edx, %edx
	leaq	1152(%rsp), %rsi
	call	UFDBsearchHashtable
	testq	%rax, %rax
	movq	%rax, %r15
# SUCC: 106 [46.7%]  (CAN_FALLTHRU,LOOP_EXIT) 99 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L973
# BLOCK 99 freq:516 seq:97
# PRED: 98 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%edi, %edi
	call	time
	movq	8(%r15), %rcx
	movl	(%r15), %esi
	movq	%rax, %rdi
	movq	%r15, 80(%rsp)
	cmpq	UFDBglobalDatabaseLoadTime(%rip), %rcx
# SUCC: 100 [67.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 107 [33.0%]  (CAN_FALLTHRU)
	jge	.L907
# BLOCK 100 freq:380 seq:98
# PRED: 99 [67.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 122 [67.0%]  (CAN_FALLTHRU) 147 [83.9%]  (CAN_FALLTHRU)
.L909:
# SUCC: 101 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$28, %eax
# BLOCK 101 freq:832 seq:99
# PRED: 100 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 106 [100.0%] 
.L906:
# SUCC: 102 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%eax, 16(%rsp)
# BLOCK 102 freq:22 seq:100
# PRED: 108 (CAN_FALLTHRU,LOOP_EXIT) 101 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 124 [100.0%] 
.L911:
	cmpl	$9, %r14d
# SUCC: 103 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 25 [39.0%]  (CAN_FALLTHRU)
	jle	.L1052
# BLOCK 103 freq:13 seq:101
# PRED: 102 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 105 [52.5%]  (CAN_FALLTHRU) 104 [47.5%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L916
# BLOCK 104 freq:6 seq:102
# PRED: 103 [47.5%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, 12(%rsp)
# SUCC: 105 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 25 [39.0%]  (CAN_FALLTHRU)
	je	.L1052
# BLOCK 105 freq:11 seq:103
# PRED: 104 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 103 [52.5%]  (CAN_FALLTHRU)
.L916:
	movl	%r12d, %ecx
	movq	%rbx, %rdx
	movl	%r14d, %esi
	movl	$.LC193, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 25 [100.0%] 
	jmp	.L1052
# BLOCK 106 freq:452 seq:104
# PRED: 98 [46.7%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L973:
	movl	$2, %eax
# SUCC: 101 [100.0%] 
	jmp	.L906
# BLOCK 107 freq:170 seq:105
# PRED: 99 [33.0%]  (CAN_FALLTHRU)
.L907:
	cmpl	$8, %esi
	.p2align 4,,3
# SUCC: 122 [28.0%]  (CAN_FALLTHRU,LOOP_EXIT) 108 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1067
# BLOCK 108 freq:122 seq:106
# PRED: 107 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$31, %esi
	movq	%rax, 8(%r15)
	movl	%esi, 16(%rsp)
# SUCC: 109 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 102 (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L911
# BLOCK 109 freq:947 seq:107
# PRED: 108 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	random
	addl	$1, %r14d
	movq	%rax, %rdi
	movabsq	$5440318517184312447, %rax
	imulq	%rdi
	movq	%rdi, %rax
	sarq	$63, %rax
	sarq	$13, %rdx
	subq	%rax, %rdx
	imulq	$27777, %rdx, %rdx
	subq	%rdx, %rdi
	addl	$80000, %edi
	call	usleep
	cmpl	$10, %r14d
# SUCC: 113 [28.0%]  (CAN_FALLTHRU) 110 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1068
# BLOCK 110 freq:682 seq:108
# PRED: 109 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$150, %r14d
# SUCC: 111 [1.4%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 96 [98.6%]  (DFS_BACK,CAN_FALLTHRU)
	jne	.L905
# BLOCK 111 freq:9 seq:109
# PRED: 110 [1.4%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 112 [69.2%]  (FALLTHRU,CAN_FALLTHRU) 161 [30.8%]  (CAN_FALLTHRU)
	jle	.L1069
# BLOCK 112 freq:8 seq:110
# PRED: 111 [69.2%]  (FALLTHRU,CAN_FALLTHRU) 162 [100.0%] 
.L918:
	xorl	%eax, %eax
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC194, %edi
	call	ufdbLogMessage
	movl	$31, %eax
# SUCC: 8 [100.0%] 
	jmp	.L1048
# BLOCK 113 freq:265 seq:111
# PRED: 109 [28.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1068:
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 115 [52.5%]  (CAN_FALLTHRU) 114 [47.5%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L913
# BLOCK 114 freq:126 seq:112
# PRED: 113 [47.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	12(%rsp), %eax
	testl	%eax, %eax
# SUCC: 115 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 96 [50.0%]  (DFS_BACK,CAN_FALLTHRU)
	je	.L905
# BLOCK 115 freq:202 seq:113
# PRED: 114 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 113 [52.5%]  (CAN_FALLTHRU)
.L913:
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC192, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 96 [100.0%] 
	jmp	.L905
# BLOCK 116 freq:76 seq:114
# PRED: 34 [45.3%]  (CAN_FALLTHRU)
.L1062:
	movl	%r15d, %edi
	call	close
	movl	$8, %eax
# SUCC: 8 [100.0%] 
	jmp	.L1048
# BLOCK 117 freq:53 seq:115
# PRED: 54 [50.0%]  (CAN_FALLTHRU) 53 [15.0%]  (CAN_FALLTHRU)
.L930:
	movl	16(%rsp), %r8d
	testl	%r8d, %r8d
# SUCC: 118 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 81 [71.0%]  (CAN_FALLTHRU)
	jne	.L933
# BLOCK 118 freq:15 seq:116
# PRED: 117 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%eax, %eax
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC198, %edi
	call	ufdbLogMessage
	movl	%r15d, %edi
	call	close
	xorl	%eax, %eax
	testq	%r14, %r14
# SUCC: 119 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 8 [30.2%]  (CAN_FALLTHRU)
	je	.L1048
# BLOCK 119 freq:11 seq:117
# PRED: 118 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%r14, %rdi
	movl	%eax, 12(%rsp)
	call	ufdbFree
	movl	12(%rsp), %eax
# SUCC: 8 [100.0%] 
	jmp	.L1048
# BLOCK 120 freq:52 seq:118
# PRED: 33 [23.5%]  (CAN_FALLTHRU)
.L1061:
	movq	%rbx, %rsi
	xorl	%eax, %eax
	movl	%r12d, %edx
	movl	$.LC195, %edi
	call	ufdbLogError
	movq	%rbx, %rdx
	movl	%r12d, %ecx
	leaq	1152(%rsp), %rdi
	movl	$.LC19, %esi
	xorl	%eax, %eax
	call	sprintf
	xorl	%edi, %edi
	call	time
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	1152(%rsp), %rsi
	movq	%rax, %rbx
	call	UFDBsearchHashtable
	testq	%rax, %rax
# SUCC: 167 [4.4%]  (CAN_FALLTHRU) 121 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1070
# BLOCK 121 freq:49 seq:119
# PRED: 120 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$8, (%rax)
	movq	%rbx, 8(%rax)
# SUCC: 7 [100.0%] 
	jmp	.L1050
# BLOCK 122 freq:48 seq:120
# PRED: 107 [28.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L1067:
	leaq	-1800(%rax), %rax
	cmpq	%rcx, %rax
# SUCC: 100 [67.0%]  (CAN_FALLTHRU) 123 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L909
# BLOCK 123 freq:16 seq:121
# PRED: 122 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	-600(%rdi), %rax
	cmpq	%rcx, %rax
# SUCC: 146 [29.0%]  (CAN_FALLTHRU) 124 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L1071
# BLOCK 124 freq:14 seq:122
# PRED: 123 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 146 [50.0%]  (CAN_FALLTHRU) 148 [100.0%] 
.L910:
	movq	%rdi, 8(%r15)
	movl	$8, 16(%rsp)
# SUCC: 102 [100.0%] 
	jmp	.L911
# BLOCK 125 freq:46 seq:123
# PRED: 64 [50.0%]  (CAN_FALLTHRU)
.L940:
	movb	$46, 1152(%rsp,%rdx)
# SUCC: 60 [100.0%] 
	jmp	.L939
# BLOCK 126 freq:27 seq:124
# PRED: 68 [28.0%]  (CAN_FALLTHRU)
.L1064:
	leaq	-1800(%r15), %rax
	cmpq	%rax, %rdx
# SUCC: 29 [67.0%]  (CAN_FALLTHRU) 127 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	jl	.L892
# BLOCK 127 freq:9 seq:125
# PRED: 126 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	-600(%r15), %rax
	cmpq	%rax, %rdx
# SUCC: 164 [29.0%]  (CAN_FALLTHRU) 128 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jl	.L1072
# BLOCK 128 freq:8 seq:126
# PRED: 127 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 164 [50.0%]  (CAN_FALLTHRU) 166 [100.0%] 
.L894:
	movq	%r15, 8(%r14)
# SUCC: 7 [100.0%] 
	jmp	.L1050
# BLOCK 129 freq:22 seq:127
# PRED: 90 [28.0%]  (CAN_FALLTHRU)
.L962:
	movl	$0, 16(%rsp)
	xorl	%r13d, %r13d
# SUCC: 92 [100.0%] 
	jmp	.L963
# BLOCK 130 freq:11 seq:128
# PRED: 60 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 61 [1.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L941:
	movb	$0, 1152(%rsp,%rax)
	movl	%r12d, %edx
	movq	%rbx, %rsi
	leaq	1152(%rsp), %rcx
	movl	$.LC199, %edi
	xorl	%eax, %eax
# SUCC: 131 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 131 freq:15 seq:129
# PRED: 130 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 57 [27.0%]  (CAN_FALLTHRU)
.L971:
	movl	$.LC200, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 132 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 158 [30.2%]  (CAN_FALLTHRU)
	je	.L1073
# BLOCK 132 freq:14 seq:130
# PRED: 158 [85.0%]  (CAN_FALLTHRU) 131 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
.L943:
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC202, %edi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$11, 16(%rsp)
# SUCC: 81 [100.0%] 
	jmp	.L933
# BLOCK 133 freq:10 seq:131
# PRED: 56 [50.0%]  (CAN_FALLTHRU)
.L1063:
	movl	$.LC30, %edi
	movl	$5, %ecx
	movq	%r14, %rsi
	repz cmpsb
# SUCC: 57 [50.0%]  (CAN_FALLTHRU) 134 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L935
# BLOCK 134 freq:5 seq:132
# PRED: 133 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalUnknownProtocolOverHttps(%rip)
	movl	$.LC190, %eax
	movl	%r12d, %edx
	movl	$.LC189, %ecx
	movq	%rbx, %rsi
	movl	$.LC218, %edi
	cmove	%rax, %rcx
	xorl	%eax, %eax
	call	ufdbLogMessage
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 135 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 151 [27.0%]  (CAN_FALLTHRU)
	jle	.L952
# BLOCK 135 freq:4 seq:133
# PRED: 134 [73.0%]  (FALLTHRU,CAN_FALLTHRU)
	movzbl	(%r14), %edx
	testb	%dl, %dl
# SUCC: 136 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 149 [4.5%]  (CAN_FALLTHRU)
	je	.L977
# BLOCK 136 freq:4 seq:134
# PRED: 135 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$1, %ecx
	xorl	%r13d, %r13d
# SUCC: 140 [100.0%] 
	jmp	.L958
# BLOCK 137 freq:31 seq:135
# PRED: 141 [50.0%]  (CAN_FALLTHRU) 140 [50.0%]  (CAN_FALLTHRU)
.L980:
	movb	$95, 1152(%rsp,%r13)
# SUCC: 138 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movslq	%ecx, %rax
# BLOCK 138 freq:62 seq:136
# PRED: 143 [100.0%]  137 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 144 [100.0%] 
.L956:
	movzbl	1(%r14,%r13), %edx
	testb	%dl, %dl
# SUCC: 139 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 150 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L953
# BLOCK 139 freq:59 seq:137
# PRED: 138 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	addq	$1, %r13
	addl	$1, %ecx
	cmpq	$2399, %r13
# SUCC: 140 [99.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 150 [1.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L953
# BLOCK 140 freq:62 seq:138
# PRED: 139 [99.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 136 [100.0%] 
.L958:
	cmpb	$10, %dl
# SUCC: 137 [50.0%]  (CAN_FALLTHRU) 141 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L980
# BLOCK 141 freq:31 seq:139
# PRED: 140 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpb	$13, %dl
# SUCC: 142 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 137 [50.0%]  (CAN_FALLTHRU)
	je	.L980
# BLOCK 142 freq:31 seq:140
# PRED: 141 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%ecx, (%rsp)
	movl	%edx, 12(%rsp)
	call	__ctype_b_loc
	movl	12(%rsp), %edx
	movq	(%rax), %rax
	movl	(%rsp), %ecx
	movsbq	%dl, %rsi
	testb	$64, 1(%rax,%rsi,2)
	movslq	%ecx, %rax
# SUCC: 143 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 144 [50.0%]  (CAN_FALLTHRU)
	je	.L957
# BLOCK 143 freq:15 seq:141
# PRED: 142 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movb	%dl, 1152(%rsp,%r13)
# SUCC: 138 [100.0%] 
	jmp	.L956
# BLOCK 144 freq:15 seq:142
# PRED: 142 [50.0%]  (CAN_FALLTHRU)
.L957:
	movb	$46, 1152(%rsp,%r13)
# SUCC: 138 [100.0%] 
	jmp	.L956
# BLOCK 145 freq:8 seq:143
# PRED: 75 [13.7%]  (CAN_FALLTHRU)
.L904:
	movl	$100000, %edi
	call	usleep
# SUCC: 76 [100.0%] 
	jmp	.L1051
# BLOCK 146 freq:5 seq:144
# PRED: 123 [29.0%]  (CAN_FALLTHRU)
.L1071:
	movq	%rdi, (%rsp)
	call	UFDBhttpsVerificationQueued
	movq	(%rsp), %rdi
	cmpl	$31, %eax
# SUCC: 147 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 124 [50.0%]  (CAN_FALLTHRU)
	jg	.L910
# BLOCK 147 freq:2 seq:145
# PRED: 146 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$2, UFDBglobalTunnelCheckMethod(%rip)
# SUCC: 100 [83.9%]  (CAN_FALLTHRU) 148 [16.1%]  (FALLTHRU)
	jne	.L909
# BLOCK 148 seq:146
# PRED: 147 [16.1%]  (FALLTHRU)
# SUCC: 124 [100.0%] 
	jmp	.L910
# BLOCK 149 seq:147
# PRED: 135 [4.5%]  (CAN_FALLTHRU)
.L977:
# SUCC: 150 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%eax, %eax
# BLOCK 150 freq:4 seq:148
# PRED: 138 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 149 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 139 [1.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L953:
	movb	$0, 1152(%rsp,%rax)
	movl	%r12d, %edx
	movq	%rbx, %rsi
	leaq	1152(%rsp), %rcx
	movl	$.LC199, %edi
	xorl	%eax, %eax
# SUCC: 151 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 151 freq:5 seq:149
# PRED: 134 [27.0%]  (CAN_FALLTHRU) 150 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L952:
	cmpl	$1, UFDBglobalUnknownProtocolOverHttps(%rip)
	sbbl	%eax, %eax
	andl	$-8, %eax
	addl	$19, %eax
	movl	%eax, 16(%rsp)
# SUCC: 81 [100.0%] 
	jmp	.L933
# BLOCK 152 freq:5 seq:150
# PRED: 30 [4.4%]  (CAN_FALLTHRU)
.L1060:
	movl	$1112, %edi
	call	ufdbMalloc
	leaq	1152(%rsp), %rdi
	movq	%r15, 8(%rax)
	movq	%rax, %r14
	movl	$31, (%rax)
	movq	$0, 16(%rax)
	movb	$0, 24(%rax)
	movb	$0, 88(%rax)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%r14, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
# SUCC: 32 [100.0%] 
	jmp	.L902
# BLOCK 153 freq:4 seq:151
# PRED: 21 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
.L1057:
	movq	%rbx, %rcx
# SUCC: 155 [100.0%] 
	jmp	.L885
# BLOCK 154 freq:50 seq:152
# PRED: 156 [95.5%]  (CAN_FALLTHRU) 155 [28.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L884:
	addq	$1, %rcx
	movzbl	(%rcx), %edx
	testb	%dl, %dl
# SUCC: 155 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 23 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L882
# BLOCK 155 freq:52 seq:153
# PRED: 154 [95.5%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 153 [100.0%] 
.L885:
	cmpb	$58, %dl
# SUCC: 156 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 154 [28.0%]  (CAN_FALLTHRU)
	je	.L884
# BLOCK 156 freq:38 seq:154
# PRED: 155 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	movsbq	%dl, %rdx
	testb	$16, 1(%rax,%rdx,2)
# SUCC: 157 [4.5%]  (FALLTHRU) 154 [95.5%]  (CAN_FALLTHRU)
	jne	.L884
# BLOCK 157 freq:2 seq:155
# PRED: 156 [4.5%]  (FALLTHRU)
# SUCC: 6 [100.0%] 
	jmp	.L881
# BLOCK 158 freq:4 seq:156
# PRED: 131 [30.2%]  (CAN_FALLTHRU)
.L1073:
	movl	$.LC201, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 132 [85.0%]  (CAN_FALLTHRU) 159 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L943
# BLOCK 159 freq:1 seq:157
# PRED: 158 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC203, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 160 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 168 [30.2%]  (CAN_FALLTHRU)
	je	.L945
# BLOCK 160 seq:158
# PRED: 159 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC204, %edi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$11, 16(%rsp)
# SUCC: 81 [100.0%] 
	jmp	.L933
# BLOCK 161 freq:3 seq:159
# PRED: 111 [30.8%]  (CAN_FALLTHRU)
.L1069:
	cmpl	$0, 12(%rsp)
# SUCC: 162 [50.0%]  (FALLTHRU) 76 [50.0%]  (CAN_FALLTHRU)
	je	.L1051
# BLOCK 162 freq:2 seq:160
# PRED: 161 [50.0%]  (FALLTHRU)
# SUCC: 112 [100.0%] 
	jmp	.L918
# BLOCK 163 freq:3 seq:161
# PRED: 92 [4.4%]  (CAN_FALLTHRU)
.L1066:
	movl	$1112, %edi
	call	ufdbMalloc
	leaq	1152(%rsp), %rdi
	movq	%r12, 8(%rax)
	movq	%rax, %rbx
	movl	%r13d, (%rax)
	movq	$0, 16(%rax)
	movb	$0, 24(%rax)
	movb	$0, 88(%rax)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%rbx, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
# SUCC: 94 [100.0%] 
	jmp	.L965
# BLOCK 164 freq:3 seq:162
# PRED: 127 [29.0%]  (CAN_FALLTHRU)
.L1072:
	call	UFDBhttpsVerificationQueued
	cmpl	$31, %eax
	.p2align 4,,5
# SUCC: 165 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 128 [50.0%]  (CAN_FALLTHRU)
	jg	.L894
# BLOCK 165 freq:1 seq:163
# PRED: 164 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$2, UFDBglobalTunnelCheckMethod(%rip)
# SUCC: 29 [83.9%]  (CAN_FALLTHRU) 166 [16.1%]  (FALLTHRU)
	jne	.L892
# BLOCK 166 seq:164
# PRED: 165 [16.1%]  (FALLTHRU)
# SUCC: 128 [100.0%] 
	jmp	.L894
# BLOCK 167 freq:2 seq:165
# PRED: 120 [4.4%]  (CAN_FALLTHRU)
.L1070:
	movl	$1112, %edi
	call	ufdbMalloc
	leaq	1152(%rsp), %rdi
	movq	%rbx, 8(%rax)
	movq	%rax, %r12
	movl	$8, (%rax)
	movq	$0, 16(%rax)
	movb	$0, 24(%rax)
	movb	$0, 88(%rax)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%r12, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
# SUCC: 7 [100.0%] 
	jmp	.L1050
# BLOCK 168 seq:166
# PRED: 159 [30.2%]  (CAN_FALLTHRU)
.L945:
	movl	$.LC205, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 169 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 77 [30.2%]  (CAN_FALLTHRU)
	je	.L1074
# BLOCK 169 seq:167
# PRED: 77 [85.0%]  (CAN_FALLTHRU) 168 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 78 [85.0%]  (CAN_FALLTHRU)
.L946:
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC208, %edi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$11, 16(%rsp)
# SUCC: 81 [100.0%] 
	jmp	.L933
# BLOCK 170 seq:168
# PRED: 23 [4.4%]  (CAN_FALLTHRU)
.L1058:
	movl	$1112, %edi
	call	ufdbMalloc
	leaq	1152(%rsp), %rdi
	movq	%r12, 8(%rax)
	movq	%rax, %rbx
	movl	$13, (%rax)
	movq	$0, 16(%rax)
	movb	$0, 24(%rax)
	movb	$0, 88(%rax)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%rbx, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
# SUCC: 25 [100.0%] 
	jmp	.L1052
# BLOCK 171 seq:169
# PRED: 79 [30.2%]  (CAN_FALLTHRU)
.L1065:
	movl	$.LC210, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 80 [85.0%]  (CAN_FALLTHRU) 172 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L948
# BLOCK 172 seq:170
# PRED: 171 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC211, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 80 [85.0%]  (CAN_FALLTHRU) 173 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L948
# BLOCK 173 seq:171
# PRED: 172 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC213, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 174 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 175 [30.2%]  (CAN_FALLTHRU)
	je	.L1075
# BLOCK 174 seq:172
# PRED: 175 [85.0%]  (CAN_FALLTHRU) 173 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 177 [85.0%]  (CAN_FALLTHRU) 176 [85.0%]  (CAN_FALLTHRU)
.L950:
	movl	%r12d, %edx
	movq	%rbx, %rsi
	movl	$.LC217, %edi
	xorl	%eax, %eax
	call	ufdbLogError
	movl	$11, 16(%rsp)
# SUCC: 81 [100.0%] 
	jmp	.L933
# BLOCK 175 seq:173
# PRED: 173 [30.2%]  (CAN_FALLTHRU)
.L1075:
	movl	$.LC214, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 174 [85.0%]  (CAN_FALLTHRU) 176 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L950
# BLOCK 176 seq:174
# PRED: 175 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC215, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 174 [85.0%]  (CAN_FALLTHRU) 177 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L950
# BLOCK 177 seq:175
# PRED: 176 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC216, %esi
	movq	%r14, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 174 [85.0%]  (CAN_FALLTHRU) 178 [15.0%]  (FALLTHRU)
	jne	.L950
# BLOCK 178 seq:176
# PRED: 177 [15.0%]  (FALLTHRU)
# SUCC: 81 [100.0%] 
	jmp	.L933
	.cfi_endproc
.LFE75:
	.size	UFDBverifyPortHasHTTPS, .-UFDBverifyPortHasHTTPS
	.section	.rodata.str1.8
	.align 8
.LC221:
	.string	"UFDBcheckForHTTPStunnel  %s:%d  %04x"
	.align 8
.LC222:
	.string	"UFDBcheckForHTTPStunnel: ufdbHttpsQueueRequest( %s:%d ) returned with code %d"
	.text
	.p2align 4,,15
	.globl	UFDBcheckForHTTPStunnel
	.type	UFDBcheckForHTTPStunnel, @function
UFDBcheckForHTTPStunnel:
.LFB73:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	movl	UFDBglobalTunnelCheckMethod(%rip), %ecx
	movl	$2, %eax
	testl	%ecx, %ecx
# SUCC: 3 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 4 [33.0%]  (CAN_FALLTHRU)
	jne	.L1103
# BLOCK 3 seq:1
# PRED: 2 [67.0%]  (FALLTHRU,CAN_FALLTHRU)
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 4 freq:3300 seq:2
# PRED: 2 [33.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1103:
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movl	%esi, %r12d
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movl	%edx, %ebx
	subq	$1048, %rsp
	.cfi_def_cfa_offset 1088
	movl	UFDBglobalDebug(%rip), %eax
	testl	%eax, %eax
# SUCC: 11 [29.0%]  (CAN_FALLTHRU) 5 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1104
# BLOCK 5 freq:3300 seq:3
# PRED: 4 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 11 [100.0%] 
.L1078:
	testb	$1, %bl
# SUCC: 6 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 9 [50.0%]  (CAN_FALLTHRU)
	je	.L1079
# BLOCK 6 freq:1650 seq:4
# PRED: 5 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r12d, %ecx
	movq	%rbp, %rdx
	movl	$.LC19, %esi
	movq	%rsp, %rdi
	xorl	%eax, %eax
	call	sprintf
	movq	myht(%rip), %rdi
	movq	%rsp, %rsi
	movl	$1, %edx
	call	UFDBsearchHashtable
	testq	%rax, %rax
	movq	%rax, %rbx
# SUCC: 12 [46.7%]  (CAN_FALLTHRU) 7 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1087
# BLOCK 7 freq:879 seq:5
# PRED: 6 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%edi, %edi
	call	time
	movq	8(%rbx), %rdx
	movq	%rax, %r13
	movl	(%rbx), %ecx
	cmpq	UFDBglobalDatabaseLoadTime(%rip), %rdx
# SUCC: 8 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 17 [33.0%]  (CAN_FALLTHRU)
	jge	.L1105
# BLOCK 8 freq:646 seq:6
# PRED: 7 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 20 [67.0%]  (CAN_FALLTHRU) 25 [83.9%]  (CAN_FALLTHRU)
.L1081:
	movl	$28, %eax
# SUCC: 10 [100.0%] 
	jmp	.L1099
# BLOCK 9 freq:1650 seq:7
# PRED: 5 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1079:
	movl	%ebx, %edx
	movl	%r12d, %esi
	movq	%rbp, %rdi
# SUCC: 10 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	UFDBverifyPortHasHTTPS
# BLOCK 10 freq:13300 seq:8
# PRED: 16 [100.0%]  9 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 19 [100.0%]  14 [37.8%]  (CAN_FALLTHRU) 22 [100.0%]  8 [100.0%]  23 [100.0%] 
.L1099:
	addq	$1048, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_restore 12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_restore 13
	.cfi_def_cfa_offset 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 11 freq:957 seq:9
# PRED: 4 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1104:
	.cfi_restore_state
	movl	%edx, %ecx
	xorl	%eax, %eax
	movl	%esi, %edx
	movq	%rdi, %rsi
	movl	$.LC221, %edi
	call	ufdbLogMessage
# SUCC: 5 [100.0%] 
	jmp	.L1078
# BLOCK 12 freq:328 seq:10
# PRED: 18 (CAN_FALLTHRU) 6 [46.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1087:
	movl	%r12d, %ecx
	movq	%rbp, %rdx
	movl	$.LC19, %esi
	movq	%rsp, %rdi
	xorl	%eax, %eax
	call	sprintf
	movl	$1112, %edi
	call	ufdbMalloc
	xorl	%edi, %edi
	movq	%rax, %rbx
	call	time
	movq	%rsp, %rdi
	movl	$10, (%rbx)
	movq	%rax, 8(%rbx)
	movq	$0, 16(%rbx)
	movb	$0, 24(%rbx)
	movb	$0, 88(%rbx)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%rbx, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
	movl	%r12d, %esi
	movq	%rbp, %rdi
	call	ufdbHttpsQueueRequest
	testl	%eax, %eax
	movl	%eax, %ecx
# SUCC: 13 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 16 [29.0%]  (CAN_FALLTHRU)
	je	.L1085
# BLOCK 13 freq:233 seq:11
# PRED: 12 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$18, %eax
# SUCC: 23 [28.0%]  (CAN_FALLTHRU) 14 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1106
# BLOCK 14 freq:168 seq:12
# PRED: 13 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$10, %ecx
	movl	$31, %eax
# SUCC: 10 [37.8%]  (CAN_FALLTHRU) 15 [62.2%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1099
# BLOCK 15 freq:104 seq:13
# PRED: 14 [62.2%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r12d, %edx
	movq	%rbp, %rsi
	movl	$.LC222, %edi
	xorl	%eax, %eax
# SUCC: 16 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogError
# BLOCK 16 freq:200 seq:14
# PRED: 12 [29.0%]  (CAN_FALLTHRU) 15 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1085:
	call	sched_yield
	movl	$31, %eax
# SUCC: 10 [100.0%] 
	jmp	.L1099
# BLOCK 17 freq:290 seq:15
# PRED: 7 [33.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1105:
	cmpl	$8, %ecx
# SUCC: 20 [28.0%]  (CAN_FALLTHRU) 18 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1107
# BLOCK 18 freq:208 seq:16
# PRED: 17 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$2, %ecx
	movq	%rax, 8(%rbx)
# SUCC: 12 (CAN_FALLTHRU) 19 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1087
# BLOCK 19 freq:208 seq:17
# PRED: 18 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%ecx, %eax
# SUCC: 10 [100.0%] 
	jmp	.L1099
# BLOCK 20 freq:81 seq:18
# PRED: 17 [28.0%]  (CAN_FALLTHRU)
.L1107:
	leaq	-1800(%rax), %rax
	cmpq	%rax, %rdx
# SUCC: 8 [67.0%]  (CAN_FALLTHRU) 21 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	jl	.L1081
# BLOCK 21 freq:27 seq:19
# PRED: 20 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	-600(%r13), %rax
	cmpq	%rax, %rdx
# SUCC: 24 [29.0%]  (CAN_FALLTHRU) 22 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jl	.L1108
# BLOCK 22 freq:24 seq:20
# PRED: 21 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 24 [50.0%]  (CAN_FALLTHRU) 26 [100.0%] 
.L1083:
	movq	%r13, 8(%rbx)
	movl	$8, %eax
# SUCC: 10 [100.0%] 
	jmp	.L1099
# BLOCK 23 freq:65 seq:21
# PRED: 13 [28.0%]  (CAN_FALLTHRU)
.L1106:
	movl	$8, (%rbx)
	movl	$8, %eax
# SUCC: 10 [100.0%] 
	jmp	.L1099
# BLOCK 24 freq:8 seq:22
# PRED: 21 [29.0%]  (CAN_FALLTHRU)
.L1108:
	call	UFDBhttpsVerificationQueued
	cmpl	$31, %eax
# SUCC: 25 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 22 [50.0%]  (CAN_FALLTHRU)
	jg	.L1083
# BLOCK 25 freq:4 seq:23
# PRED: 24 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$2, UFDBglobalTunnelCheckMethod(%rip)
# SUCC: 8 [83.9%]  (CAN_FALLTHRU) 26 [16.1%]  (FALLTHRU)
	jne	.L1081
# BLOCK 26 freq:1 seq:24
# PRED: 25 [16.1%]  (FALLTHRU)
# SUCC: 22 [100.0%] 
	jmp	.L1083
	.cfi_endproc
.LFE73:
	.size	UFDBcheckForHTTPStunnel, .-UFDBcheckForHTTPStunnel
	.section	.rodata.str1.8
	.align 8
.LC223:
	.string	"T%02ld: UFDBhttpsTunnelVerifier: start TLS/SSL verification %s:%d ..."
	.text
	.p2align 4,,15
	.globl	UFDBhttpsTunnelVerifier
	.type	UFDBhttpsTunnelVerifier, @function
UFDBhttpsTunnelVerifier:
.LFB74:
	.cfi_startproc
# BLOCK 2 freq:147 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$1184, %rsp
	leaq	16(%rsp), %rdi
	call	sigemptyset
	leaq	16(%rsp), %rdi
	movl	$1, %esi
	call	sigaddset
	leaq	16(%rsp), %rdi
	movl	$10, %esi
	call	sigaddset
	leaq	16(%rsp), %rdi
	movl	$17, %esi
	call	sigaddset
	leaq	16(%rsp), %rdi
	movl	$15, %esi
	call	sigaddset
	leaq	16(%rsp), %rdi
	movl	$2, %esi
	call	sigaddset
	xorl	%edx, %edx
	xorl	%edi, %edi
	leaq	16(%rsp), %rsi
	call	pthread_sigmask
	movl	%ebx, %edi
	call	ufdbSetThreadCPUaffinity
	call	waitForTLSinitilisation
# SUCC: 3 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
# BLOCK 3 freq:2351 seq:1
# PRED: 2 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 4 [71.0%]  (DFS_BACK,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1112:
	movl	UFDBglobalTerminating(%rip), %eax
	testl	%eax, %eax
# SUCC: 4 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 11 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L1126
# BLOCK 4 freq:3105 seq:2
# PRED: 3 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 10 [95.5%]  (CAN_FALLTHRU)
.L1119:
	call	pthread_testcancel
	leaq	144(%rsp), %rdi
	movq	%rsp, %rsi
	call	ufdbGetHttpsRequest
	movl	UFDBglobalTunnelCheckMethod(%rip), %edx
	testl	%edx, %edx
# SUCC: 3 [71.0%]  (DFS_BACK,CAN_FALLTHRU) 5 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1112
# BLOCK 5 freq:900 seq:3
# PRED: 4 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	pthread_testcancel
	movl	UFDBglobalDebug(%rip), %eax
	testl	%eax, %eax
# SUCC: 12 [29.0%]  (CAN_FALLTHRU) 6 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1127
# BLOCK 6 freq:900 seq:4
# PRED: 5 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 12 [100.0%] 
.L1113:
	movl	(%rsp), %esi
	leaq	144(%rsp), %rdi
	xorl	%edx, %edx
	call	UFDBverifyPortHasHTTPS
	movl	UFDBglobalTerminating(%rip), %eax
	testl	%eax, %eax
# SUCC: 7 [0.0%]  (FALLTHRU) 9 [100.0%]  (CAN_FALLTHRU)
	je	.L1125
# BLOCK 7 seq:5
# PRED: 6 [0.0%]  (FALLTHRU)
# SUCC: 13 [100.0%] 
	jmp	.L1128
# BLOCK 8 freq:9100 seq:6
# PRED: 9 [91.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1122:
	call	pthread_testcancel
	movl	$400000, %edi
# SUCC: 9 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU)
	call	usleep
# BLOCK 9 freq:10000 seq:7
# PRED: 8 [100.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 6 [100.0%]  (CAN_FALLTHRU)
.L1125:
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 8 [91.0%]  (CAN_FALLTHRU) 10 [9.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU,LOOP_EXIT)
	jne	.L1122
# BLOCK 10 freq:900 seq:8
# PRED: 9 [9.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU,LOOP_EXIT)
	movl	UFDBglobalTerminating(%rip), %eax
	testl	%eax, %eax
# SUCC: 4 [95.5%]  (CAN_FALLTHRU) 11 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	je	.L1119
# BLOCK 11 freq:146 seq:9
# PRED: 3 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 10 [4.5%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
.L1126:
	xorl	%eax, %eax
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 12 freq:261 seq:10
# PRED: 5 [29.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1127:
	.cfi_restore_state
	movl	(%rsp), %ecx
	leaq	144(%rsp), %rdx
	movq	%rbx, %rsi
	movl	$.LC223, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 6 [100.0%] 
	jmp	.L1113
# BLOCK 13 seq:11
# PRED: 7 [100.0%] 
.L1128:
	xorl	%edi, %edi
# SUCC:
	call	pthread_exit
	.cfi_endproc
.LFE74:
	.size	UFDBhttpsTunnelVerifier, .-UFDBhttpsTunnelVerifier
	.section	.rodata.str1.8
	.align 8
.LC224:
	.string	"W%03d: UFDBsslPeekServer  %s:%d  start"
	.align 8
.LC225:
	.string	"W%03d: UFDBsslPeekServer  %s:%d  cached status %s"
	.align 8
.LC226:
	.string	"W%03d: UFDBsslPeekServer: waiting for other thread on status for %s:%d"
	.align 8
.LC227:
	.string	"W%03d: UFDBsslPeekServer: finished waiting %d intervals for other thread on %s:%d  status %d"
	.align 8
.LC228:
	.string	"W%03d: UFDBsslPeekServer: stopped waiting for other thread peeking %s:%d since reconfig=%d"
	.align 8
.LC229:
	.string	"W%03d: UFDBsslPeekServer: waited too long for other thread on status for %s:%d"
	.align 8
.LC230:
	.string	"W%03d: HTTPS protocol verification for %s:%d FAILED for peek-server: cannot open communication socket"
	.align 8
.LC231:
	.string	"W%03d: UFDBsslPeekServer: socket to %s is opened successfully. fd=%d"
	.align 8
.LC232:
	.string	"W%03d: UFDBsslPeekServer: interrupted peeking %s:%d since reconfig=%d"
	.align 8
.LC233:
	.string	"W%03d: UFDBsslPeekServer: httpsGETroot for %s:%d returned status %s, content is %sNULL, CN is \"%s\""
	.align 8
.LC234:
	.string	"W%03d: HTTPS server %s:%d did not send any content."
	.align 8
.LC235:
	.string	"W%03d: HTTPS protocol reply for %s:%d:\n   %s"
	.align 8
.LC236:
	.string	"W%03d: HTTPS protocol on %s:%d uses a hamachi/logmein TUNNEL"
	.align 8
.LC237:
	.string	"W%03d: HTTPS protocol on %s:%d uses an SSL-Explorer TUNNEL"
	.align 8
.LC238:
	.string	"W%03d: HTTPS protocol on %s:%d uses a BARRACUDA proxy TUNNEL"
	.align 8
.LC239:
	.string	"W%03d: HTTPS protocol on %s:%d uses a VNC proxy TUNNEL"
	.align 8
.LC240:
	.string	"W%03d: HTTPS protocol on %s:%d uses Webtunnel TUNNEL"
	.align 8
.LC241:
	.string	"W%03d: HTTPS protocol on %s:%d encapsulates a non-HTTP protocol %s"
	.align 8
.LC242:
	.string	"W%03d: HTTPS protocol reply for %s:%d:\n%s"
	.align 8
.LC243:
	.string	"W%03d: HTTPS protocol on %s:%d has been verified and status is %s"
	.align 8
.LC244:
	.string	"W%03d: UFDBsslPeekServer  %s:%d  status updated to %s"
	.text
	.p2align 4,,15
	.globl	UFDBsslPeekServer
	.type	UFDBsslPeekServer, @function
UFDBsslPeekServer:
.LFB85:
	.cfi_startproc
# BLOCK 2 freq:10000 seq:0
# PRED: ENTRY [100.0%]  (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	xorl	%r15d, %r15d
	pushq	%r14
	.cfi_offset 14, -32
	movq	%rcx, %r14
	pushq	%r13
	.cfi_offset 13, -40
	movq	%rdx, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movq	%r8, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$2736, %rsp
	.cfi_offset 3, -56
	movb	$0, (%rdx)
	movl	$0, (%rcx)
	movl	UFDBglobalTunnelCheckMethod(%rip), %eax
	movl	%esi, 44(%rsp)
	movl	%r9d, 40(%rsp)
	movq	$0, (%r8)
	testl	%eax, %eax
# SUCC: 16 [39.0%]  (CAN_FALLTHRU) 3 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1260
# BLOCK 3 freq:6100 seq:1
# PRED: 2 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 16 [39.0%]  (CAN_FALLTHRU) 4 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1260
# BLOCK 4 freq:3721 seq:2
# PRED: 3 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalTerminating(%rip), %edx
	movl	%eax, %r15d
	testl	%edx, %edx
# SUCC: 16 [39.0%]  (CAN_FALLTHRU) 5 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1260
# BLOCK 5 freq:2270 seq:3
# PRED: 4 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$1, UFDBglobalDebug(%rip)
	movq	%rdi, %rbx
# SUCC: 6 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 7 [47.5%]  (CAN_FALLTHRU)
	jle	.L1131
# BLOCK 6 freq:1191 seq:4
# PRED: 5 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%esi, %ecx
	movq	%rdi, %rdx
	movl	%r9d, %esi
	movl	$.LC224, %edi
# SUCC: 7 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 7 freq:2270 seq:5
# PRED: 5 [47.5%]  (CAN_FALLTHRU) 6 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1131:
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	leaq	128(%rsp), %rdi
	xorl	%eax, %eax
	movl	$2, %r15d
	movq	$0, 80(%rsp)
	call	sprintf
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	128(%rsp), %rsi
	call	UFDBsearchHashtable
	testq	%rax, %rax
	movq	%rax, 32(%rsp)
# SUCC: 10 [46.7%]  (CAN_FALLTHRU) 8 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1132
# BLOCK 8 freq:1209 seq:6
# PRED: 7 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%edi, %edi
	call	time
	movq	32(%rsp), %rdx
	movq	%rax, %r15
	movq	8(%rdx), %rcx
	movl	(%rdx), %r9d
	movq	%rdx, 80(%rsp)
	cmpq	UFDBglobalDatabaseLoadTime(%rip), %rcx
# SUCC: 9 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 49 [33.0%]  (CAN_FALLTHRU)
	jge	.L1133
# BLOCK 9 freq:889 seq:7
# PRED: 8 [67.0%]  (FALLTHRU,CAN_FALLTHRU) 84 [67.0%]  (CAN_FALLTHRU) 87 [83.9%]  (CAN_FALLTHRU)
.L1135:
# SUCC: 10 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$28, %r15d
# BLOCK 10 freq:2270 seq:8
# PRED: 7 [46.7%]  (CAN_FALLTHRU) 9 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 50 [100.0%] 
.L1132:
	cmpl	$1, UFDBglobalDebug(%rip)
	movl	%r15d, 48(%rsp)
# SUCC: 11 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 12 [47.5%]  (CAN_FALLTHRU)
	jle	.L1137
# BLOCK 11 freq:1191 seq:9
# PRED: 10 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r15d, %edi
	call	ufdbAPIstatusString
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	40(%rsp), %esi
	movq	%rax, %r8
	movl	$.LC225, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 12 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	48(%rsp), %r15d
# BLOCK 12 freq:2270 seq:10
# PRED: 10 [47.5%]  (CAN_FALLTHRU) 11 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1137:
	cmpl	$28, %r15d
# SUCC: 17 [13.7%]  (CAN_FALLTHRU) 13 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1263
# BLOCK 13 freq:1959 seq:11
# PRED: 12 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$2, %r15d
# SUCC: 51 [13.7%]  (CAN_FALLTHRU) 14 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1264
# BLOCK 14 freq:1690 seq:12
# PRED: 13 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$10, %r15d
# SUCC: 52 [13.7%]  (CAN_FALLTHRU) 15 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1265
# BLOCK 15 freq:1458 seq:13
# PRED: 14 [86.3%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$31, %r15d
# SUCC: 16 [80.1%]  (FALLTHRU,CAN_FALLTHRU) 29 [19.9%]  (CAN_FALLTHRU)
	je	.L1266
# BLOCK 16 freq:20000 seq:14
# PRED: 57 [100.0%]  2 [39.0%]  (CAN_FALLTHRU) 3 [39.0%]  (CAN_FALLTHRU) 4 [39.0%]  (CAN_FALLTHRU) 15 [80.1%]  (FALLTHRU,CAN_FALLTHRU) 35 [39.0%]  (CAN_FALLTHRU) 36 [36.7%]  (CAN_FALLTHRU) 105 [100.0%]  61 [47.5%]  (CAN_FALLTHRU) 46 [100.0%]  45 [71.0%]  (CAN_FALLTHRU) 28 [100.0%]  37 [100.0%]  62 [100.0%]  109 [100.0%] 
	.p2align 4,,10
	.p2align 3
.L1260:
	leaq	-40(%rbp), %rsp
	movl	%r15d, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
# BLOCK 17 freq:311 seq:15
# PRED: 12 [13.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1263:
	.cfi_restore_state
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	leaq	128(%rsp), %rdi
	xorl	%eax, %eax
	call	sprintf
	xorl	%edi, %edi
	call	time
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	128(%rsp), %rsi
	movq	%rax, 32(%rsp)
	call	UFDBsearchHashtable
	testq	%rax, %rax
	movq	%rax, %r15
# SUCC: 120 [4.4%]  (CAN_FALLTHRU) 18 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1267
# BLOCK 18 freq:297 seq:16
# PRED: 17 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$31, (%rax)
	movq	32(%rsp), %rax
# SUCC: 19 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rax, 8(%r15)
# BLOCK 19 freq:311 seq:17
# PRED: 18 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 120 [100.0%] 
.L1140:
	movq	16(%r15), %rdi
	movb	$0, 24(%r15)
	movb	$0, 88(%r15)
	movq	%r15, 80(%rsp)
	call	ufdbFree
	movq	80(%rsp), %rax
# SUCC: 20 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	$0, 16(%rax)
# BLOCK 20 freq:878 seq:18
# PRED: 30 [2.0%]  (CAN_FALLTHRU,LOOP_EXIT) 19 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 51 [100.0%]  54 [100.0%] 
.L1141:
	movl	44(%rsp), %esi
	movq	%rbx, %rdi
	call	UFDBopenSocket
	testl	%eax, %eax
	movl	%eax, %r15d
# SUCC: 55 [23.5%]  (CAN_FALLTHRU) 21 [76.5%]  (FALLTHRU,CAN_FALLTHRU)
	js	.L1268
# BLOCK 21 freq:672 seq:19
# PRED: 20 [76.5%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 22 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 23 [47.5%]  (CAN_FALLTHRU)
	jle	.L1162
# BLOCK 22 freq:353 seq:20
# PRED: 21 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	40(%rsp), %esi
	movl	%eax, %ecx
	movq	%rbx, %rdx
	movl	$.LC231, %edi
	xorl	%eax, %eax
# SUCC: 23 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 23 freq:672 seq:21
# PRED: 21 [47.5%]  (CAN_FALLTHRU) 22 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1162:
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 24 [83.2%]  (FALLTHRU,CAN_FALLTHRU) 63 [16.8%]  (CAN_FALLTHRU)
	je	.L1163
# BLOCK 24 freq:560 seq:22
# PRED: 23 [83.2%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebug(%rip), %r11d
	testl	%r11d, %r11d
# SUCC: 25 [39.0%]  (FALLTHRU,CAN_FALLTHRU) 26 [61.0%]  (CAN_FALLTHRU)
	je	.L1164
# BLOCK 25 freq:218 seq:23
# PRED: 24 [39.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalReconfig(%rip), %r8d
	movq	%rbx, %rdx
	movl	$.LC232, %edi
	movl	44(%rsp), %ecx
	xorl	%eax, %eax
	movl	40(%rsp), %esi
# SUCC: 26 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 26 freq:560 seq:24
# PRED: 24 [61.0%]  (CAN_FALLTHRU) 25 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1164:
	movl	%r15d, %edi
	call	close
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	leaq	128(%rsp), %rdi
	movl	$.LC19, %esi
	xorl	%eax, %eax
	call	sprintf
	xorl	%edi, %edi
	call	time
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	128(%rsp), %rsi
	movq	%rax, %rbx
	call	UFDBsearchHashtable
	testq	%rax, %rax
# SUCC: 112 [4.4%]  (CAN_FALLTHRU) 27 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1269
# BLOCK 27 freq:535 seq:25
# PRED: 26 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$28, (%rax)
# SUCC: 28 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, 8(%rax)
# BLOCK 28 freq:560 seq:26
# PRED: 27 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 112 [100.0%] 
.L1166:
	movl	$31, %r15d
# SUCC: 16 [100.0%] 
	jmp	.L1260
# BLOCK 29 freq:290 seq:27
# PRED: 15 [19.9%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1266:
	call	random
	movabsq	$3778082090400236183, %rdx
	movq	%rax, %rcx
	imulq	%rdx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$12, %rdx
	subq	%rax, %rdx
	imulq	$19999, %rdx, %rdx
	subq	%rdx, %rcx
	leal	35000(%rcx), %edi
	call	usleep
	movl	$0, 32(%rsp)
# SUCC: 30 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
# BLOCK 30 freq:3341 seq:28
# PRED: 44 [99.0%]  (DFS_BACK,CAN_FALLTHRU) 29 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1146:
	cmpl	$31, 48(%rsp)
# SUCC: 31 [98.0%]  (FALLTHRU,CAN_FALLTHRU) 20 [2.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L1141
# BLOCK 31 freq:3275 seq:29
# PRED: 30 [98.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	leaq	128(%rsp), %rdi
	xorl	%eax, %eax
	call	sprintf
	movq	myht(%rip), %rdi
	xorl	%edx, %edx
	leaq	128(%rsp), %rsi
	call	UFDBsearchHashtable
	testq	%rax, %rax
	movq	%rax, 24(%rsp)
# SUCC: 38 [46.7%]  (CAN_FALLTHRU,LOOP_EXIT) 32 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1216
# BLOCK 32 freq:1744 seq:30
# PRED: 31 [53.3%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%edi, %edi
	call	time
	movq	24(%rsp), %rdx
	movq	%rax, %rdi
	movq	8(%rdx), %rcx
	movl	(%rdx), %esi
	movq	%rdx, 80(%rsp)
	cmpq	UFDBglobalDatabaseLoadTime(%rip), %rcx
# SUCC: 33 [67.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 39 [33.0%]  (CAN_FALLTHRU)
	jge	.L1148
# BLOCK 33 freq:1283 seq:31
# PRED: 32 [67.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 58 [67.0%]  (CAN_FALLTHRU) 118 [83.9%]  (CAN_FALLTHRU)
.L1150:
# SUCC: 34 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$28, %r15d
# BLOCK 34 freq:2813 seq:32
# PRED: 33 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 38 [100.0%] 
.L1147:
# SUCC: 35 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r15d, 48(%rsp)
# BLOCK 35 freq:65 seq:33
# PRED: 34 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 60 [100.0%]  144 [100.0%] 
.L1152:
	movl	32(%rsp), %edx
	cmpl	$9, %edx
# SUCC: 36 [61.0%]  (FALLTHRU,CAN_FALLTHRU) 16 [39.0%]  (CAN_FALLTHRU)
	jle	.L1260
# BLOCK 36 freq:40 seq:34
# PRED: 35 [61.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 37 [63.3%]  (FALLTHRU,CAN_FALLTHRU) 16 [36.7%]  (CAN_FALLTHRU)
	jle	.L1260
# BLOCK 37 freq:25 seq:35
# PRED: 36 [63.3%]  (FALLTHRU,CAN_FALLTHRU)
	movl	44(%rsp), %r8d
	movl	%r15d, %r9d
	movq	%rbx, %rcx
	movl	40(%rsp), %esi
	movl	$.LC227, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	48(%rsp), %r15d
# SUCC: 16 [100.0%] 
	jmp	.L1260
# BLOCK 38 freq:1530 seq:36
# PRED: 31 [46.7%]  (CAN_FALLTHRU,LOOP_EXIT)
	.p2align 4,,10
	.p2align 3
.L1216:
	movl	$2, %r15d
# SUCC: 34 [100.0%] 
	jmp	.L1147
# BLOCK 39 freq:576 seq:37
# PRED: 32 [33.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1148:
	cmpl	$8, %esi
# SUCC: 58 [28.0%]  (CAN_FALLTHRU,LOOP_EXIT) 40 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1270
# BLOCK 40 freq:414 seq:38
# PRED: 39 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$31, %esi
	movq	%rax, 8(%rdx)
	movl	%esi, 48(%rsp)
# SUCC: 41 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 144 (CAN_FALLTHRU,LOOP_EXIT)
	jne	.L1217
# BLOCK 41 freq:3209 seq:39
# PRED: 40 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	random
	movq	%rax, %rdi
	movabsq	$5440318517184312447, %rax
	imulq	%rdi
	movq	%rdi, %rax
	sarq	$63, %rax
	sarq	$13, %rdx
	subq	%rax, %rdx
	imulq	$27777, %rdx, %rdx
	subq	%rdx, %rdi
	addl	$80000, %edi
	call	usleep
	addl	$1, 32(%rsp)
	cmpl	$10, 32(%rsp)
# SUCC: 47 [28.0%]  (CAN_FALLTHRU) 42 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1271
# BLOCK 42 freq:3209 seq:40
# PRED: 41 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 47 [47.5%]  (CAN_FALLTHRU) 48 [100.0%] 
.L1153:
	movl	UFDBglobalReconfig(%rip), %eax
	testl	%eax, %eax
# SUCC: 61 [2.0%]  (CAN_FALLTHRU,LOOP_EXIT) 43 [98.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1155
# BLOCK 43 freq:3145 seq:41
# PRED: 42 [98.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalTerminating(%rip), %eax
	testl	%eax, %eax
# SUCC: 61 [2.0%]  (CAN_FALLTHRU,LOOP_EXIT) 44 [98.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1155
# BLOCK 44 freq:3082 seq:42
# PRED: 43 [98.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$200, 32(%rsp)
# SUCC: 45 [1.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT) 30 [99.0%]  (DFS_BACK,CAN_FALLTHRU)
	jne	.L1146
# BLOCK 45 freq:31 seq:43
# PRED: 44 [1.0%]  (FALLTHRU,CAN_FALLTHRU,LOOP_EXIT)
	movl	UFDBglobalDebug(%rip), %r12d
	testl	%r12d, %r12d
# SUCC: 46 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 16 [71.0%]  (CAN_FALLTHRU)
	je	.L1260
# BLOCK 46 freq:9 seq:44
# PRED: 45 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC229, %edi
	movl	40(%rsp), %esi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 16 [100.0%] 
	jmp	.L1260
# BLOCK 47 freq:899 seq:45
# PRED: 41 [28.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1271:
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 48 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 42 [47.5%]  (CAN_FALLTHRU)
	jle	.L1153
# BLOCK 48 freq:472 seq:46
# PRED: 47 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC226, %edi
	movl	40(%rsp), %esi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 42 [100.0%] 
	jmp	.L1153
# BLOCK 49 freq:399 seq:47
# PRED: 8 [33.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1133:
	cmpl	$8, %r9d
# SUCC: 84 [28.0%]  (CAN_FALLTHRU) 50 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1134
# BLOCK 50 freq:320 seq:48
# PRED: 49 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 85 [71.0%]  (CAN_FALLTHRU) 86 [50.0%]  (CAN_FALLTHRU) 88 [100.0%] 
.L1136:
	movq	%r15, 8(%rdx)
	movl	%r9d, %r15d
# SUCC: 10 [100.0%] 
	jmp	.L1132
# BLOCK 51 freq:269 seq:49
# PRED: 13 [13.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1264:
	movl	44(%rsp), %esi
	leaq	80(%rsp), %rdx
	movq	%rbx, %rdi
	call	insertHTTPScache.constprop.5
# SUCC: 20 [100.0%] 
	jmp	.L1141
# BLOCK 52 freq:232 seq:50
# PRED: 14 [13.7%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1265:
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	leaq	128(%rsp), %rdi
	xorl	%eax, %eax
	call	sprintf
	xorl	%edi, %edi
	call	time
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	128(%rsp), %rsi
	movq	%rax, 32(%rsp)
	call	UFDBsearchHashtable
	testq	%rax, %rax
	movq	%rax, %r15
# SUCC: 133 [4.4%]  (CAN_FALLTHRU) 53 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1272
# BLOCK 53 freq:221 seq:51
# PRED: 52 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$31, (%rax)
	movq	32(%rsp), %rax
# SUCC: 54 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rax, 8(%r15)
# BLOCK 54 freq:232 seq:52
# PRED: 53 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 133 [100.0%] 
.L1145:
	movq	%r15, 80(%rsp)
# SUCC: 20 [100.0%] 
	jmp	.L1141
# BLOCK 55 freq:206 seq:53
# PRED: 20 [23.5%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1268:
	movl	44(%rsp), %r14d
	movq	%rbx, %rdx
	xorl	%eax, %eax
	movl	40(%rsp), %esi
	movl	$.LC230, %edi
	movl	%r14d, %ecx
	call	ufdbLogMessage
	movq	%rbx, %rdx
	movl	%r14d, %ecx
	leaq	128(%rsp), %rdi
	movl	$.LC19, %esi
	xorl	%eax, %eax
	call	sprintf
	xorl	%edi, %edi
	call	time
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	128(%rsp), %rsi
	movq	%rax, %rbx
	call	UFDBsearchHashtable
	testq	%rax, %rax
# SUCC: 134 [4.4%]  (CAN_FALLTHRU) 56 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1273
# BLOCK 56 freq:197 seq:54
# PRED: 55 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$8, (%rax)
# SUCC: 57 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, 8(%rax)
# BLOCK 57 freq:206 seq:55
# PRED: 56 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 134 [100.0%] 
.L1161:
	movl	$8, %r15d
# SUCC: 16 [100.0%] 
	jmp	.L1260
# BLOCK 58 freq:161 seq:56
# PRED: 39 [28.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L1270:
	leaq	-1800(%rax), %rax
	cmpq	%rcx, %rax
# SUCC: 33 [67.0%]  (CAN_FALLTHRU) 59 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L1150
# BLOCK 59 freq:53 seq:57
# PRED: 58 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	-600(%rdi), %rax
	cmpq	%rcx, %rax
# SUCC: 117 [29.0%]  (CAN_FALLTHRU) 60 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jg	.L1274
# BLOCK 60 freq:47 seq:58
# PRED: 59 [71.0%]  (FALLTHRU,CAN_FALLTHRU) 117 [50.0%]  (CAN_FALLTHRU) 119 [100.0%] 
.L1151:
	movq	%rdi, 8(%rdx)
	movl	$8, %r15d
	movl	$8, 48(%rsp)
# SUCC: 35 [100.0%] 
	jmp	.L1152
# BLOCK 61 freq:127 seq:59
# PRED: 43 [2.0%]  (CAN_FALLTHRU,LOOP_EXIT) 42 [2.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L1155:
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 62 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 16 [47.5%]  (CAN_FALLTHRU)
	jle	.L1260
# BLOCK 62 freq:67 seq:60
# PRED: 61 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalReconfig(%rip), %r8d
	movq	%rbx, %rdx
	movl	$.LC228, %edi
	movl	44(%rsp), %ecx
	xorl	%eax, %eax
	movl	40(%rsp), %esi
	call	ufdbLogMessage
# SUCC: 16 [100.0%] 
	jmp	.L1260
# BLOCK 63 freq:113 seq:61
# PRED: 23 [16.8%]  (CAN_FALLTHRU)
.L1163:
	movl	$16, %r8d
	movl	$20, %edx
	movl	$1, %esi
	leaq	112(%rsp), %rcx
	movl	%r15d, %edi
	movq	$0, (%r12)
	movl	$0, 48(%rsp)
	movq	$3, 112(%rsp)
	movq	$500000, 120(%rsp)
	call	setsockopt
	movl	$16, %r8d
	movl	$21, %edx
	leaq	112(%rsp), %rcx
	movl	$1, %esi
	movl	%r15d, %edi
	call	setsockopt
	movl	44(%rsp), %edx
	leaq	48(%rsp), %r9
	movq	%r13, %r8
	movq	%r14, %rcx
	movq	%rbx, %rsi
	movl	%r15d, %edi
	movl	$0, 48(%rsp)
	call	httpsGETroot.constprop.7
	movl	UFDBglobalDebug(%rip), %r10d
	movq	%rax, (%r12)
	testl	%r10d, %r10d
# SUCC: 70 [50.0%]  (CAN_FALLTHRU) 64 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1167
# BLOCK 64 freq:56 seq:62
# PRED: 63 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugAim(%rip), %r9d
	testl	%r9d, %r9d
# SUCC: 70 [50.0%]  (CAN_FALLTHRU) 65 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1167
# BLOCK 65 freq:28 seq:63
# PRED: 64 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugGtalk(%rip), %r8d
	testl	%r8d, %r8d
# SUCC: 70 [50.0%]  (CAN_FALLTHRU) 66 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1167
# BLOCK 66 freq:14 seq:64
# PRED: 65 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugSkype(%rip), %edi
	testl	%edi, %edi
# SUCC: 70 [50.0%]  (CAN_FALLTHRU) 67 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1167
# BLOCK 67 freq:7 seq:65
# PRED: 66 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugYahooMsg(%rip)
# SUCC: 70 [50.0%]  (CAN_FALLTHRU) 68 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1167
# BLOCK 68 freq:4 seq:66
# PRED: 67 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugFBchat(%rip)
# SUCC: 70 [50.0%]  (CAN_FALLTHRU) 69 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1167
# BLOCK 69 freq:2 seq:67
# PRED: 68 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugCitrixOnline(%rip)
# SUCC: 70 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 71 [50.0%]  (CAN_FALLTHRU)
	je	.L1168
# BLOCK 70 freq:112 seq:68
# PRED: 64 [50.0%]  (CAN_FALLTHRU) 63 [50.0%]  (CAN_FALLTHRU) 69 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 68 [50.0%]  (CAN_FALLTHRU) 67 [50.0%]  (CAN_FALLTHRU) 66 [50.0%]  (CAN_FALLTHRU) 65 [50.0%]  (CAN_FALLTHRU)
.L1167:
	movl	48(%rsp), %edi
	testq	%rax, %rax
	movl	$.LC16, %r14d
	movl	$.LC15, %eax
	cmovne	%rax, %r14
	call	ufdbAPIstatusString
	movl	44(%rsp), %ecx
	movq	%r13, (%rsp)
	movl	40(%rsp), %esi
	movq	%rax, %r8
	movq	%r14, %r9
	movq	%rbx, %rdx
	movl	$.LC233, %edi
	xorl	%eax, %eax
# SUCC: 71 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 71 freq:113 seq:69
# PRED: 69 [50.0%]  (CAN_FALLTHRU) 70 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1168:
	movq	(%r12), %rdx
	movl	48(%rsp), %eax
	testq	%rdx, %rdx
# SUCC: 106 [15.0%]  (CAN_FALLTHRU) 72 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1170
# BLOCK 72 freq:96 seq:70
# PRED: 71 [85.0%]  (FALLTHRU,CAN_FALLTHRU)
	movzbl	(%rdx), %r13d
	testb	%r13b, %r13b
# SUCC: 106 [50.0%]  (CAN_FALLTHRU) 73 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1170
# BLOCK 73 freq:48 seq:71
# PRED: 72 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	testl	%eax, %eax
# SUCC: 89 [50.0%]  (CAN_FALLTHRU) 74 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1172
# BLOCK 74 freq:24 seq:72
# PRED: 73 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC28, %edi
	movl	$5, %ecx
	movq	%rdx, %rsi
	repz cmpsb
# SUCC: 75 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 121 [50.0%]  (CAN_FALLTHRU)
	jne	.L1275
# BLOCK 75 seq:73
# PRED: 74 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 121 [50.0%]  (CAN_FALLTHRU)
.L1178:
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 76 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 115 [27.0%]  (CAN_FALLTHRU)
	jle	.L1211
# BLOCK 76 freq:12 seq:74
# PRED: 75 [73.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$1, %ecx
	xorl	%r14d, %r14d
# SUCC: 80 [100.0%] 
	jmp	.L1185
# BLOCK 77 freq:114 seq:75
# PRED: 81 [50.0%]  (CAN_FALLTHRU) 80 [50.0%]  (CAN_FALLTHRU)
.L1222:
	movb	$95, 128(%rsp,%r14)
# SUCC: 78 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movslq	%ecx, %rax
# BLOCK 78 freq:227 seq:76
# PRED: 83 [100.0%]  77 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 110 [100.0%] 
.L1182:
	movzbl	1(%rdx,%r14), %r13d
	testb	%r13b, %r13b
# SUCC: 79 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 114 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L1184
# BLOCK 79 freq:217 seq:77
# PRED: 78 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	addq	$1, %r14
	addl	$1, %ecx
	cmpq	$2599, %r14
# SUCC: 80 [99.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 114 [1.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L1184
# BLOCK 80 freq:227 seq:78
# PRED: 79 [99.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 76 [100.0%] 
.L1185:
	cmpb	$10, %r13b
# SUCC: 77 [50.0%]  (CAN_FALLTHRU) 81 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1222
# BLOCK 81 freq:114 seq:79
# PRED: 80 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpb	$13, %r13b
# SUCC: 82 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 77 [50.0%]  (CAN_FALLTHRU)
	je	.L1222
# BLOCK 82 freq:114 seq:80
# PRED: 81 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdx, 24(%rsp)
	movl	%ecx, 32(%rsp)
	call	__ctype_b_loc
	movsbq	%r13b, %rsi
	movl	32(%rsp), %ecx
	movq	(%rax), %rax
	movq	24(%rsp), %rdx
	testb	$64, 1(%rax,%rsi,2)
	movslq	%ecx, %rax
# SUCC: 83 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 110 [50.0%]  (CAN_FALLTHRU)
	je	.L1183
# BLOCK 83 freq:57 seq:81
# PRED: 82 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movb	%r13b, 128(%rsp,%r14)
# SUCC: 78 [100.0%] 
	jmp	.L1182
# BLOCK 84 freq:112 seq:82
# PRED: 49 [28.0%]  (CAN_FALLTHRU)
.L1134:
	leaq	-1800(%rax), %rax
	cmpq	%rax, %rcx
# SUCC: 9 [67.0%]  (CAN_FALLTHRU) 85 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	jl	.L1135
# BLOCK 85 freq:37 seq:83
# PRED: 84 [33.0%]  (FALLTHRU,CAN_FALLTHRU)
	leaq	-600(%r15), %rax
	cmpq	%rax, %rcx
# SUCC: 86 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 50 [71.0%]  (CAN_FALLTHRU)
	jge	.L1136
# BLOCK 86 freq:11 seq:84
# PRED: 85 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r9d, 24(%rsp)
	movq	%rdx, 32(%rsp)
	call	UFDBhttpsVerificationQueued
	movq	32(%rsp), %rdx
	cmpl	$31, %eax
	movl	24(%rsp), %r9d
# SUCC: 87 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 50 [50.0%]  (CAN_FALLTHRU)
	jg	.L1136
# BLOCK 87 freq:5 seq:85
# PRED: 86 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$2, UFDBglobalTunnelCheckMethod(%rip)
# SUCC: 9 [83.9%]  (CAN_FALLTHRU) 88 [16.1%]  (FALLTHRU)
	jne	.L1135
# BLOCK 88 freq:1 seq:86
# PRED: 87 [16.1%]  (FALLTHRU)
# SUCC: 50 [100.0%] 
	jmp	.L1136
# BLOCK 89 freq:70 seq:87
# PRED: 106 [71.0%]  (CAN_FALLTHRU) 73 [50.0%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1172:
	cmpl	$2, %eax
# SUCC: 113 [28.0%]  (CAN_FALLTHRU) 90 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1276
# BLOCK 90 freq:94 seq:88
# PRED: 116 [100.0%]  148 [100.0%]  137 [100.0%]  152 [100.0%]  113 [100.0%]  89 [72.0%]  (FALLTHRU,CAN_FALLTHRU) 140 [100.0%]  143 [100.0%]  156 [100.0%] 
.L1175:
	movl	%r15d, %edi
	.p2align 4,,5
	call	close
	movl	UFDBglobalDebug(%rip), %esi
	testl	%esi, %esi
# SUCC: 98 [29.0%]  (CAN_FALLTHRU) 91 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1203
# BLOCK 91 freq:67 seq:89
# PRED: 90 [71.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalLogAllRequests(%rip), %ecx
	testl	%ecx, %ecx
# SUCC: 98 [50.0%]  (CAN_FALLTHRU) 92 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1203
# BLOCK 92 freq:33 seq:90
# PRED: 91 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugAim(%rip), %edx
	testl	%edx, %edx
# SUCC: 98 [50.0%]  (CAN_FALLTHRU) 93 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1203
# BLOCK 93 freq:17 seq:91
# PRED: 92 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	UFDBglobalDebugGtalk(%rip), %eax
	testl	%eax, %eax
# SUCC: 98 [50.0%]  (CAN_FALLTHRU) 94 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1203
# BLOCK 94 freq:8 seq:92
# PRED: 93 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugSkype(%rip)
# SUCC: 98 [50.0%]  (CAN_FALLTHRU) 95 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1203
# BLOCK 95 freq:4 seq:93
# PRED: 94 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugYahooMsg(%rip)
# SUCC: 98 [50.0%]  (CAN_FALLTHRU) 96 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1203
# BLOCK 96 freq:2 seq:94
# PRED: 95 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugFBchat(%rip)
# SUCC: 98 [50.0%]  (CAN_FALLTHRU) 97 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1203
# BLOCK 97 freq:1 seq:95
# PRED: 96 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalDebugCitrixOnline(%rip)
# SUCC: 98 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 99 [50.0%]  (CAN_FALLTHRU)
	je	.L1204
# BLOCK 98 freq:93 seq:96
# PRED: 91 [50.0%]  (CAN_FALLTHRU) 90 [29.0%]  (CAN_FALLTHRU) 97 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 96 [50.0%]  (CAN_FALLTHRU) 95 [50.0%]  (CAN_FALLTHRU) 94 [50.0%]  (CAN_FALLTHRU) 93 [50.0%]  (CAN_FALLTHRU) 92 [50.0%]  (CAN_FALLTHRU)
.L1203:
	movl	48(%rsp), %edi
	call	ufdbAPIstatusString
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	40(%rsp), %esi
	movq	%rax, %r8
	movl	$.LC243, %edi
	xorl	%eax, %eax
# SUCC: 99 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 99 freq:94 seq:97
# PRED: 97 [50.0%]  (CAN_FALLTHRU) 98 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1204:
	cmpl	$3, UFDBglobalTunnelCheckMethod(%rip)
# SUCC: 111 [28.0%]  (CAN_FALLTHRU) 100 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1205
# BLOCK 100 freq:68 seq:98
# PRED: 99 [72.0%]  (FALLTHRU,CAN_FALLTHRU)
# SUCC: 101 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	48(%rsp), %r12d
# BLOCK 101 freq:94 seq:99
# PRED: 100 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 111 [100.0%] 
.L1206:
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 102 [52.5%]  (FALLTHRU,CAN_FALLTHRU) 103 [47.5%]  (CAN_FALLTHRU)
	jle	.L1207
# BLOCK 102 freq:49 seq:100
# PRED: 101 [52.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r12d, %edi
	call	ufdbAPIstatusString
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	40(%rsp), %esi
	movq	%rax, %r8
	movl	$.LC244, %edi
	xorl	%eax, %eax
	call	ufdbLogMessage
# SUCC: 103 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	48(%rsp), %r12d
# BLOCK 103 freq:94 seq:101
# PRED: 101 [47.5%]  (CAN_FALLTHRU) 102 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1207:
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC19, %esi
	leaq	128(%rsp), %rdi
	xorl	%eax, %eax
	call	sprintf
	xorl	%edi, %edi
	call	time
	movq	myht(%rip), %rdi
	movl	$1, %edx
	leaq	128(%rsp), %rsi
	movq	%rax, %rbx
	call	UFDBsearchHashtable
	testq	%rax, %rax
# SUCC: 141 [4.4%]  (CAN_FALLTHRU) 104 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1277
# BLOCK 104 freq:90 seq:102
# PRED: 103 [95.6%]  (FALLTHRU,CAN_FALLTHRU)
	movl	%r12d, (%rax)
# SUCC: 105 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rbx, 8(%rax)
# BLOCK 105 freq:94 seq:103
# PRED: 104 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 141 [100.0%] 
.L1209:
	movl	48(%rsp), %r15d
# SUCC: 16 [100.0%] 
	jmp	.L1260
# BLOCK 106 freq:65 seq:104
# PRED: 72 [50.0%]  (CAN_FALLTHRU) 71 [15.0%]  (CAN_FALLTHRU)
.L1170:
	testl	%eax, %eax
# SUCC: 107 [29.0%]  (FALLTHRU,CAN_FALLTHRU) 89 [71.0%]  (CAN_FALLTHRU)
	jne	.L1172
# BLOCK 107 freq:19 seq:105
# PRED: 106 [29.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC234, %edi
	movl	40(%rsp), %esi
	call	ufdbLogMessage
	movl	%r15d, %edi
	call	close
	movq	(%r12), %rdi
	testq	%rdi, %rdi
# SUCC: 108 [78.3%]  (FALLTHRU,CAN_FALLTHRU) 109 [21.6%]  (CAN_FALLTHRU)
	je	.L1173
# BLOCK 108 freq:15 seq:106
# PRED: 107 [78.3%]  (FALLTHRU,CAN_FALLTHRU)
# SUCC: 109 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbFree
# BLOCK 109 freq:19 seq:107
# PRED: 107 [21.6%]  (CAN_FALLTHRU) 108 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1173:
	movq	$0, (%r12)
	xorl	%r15d, %r15d
# SUCC: 16 [100.0%] 
	jmp	.L1260
# BLOCK 110 freq:57 seq:108
# PRED: 82 [50.0%]  (CAN_FALLTHRU)
.L1183:
	movb	$46, 128(%rsp,%r14)
# SUCC: 78 [100.0%] 
	jmp	.L1182
# BLOCK 111 freq:26 seq:109
# PRED: 99 [28.0%]  (CAN_FALLTHRU)
.L1205:
	movl	$0, 48(%rsp)
	xorl	%r12d, %r12d
# SUCC: 101 [100.0%] 
	jmp	.L1206
# BLOCK 112 freq:25 seq:110
# PRED: 26 [4.4%]  (CAN_FALLTHRU)
.L1269:
	movl	$1112, %edi
	call	ufdbMalloc
	leaq	128(%rsp), %rdi
	movq	%rbx, 8(%rax)
	movq	%rax, %r12
	movl	$28, (%rax)
	movq	$0, 16(%rax)
	movb	$0, 24(%rax)
	movb	$0, 88(%rax)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%r12, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
# SUCC: 28 [100.0%] 
	jmp	.L1166
# BLOCK 113 freq:20 seq:111
# PRED: 89 [28.0%]  (CAN_FALLTHRU)
.L1276:
	movl	$8, 48(%rsp)
# SUCC: 90 [100.0%] 
	jmp	.L1175
# BLOCK 114 freq:13 seq:112
# PRED: 78 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 79 [1.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L1184:
	movl	44(%rsp), %ecx
	movb	$0, 128(%rsp,%rax)
	movq	%rbx, %rdx
	movl	40(%rsp), %esi
	leaq	128(%rsp), %r8
	xorl	%eax, %eax
	movl	$.LC235, %edi
	call	ufdbLogMessage
# SUCC: 115 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	(%r12), %rdx
# BLOCK 115 freq:18 seq:113
# PRED: 114 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 75 [27.0%]  (CAN_FALLTHRU)
.L1211:
	movq	%rdx, %rdi
	movl	$.LC200, %esi
	movq	%rdx, 32(%rsp)
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 116 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 138 [30.2%]  (CAN_FALLTHRU)
	je	.L1278
# BLOCK 116 freq:17 seq:114
# PRED: 138 [85.0%]  (CAN_FALLTHRU) 115 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
.L1186:
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC236, %edi
	movl	40(%rsp), %esi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$11, 48(%rsp)
# SUCC: 90 [100.0%] 
	jmp	.L1175
# BLOCK 117 freq:15 seq:115
# PRED: 59 [29.0%]  (CAN_FALLTHRU)
.L1274:
	movq	%rdi, 16(%rsp)
	movq	%rdx, 24(%rsp)
	call	UFDBhttpsVerificationQueued
	movq	24(%rsp), %rdx
	cmpl	$31, %eax
	movq	16(%rsp), %rdi
# SUCC: 118 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 60 [50.0%]  (CAN_FALLTHRU)
	jg	.L1151
# BLOCK 118 freq:8 seq:116
# PRED: 117 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$2, UFDBglobalTunnelCheckMethod(%rip)
# SUCC: 33 [83.9%]  (CAN_FALLTHRU) 119 [16.1%]  (FALLTHRU)
	jne	.L1150
# BLOCK 119 freq:1 seq:117
# PRED: 118 [16.1%]  (FALLTHRU)
# SUCC: 60 [100.0%] 
	jmp	.L1151
# BLOCK 120 freq:14 seq:118
# PRED: 17 [4.4%]  (CAN_FALLTHRU)
	.p2align 4,,10
	.p2align 3
.L1267:
	movl	$1112, %edi
	call	ufdbMalloc
	leaq	128(%rsp), %rdi
	movq	%rax, %r15
	movq	32(%rsp), %rax
	movl	$31, (%r15)
	movq	$0, 16(%r15)
	movb	$0, 24(%r15)
	movq	%rax, 8(%r15)
	movb	$0, 88(%r15)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%r15, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
# SUCC: 19 [100.0%] 
	jmp	.L1140
# BLOCK 121 freq:12 seq:119
# PRED: 74 [50.0%]  (CAN_FALLTHRU)
.L1275:
	movl	$.LC30, %edi
	movl	$5, %ecx
	movq	%rdx, %rsi
	repz cmpsb
# SUCC: 75 [50.0%]  (CAN_FALLTHRU) 122 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1178
# BLOCK 122 freq:6 seq:120
# PRED: 121 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpl	$0, UFDBglobalUnknownProtocolOverHttps(%rip)
	movl	$.LC189, %eax
	movq	%rbx, %rdx
	movl	44(%rsp), %ecx
	movl	$.LC190, %r8d
	movl	$.LC241, %edi
	movl	40(%rsp), %esi
	cmovne	%rax, %r8
	xorl	%eax, %eax
	call	ufdbLogMessage
	cmpl	$1, UFDBglobalDebug(%rip)
# SUCC: 123 [73.0%]  (FALLTHRU,CAN_FALLTHRU) 137 [27.0%]  (CAN_FALLTHRU)
	jle	.L1195
# BLOCK 123 freq:4 seq:121
# PRED: 122 [73.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	(%r12), %rdx
	movzbl	(%rdx), %r13d
	testb	%r13b, %r13b
# SUCC: 124 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 135 [4.5%]  (CAN_FALLTHRU)
	je	.L1220
# BLOCK 124 freq:4 seq:122
# PRED: 123 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$1, %r14d
	xorl	%r12d, %r12d
# SUCC: 128 [100.0%] 
	jmp	.L1201
# BLOCK 125 freq:38 seq:123
# PRED: 129 [50.0%]  (CAN_FALLTHRU) 128 [50.0%]  (CAN_FALLTHRU)
.L1223:
	movb	$95, 128(%rsp,%r12)
# SUCC: 126 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	movslq	%r14d, %rax
# BLOCK 126 freq:76 seq:124
# PRED: 131 [100.0%]  125 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 132 [100.0%] 
.L1199:
	movzbl	1(%rdx,%r12), %r13d
	testb	%r13b, %r13b
# SUCC: 127 [95.5%]  (FALLTHRU,CAN_FALLTHRU) 136 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L1196
# BLOCK 127 freq:72 seq:125
# PRED: 126 [95.5%]  (FALLTHRU,CAN_FALLTHRU)
	addq	$1, %r12
	addl	$1, %r14d
	cmpq	$2399, %r12
# SUCC: 128 [99.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 136 [1.0%]  (CAN_FALLTHRU,LOOP_EXIT)
	je	.L1196
# BLOCK 128 freq:76 seq:126
# PRED: 127 [99.0%]  (FALLTHRU,DFS_BACK,CAN_FALLTHRU) 124 [100.0%] 
.L1201:
	cmpb	$10, %r13b
# SUCC: 125 [50.0%]  (CAN_FALLTHRU) 129 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	je	.L1223
# BLOCK 129 freq:38 seq:127
# PRED: 128 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	cmpb	$13, %r13b
# SUCC: 130 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 125 [50.0%]  (CAN_FALLTHRU)
	je	.L1223
# BLOCK 130 freq:38 seq:128
# PRED: 129 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdx, 32(%rsp)
	call	__ctype_b_loc
	movsbq	%r13b, %rcx
	movq	32(%rsp), %rdx
	movq	(%rax), %rax
	testb	$64, 1(%rax,%rcx,2)
	movslq	%r14d, %rax
# SUCC: 131 [50.0%]  (FALLTHRU,CAN_FALLTHRU) 132 [50.0%]  (CAN_FALLTHRU)
	je	.L1200
# BLOCK 131 freq:19 seq:129
# PRED: 130 [50.0%]  (FALLTHRU,CAN_FALLTHRU)
	movb	%r13b, 128(%rsp,%r12)
# SUCC: 126 [100.0%] 
	jmp	.L1199
# BLOCK 132 freq:19 seq:130
# PRED: 130 [50.0%]  (CAN_FALLTHRU)
.L1200:
	movb	$46, 128(%rsp,%r12)
# SUCC: 126 [100.0%] 
	jmp	.L1199
# BLOCK 133 freq:10 seq:131
# PRED: 52 [4.4%]  (CAN_FALLTHRU)
.L1272:
	movl	$1112, %edi
	call	ufdbMalloc
	leaq	128(%rsp), %rdi
	movq	%rax, %r15
	movq	32(%rsp), %rax
	movl	$31, (%r15)
	movq	$0, 16(%r15)
	movb	$0, 24(%r15)
	movq	%rax, 8(%r15)
	movb	$0, 88(%r15)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%r15, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
# SUCC: 54 [100.0%] 
	jmp	.L1145
# BLOCK 134 freq:9 seq:132
# PRED: 55 [4.4%]  (CAN_FALLTHRU)
.L1273:
	movl	$1112, %edi
	call	ufdbMalloc
	leaq	128(%rsp), %rdi
	movq	%rbx, 8(%rax)
	movq	%rax, %r12
	movl	$8, (%rax)
	movq	$0, 16(%rax)
	movb	$0, 24(%rax)
	movb	$0, 88(%rax)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%r12, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
# SUCC: 57 [100.0%] 
	jmp	.L1161
# BLOCK 135 seq:133
# PRED: 123 [4.5%]  (CAN_FALLTHRU)
.L1220:
# SUCC: 136 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	xorl	%eax, %eax
# BLOCK 136 freq:4 seq:134
# PRED: 126 [4.5%]  (CAN_FALLTHRU,LOOP_EXIT) 135 [100.0%]  (FALLTHRU,CAN_FALLTHRU) 127 [1.0%]  (CAN_FALLTHRU,LOOP_EXIT)
.L1196:
	movl	44(%rsp), %ecx
	movb	$0, 128(%rsp,%rax)
	movq	%rbx, %rdx
	movl	40(%rsp), %esi
	leaq	128(%rsp), %r8
	xorl	%eax, %eax
	movl	$.LC242, %edi
# SUCC: 137 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
	call	ufdbLogMessage
# BLOCK 137 freq:6 seq:135
# PRED: 122 [27.0%]  (CAN_FALLTHRU) 136 [100.0%]  (FALLTHRU,CAN_FALLTHRU)
.L1195:
	cmpl	$1, UFDBglobalUnknownProtocolOverHttps(%rip)
	sbbl	%eax, %eax
	andl	$-8, %eax
	addl	$19, %eax
	movl	%eax, 48(%rsp)
# SUCC: 90 [100.0%] 
	jmp	.L1175
# BLOCK 138 freq:5 seq:136
# PRED: 115 [30.2%]  (CAN_FALLTHRU)
.L1278:
	movq	%rdx, %rdi
	movl	$.LC201, %esi
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 116 [85.0%]  (CAN_FALLTHRU) 139 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1186
# BLOCK 139 freq:1 seq:137
# PRED: 138 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdx, %rdi
	movl	$.LC203, %esi
	movq	%rdx, 32(%rsp)
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 140 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 142 [30.2%]  (CAN_FALLTHRU)
	je	.L1188
# BLOCK 140 freq:1 seq:138
# PRED: 139 [69.8%]  (FALLTHRU,CAN_FALLTHRU)
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC237, %edi
	movl	40(%rsp), %esi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$11, 48(%rsp)
# SUCC: 90 [100.0%] 
	jmp	.L1175
# BLOCK 141 freq:4 seq:139
# PRED: 103 [4.4%]  (CAN_FALLTHRU)
.L1277:
	movl	$1112, %edi
	call	ufdbMalloc
	leaq	128(%rsp), %rdi
	movq	%rbx, 8(%rax)
	movq	%rax, %r13
	movl	%r12d, (%rax)
	movq	$0, 16(%rax)
	movb	$0, 24(%rax)
	movb	$0, 88(%rax)
	call	ufdbStrdup
	movq	myht(%rip), %rdi
	movq	%r13, %rdx
	movl	$1, %ecx
	movq	%rax, %rsi
	call	UFDBinsertHashtable
# SUCC: 105 [100.0%] 
	jmp	.L1209
# BLOCK 142 seq:140
# PRED: 139 [30.2%]  (CAN_FALLTHRU)
.L1188:
	movq	%rdx, %rdi
	movl	$.LC205, %esi
	movq	%rdx, 32(%rsp)
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 143 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 145 [30.2%]  (CAN_FALLTHRU)
	je	.L1279
# BLOCK 143 seq:141
# PRED: 145 [85.0%]  (CAN_FALLTHRU) 142 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 146 [85.0%]  (CAN_FALLTHRU)
.L1189:
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC238, %edi
	movl	40(%rsp), %esi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$11, 48(%rsp)
# SUCC: 90 [100.0%] 
	jmp	.L1175
# BLOCK 144 seq:142
# PRED: 40 (CAN_FALLTHRU,LOOP_EXIT)
.L1217:
	movl	%esi, %r15d
# SUCC: 35 [100.0%] 
	jmp	.L1152
# BLOCK 145 seq:143
# PRED: 142 [30.2%]  (CAN_FALLTHRU)
.L1279:
	movq	%rdx, %rdi
	movl	$.LC206, %esi
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 143 [85.0%]  (CAN_FALLTHRU) 146 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1189
# BLOCK 146 seq:144
# PRED: 145 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdx, %rdi
	movl	$.LC207, %esi
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 143 [85.0%]  (CAN_FALLTHRU) 147 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1189
# BLOCK 147 seq:145
# PRED: 146 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdx, %rdi
	movl	$.LC209, %esi
	movq	%rdx, 32(%rsp)
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 148 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 149 [30.2%]  (CAN_FALLTHRU)
	je	.L1280
# BLOCK 148 seq:146
# PRED: 149 [85.0%]  (CAN_FALLTHRU) 147 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 150 [85.0%]  (CAN_FALLTHRU)
.L1191:
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC239, %edi
	movl	40(%rsp), %esi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$11, 48(%rsp)
# SUCC: 90 [100.0%] 
	jmp	.L1175
# BLOCK 149 seq:147
# PRED: 147 [30.2%]  (CAN_FALLTHRU)
.L1280:
	movq	%rdx, %rdi
	movl	$.LC210, %esi
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 148 [85.0%]  (CAN_FALLTHRU) 150 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1191
# BLOCK 150 seq:148
# PRED: 149 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdx, %rdi
	movl	$.LC211, %esi
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 148 [85.0%]  (CAN_FALLTHRU) 151 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1191
# BLOCK 151 seq:149
# PRED: 150 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdx, %rdi
	movl	$.LC213, %esi
	movq	%rdx, 32(%rsp)
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 152 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 153 [30.2%]  (CAN_FALLTHRU)
	je	.L1281
# BLOCK 152 seq:150
# PRED: 153 [85.0%]  (CAN_FALLTHRU) 151 [69.8%]  (FALLTHRU,CAN_FALLTHRU) 155 [85.0%]  (CAN_FALLTHRU) 154 [85.0%]  (CAN_FALLTHRU)
.L1193:
	movl	44(%rsp), %ecx
	movq	%rbx, %rdx
	movl	$.LC240, %edi
	movl	40(%rsp), %esi
	xorl	%eax, %eax
	call	ufdbLogMessage
	movl	$11, 48(%rsp)
# SUCC: 90 [100.0%] 
	jmp	.L1175
# BLOCK 153 seq:151
# PRED: 151 [30.2%]  (CAN_FALLTHRU)
.L1281:
	movq	%rdx, %rdi
	movl	$.LC214, %esi
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 152 [85.0%]  (CAN_FALLTHRU) 154 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1193
# BLOCK 154 seq:152
# PRED: 153 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movq	%rdx, %rdi
	movl	$.LC215, %esi
	call	strstr
	movq	32(%rsp), %rdx
	testq	%rax, %rax
# SUCC: 152 [85.0%]  (CAN_FALLTHRU) 155 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	jne	.L1193
# BLOCK 155 seq:153
# PRED: 154 [15.0%]  (FALLTHRU,CAN_FALLTHRU)
	movl	$.LC216, %esi
	movq	%rdx, %rdi
	call	strstr
	testq	%rax, %rax
# SUCC: 152 [85.0%]  (CAN_FALLTHRU) 156 [15.0%]  (FALLTHRU)
	jne	.L1193
# BLOCK 156 seq:154
# PRED: 155 [15.0%]  (FALLTHRU)
# SUCC: 90 [100.0%] 
	jmp	.L1175
	.cfi_endproc
.LFE85:
	.size	UFDBsslPeekServer, .-UFDBsslPeekServer
	.section	.rodata.str1.1
.LC245:
	.string	"SSL_ERROR_NONE"
.LC246:
	.string	"SSL_ERROR_SSL"
.LC247:
	.string	"SSL_ERROR_WANT_READ"
.LC248:
	.string	"SSL_ERROR_WANT_WRITE"
.LC249:
	.string	"SSL_ERROR_WANT_X509_LOOKUP"
.LC250:
	.string	"SSL_ERROR_SYSCALL"
.LC251:
	.string	"SSL_ERROR_ZERO_RETURN"
.LC252:
	.string	"SSL_ERROR_WANT_CONNECT"
.LC253:
	.string	"SSL_ERROR_WANT_ACCEPT"
	.section	.rodata
	.align 32
	.type	CSWTCH.218, @object
	.size	CSWTCH.218, 72
CSWTCH.218:
	.quad	.LC245
	.quad	.LC246
	.quad	.LC247
	.quad	.LC248
	.quad	.LC249
	.quad	.LC250
	.quad	.LC251
	.quad	.LC252
	.quad	.LC253
	.section	.rodata.str1.1
.LC254:
	.string	"close notify"
.LC255:
	.string	"unexpected message"
.LC256:
	.string	"bad record mac"
.LC257:
	.string	"decryption failed"
.LC258:
	.string	"record overflow"
.LC259:
	.string	"decompression failure"
.LC260:
	.string	"handshake failure"
.LC261:
	.string	"no certificate"
.LC262:
	.string	"bad certificate"
.LC263:
	.string	"unsupported certificate"
.LC264:
	.string	"certificate revoked"
.LC265:
	.string	"certificate expired"
.LC266:
	.string	"certificate unknown"
.LC267:
	.string	"illegal parameter"
.LC268:
	.string	"unknown ca"
.LC269:
	.string	"access denied"
.LC270:
	.string	"decode error"
.LC271:
	.string	"decrypt error"
.LC272:
	.string	"export restriction"
.LC273:
	.string	"protocol version"
.LC274:
	.string	"insufficient security"
.LC275:
	.string	"internal error"
.LC276:
	.string	"user canceled"
.LC277:
	.string	"no renegotiation"
.LC278:
	.string	"unsupported extension"
.LC279:
	.string	"certificate unobtainable"
.LC280:
	.string	"unrecognized name"
	.section	.rodata.str1.8
	.align 8
.LC281:
	.string	"bad certificate status response"
	.section	.rodata.str1.1
.LC282:
	.string	"bad certificate hash value"
	.section	.rodata
	.align 32
	.type	CSWTCH.216, @object
	.size	CSWTCH.216, 920
CSWTCH.216:
	.quad	.LC254
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC255
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC256
	.quad	.LC257
	.quad	.LC258
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC259
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC260
	.quad	.LC261
	.quad	.LC262
	.quad	.LC263
	.quad	.LC264
	.quad	.LC265
	.quad	.LC266
	.quad	.LC267
	.quad	.LC268
	.quad	.LC269
	.quad	.LC270
	.quad	.LC271
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC272
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC273
	.quad	.LC274
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC275
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC276
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC277
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC21
	.quad	.LC278
	.quad	.LC279
	.quad	.LC280
	.quad	.LC281
	.quad	.LC282
	.local	didwait.21603
	.comm	didwait.21603,4,4
	.section	.rodata.str1.1
.LC283:
	.string	"0.4.0.2042.1.5"
.LC284:
	.string	"1.2.40.0.17.1.22"
.LC285:
	.string	"1.2.392.200091.100.721.1"
.LC286:
	.string	"1.3.6.1.4.1.311.60.2.1.3"
.LC287:
	.string	"1.3.6.1.4.1.782.1.2.1.8.1"
.LC288:
	.string	"1.3.6.1.4.1.4146.1.1"
.LC289:
	.string	"1.3.6.1.4.1.6449.1.2.1.5.1"
.LC290:
	.string	"1.3.6.1.4.1.6334.1.100.1"
.LC291:
	.string	"1.3.6.1.4.1.8024.0.2.100.1.2"
.LC292:
	.string	"1.3.6.1.4.1.13177.10.1.3.10"
.LC293:
	.string	"1.3.6.1.4.1.14370.1.6"
.LC294:
	.string	"1.3.6.1.4.1.14777.6.1.1"
.LC295:
	.string	"1.3.6.1.4.1.17326.10.14.2.1.2"
.LC296:
	.string	"1.3.6.1.4.1.17326.10.8.12.1.2"
.LC297:
	.string	"1.3.6.1.4.1.22234.2.5.2.3.1"
.LC298:
	.string	"1.3.6.1.4.1.23223.2"
.LC299:
	.string	"1.3.6.1.4.1.23223.1.1.1"
.LC300:
	.string	"1.3.6.1.4.1.34697.2.1"
.LC301:
	.string	"1.3.6.1.4.1.34697.2.2"
.LC302:
	.string	"1.3.6.1.4.1.34697.2.3"
.LC303:
	.string	"1.3.6.1.4.1.34697.2.4"
.LC304:
	.string	"1.3.6.1.4.1.36305.2"
.LC305:
	.string	"2.16.578.1.26.1.3.3"
.LC306:
	.string	"2.16.756.1.83.21.0"
.LC307:
	.string	"2.16.756.1.89.1.2.1.1"
.LC308:
	.string	"2.16.792.1.2.1.1.5.7.1.9"
.LC309:
	.string	"2.16.792.3.0.4.1.1.4"
.LC310:
	.string	"2.16.840.1.114412.1.3.0.2"
.LC311:
	.string	"2.16.840.1.114412.2.1"
.LC312:
	.string	"2.16.840.1.114028.10.1.2"
.LC313:
	.string	"2.16.840.1.114413.1.7.23.3"
.LC314:
	.string	"2.16.840.1.114414.1.7.23.3"
.LC315:
	.string	"2.16.840.1.113733.1.7.48.1"
.LC316:
	.string	"2.16.840.1.113733.1.7.23.6"
.LC317:
	.string	"2.16.840.1.114171.500.9"
.LC318:
	.string	"2.16.840.1.114404.1.1.2.4.1"
	.section	.rodata
	.align 32
	.type	knownEVcertIssuerOIDTable, @object
	.size	knownEVcertIssuerOIDTable, 304
knownEVcertIssuerOIDTable:
	.quad	.LC66
	.quad	.LC283
	.quad	.LC284
	.quad	.LC285
	.quad	.LC286
	.quad	.LC287
	.quad	.LC288
	.quad	.LC289
	.quad	.LC290
	.quad	.LC291
	.quad	.LC292
	.quad	.LC293
	.quad	.LC294
	.quad	.LC295
	.quad	.LC296
	.quad	.LC297
	.quad	.LC298
	.quad	.LC299
	.quad	.LC300
	.quad	.LC301
	.quad	.LC302
	.quad	.LC303
	.quad	.LC304
	.quad	.LC305
	.quad	.LC306
	.quad	.LC307
	.quad	.LC308
	.quad	.LC309
	.quad	.LC310
	.quad	.LC311
	.quad	.LC312
	.quad	.LC313
	.quad	.LC314
	.quad	.LC315
	.quad	.LC316
	.quad	.LC317
	.quad	.LC318
	.quad	0
	.local	OldestInHTTPScache
	.comm	OldestInHTTPScache,8,32
	.local	lastHTTPScachePurgeTime
	.comm	lastHTTPScachePurgeTime,8,32
	.local	ufdbCacertsLoaded
	.comm	ufdbCacertsLoaded,4,32
	.local	myht
	.comm	myht,8,32
	.local	crypto_mutexes
	.comm	crypto_mutexes,8,32
	.local	num_static_ssl_locks
	.comm	num_static_ssl_locks,4,32
	.local	ssl_ctx
	.comm	ssl_ctx,8,32
	.local	init_mutex
	.comm	init_mutex,40,32
	.local	tls_inited
	.comm	tls_inited,4,4
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-16)"
	.section	.note.GNU-stack,"",@progbits
