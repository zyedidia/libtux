#define N_RTCALLS 256
#define PROC_REGS 16

// pal_ctx_entry(PlatContext* ctx, void** kstackp)
.p2align 4
.global pal_ctx_entry
pal_ctx_entry:
	// save callee-saved registers to stack
	pushq %r15
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %rbx
	pushq %rbp
	// save stack to kstackp
	mov %rsp, (%rsi)
	jmp pal_restore_regs
	int3

.p2align 4
.global pal_restore_regs
pal_restore_regs:
	mov PROC_REGS+8*18(%rdi), %r11
	wrgsbase %r11
	mov PROC_REGS+8*0(%rdi), %rsp
	mov PROC_REGS+8*1(%rdi), %rax
	mov PROC_REGS+8*2(%rdi), %rcx
	mov PROC_REGS+8*3(%rdi), %rdx
	mov PROC_REGS+8*4(%rdi), %rbx
	mov PROC_REGS+8*5(%rdi), %rbp
	mov PROC_REGS+8*6(%rdi), %rsi
	mov PROC_REGS+8*8(%rdi), %r8
	mov PROC_REGS+8*9(%rdi), %r9
	mov PROC_REGS+8*10(%rdi), %r10
	mov PROC_REGS+8*11(%rdi), %r11
	mov PROC_REGS+8*12(%rdi), %r12
	mov PROC_REGS+8*13(%rdi), %r13
	mov PROC_REGS+8*14(%rdi), %r14
	mov PROC_REGS+8*15(%rdi), %r15
	mov PROC_REGS+8*7(%rdi), %rdi
	ret

.p2align 4
.global pal_sigsys_return
pal_sigsys_return:
	mov PROC_REGS+8*16(%rdi), %r11
	wrfsbase %r11
	mov PROC_REGS+8*17(%rdi), %r11
	wrgsbase %r11
	mov PROC_REGS+8*0(%rdi), %rsp
	mov PROC_REGS+8*1(%rdi), %rax
	mov PROC_REGS+8*2(%rdi), %rcx
	mov PROC_REGS+8*3(%rdi), %rdx
	mov PROC_REGS+8*4(%rdi), %rbx
	mov PROC_REGS+8*5(%rdi), %rbp
	mov PROC_REGS+8*6(%rdi), %rsi
	mov PROC_REGS+8*8(%rdi), %r8
	mov PROC_REGS+8*9(%rdi), %r9
	mov PROC_REGS+8*10(%rdi), %r10
	mov PROC_REGS+8*11(%rdi), %r11
	mov PROC_REGS+8*12(%rdi), %r12
	mov PROC_REGS+8*13(%rdi), %r13
	mov PROC_REGS+8*14(%rdi), %r14
	mov PROC_REGS+8*15(%rdi), %r15

	movq PROC_REGS+18*8+16*0(%rdi), %xmm0
	movq PROC_REGS+18*8+16*1(%rdi), %xmm1
	movq PROC_REGS+18*8+16*2(%rdi), %xmm2
	movq PROC_REGS+18*8+16*3(%rdi), %xmm3
	movq PROC_REGS+18*8+16*4(%rdi), %xmm4
	movq PROC_REGS+18*8+16*5(%rdi), %xmm5
	movq PROC_REGS+18*8+16*6(%rdi), %xmm6
	movq PROC_REGS+18*8+16*7(%rdi), %xmm7
	movq PROC_REGS+18*8+16*8(%rdi), %xmm8
	movq PROC_REGS+18*8+16*9(%rdi), %xmm9
	movq PROC_REGS+18*8+16*10(%rdi), %xmm10
	movq PROC_REGS+18*8+16*11(%rdi), %xmm11
	movq PROC_REGS+18*8+16*12(%rdi), %xmm12
	movq PROC_REGS+18*8+16*13(%rdi), %xmm13
	movq PROC_REGS+18*8+16*14(%rdi), %xmm14
	movq PROC_REGS+18*8+16*15(%rdi), %xmm15

	mov PROC_REGS+8*7(%rdi), %rdi
	jmpq *%rcx

// pal_asm_ctx_exit(void* kstackp, int code)
.p2align 4
.global pal_asm_ctx_exit
pal_asm_ctx_exit:
	movq %rdi, %rsp
	movq %rsi, %rax
	popq %rbp
	popq %rbx
	popq %r12
	popq %r13
	popq %r14
	popq %r15
	ret

.p2align 4
.global pal_ctx_xmmsave
pal_ctx_xmmsave:
	movq %xmm0, 18*8+16*0(%rdi)
	movq %xmm1, 18*8+16*1(%rdi)
	movq %xmm2, 18*8+16*2(%rdi)
	movq %xmm3, 18*8+16*3(%rdi)
	movq %xmm4, 18*8+16*4(%rdi)
	movq %xmm5, 18*8+16*5(%rdi)
	movq %xmm6, 18*8+16*6(%rdi)
	movq %xmm7, 18*8+16*7(%rdi)
	movq %xmm8, 18*8+16*8(%rdi)
	movq %xmm9, 18*8+16*9(%rdi)
	movq %xmm10, 18*8+16*10(%rdi)
	movq %xmm11, 18*8+16*11(%rdi)
	movq %xmm12, 18*8+16*12(%rdi)
	movq %xmm13, 18*8+16*13(%rdi)
	movq %xmm14, 18*8+16*14(%rdi)
	movq %xmm15, 18*8+16*15(%rdi)
	ret

.p2align 4
.global sud_rt_restore
sud_rt_restore:
	mov $15, %rax
	syscall
	nop
.p2align 3
.global sud_rt_restore_size
sud_rt_restore_size:
	.quad .-sud_rt_restore
