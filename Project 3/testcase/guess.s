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
	movl	$0, res(%rip)
	movl	$563, guess(%rip)       # imm = 0x233
	jmp	.LBB0_1
	.p2align	4, 0x90
.LBB0_2:                                # %body
                                        #   in Loop: Header=BB0_1 Depth=1
	callq	readint32
	movl	%eax, res(%rip)
.LBB0_1:                                # %con
                                        # =>This Inner Loop Header: Depth=1
	movl	guess(%rip), %eax
	cmpl	res(%rip), %eax
	jne	.LBB0_2
# %bb.3:                                # %next
	movl	guess(%rip), %eax
	movl	%eax, output(%rip)
	callq	writeint32
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	guess,@object           # @guess
	.comm	guess,4,4
	.type	res,@object             # @res
	.comm	res,4,4

	.section	".note.GNU-stack","",@progbits
