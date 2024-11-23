#define N_RTCALLS 256
#define PROC_REGS 16

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
.globl lfi_restore_regs
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

.global sud_rt_restore
sud_rt_restore:
	mov $15, %rax
	syscall
	nop
.p2align 3
.global sud_rt_restore_size
sud_rt_restore_size:
	.quad .-sud_rt_restore
