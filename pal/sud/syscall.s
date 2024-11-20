.global syscall_start
syscall_start:
	mov %r8, %r10
	mov %r9, %r8
	mov %rdi, %rax
	mov %rsi, %rdi
	mov %rdx, %rsi
	mov %rcx, %rdx
	mov $0, %r9
	syscall
	ret
.global syscall_end
syscall_end:

.global rt_restore
rt_restore:
	mov $15, %rax
	syscall
	nop
.p2align 3
.global rt_restore_size
rt_restore_size:
	.quad .-rt_restore
