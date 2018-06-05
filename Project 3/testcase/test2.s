	.text
	.file	"Program"
	.globl	square                  # -- Begin function square
	.p2align	4, 0x90
	.type	square,@function
square:                                 # @square
	.cfi_startproc
# %bb.0:                                # %square
	movl	res(%rip), %eax
	imull	%eax, %eax
	retq
.Lfunc_end0:
	.size	square, .Lfunc_end0-square
	.cfi_endproc
                                        # -- End function
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %main
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$1, res(%rip)
	cmpl	$31, res(%rip)
	jg	.LBB1_3
	.p2align	4, 0x90
.LBB1_2:                                # %body
                                        # =>This Inner Loop Header: Depth=1
	callq	square
	movl	%eax, output(%rip)
	callq	writeint32
	incl	res(%rip)
	cmpl	$31, res(%rip)
	jle	.LBB1_2
.LBB1_3:                                # %next
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.type	res,@object             # @res
	.comm	res,4,4

	.section	".note.GNU-stack","",@progbits
