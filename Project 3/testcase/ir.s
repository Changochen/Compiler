	.text
	.file	"Program"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %main
	pushq	%rax
	.cfi_def_cfa_offset 16
	cmpl	$511, k(%rip)           # imm = 0x1FF
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # %body
                                        # =>This Inner Loop Header: Depth=1
	incl	k(%rip)
	cmpl	$511, k(%rip)           # imm = 0x1FF
	jle	.LBB0_2
.LBB0_3:                                # %next
	movl	k(%rip), %eax
	movl	%eax, output(%rip)
	callq	writeint32
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	k,@object               # @k
	.comm	k,4,4

	.section	".note.GNU-stack","",@progbits