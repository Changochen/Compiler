	.text
	.file	"Program"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %main
	cmpl	$232, k(%rip)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # %body
                                        # =>This Inner Loop Header: Depth=1
	incl	k(%rip)
	cmpl	$232, k(%rip)
	jle	.LBB0_2
.LBB0_3:                                # %next
	movl	k(%rip), %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	k,@object               # @k
	.comm	k,4,4

	.section	".note.GNU-stack","",@progbits
