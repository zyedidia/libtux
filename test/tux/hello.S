#include <sys/syscall.h>

.global _start
_start:
	mov $SYS_write,%eax
	mov $1, %rdi
	lea hello(%rip), %rsi
	mov $6, %rdx
	syscall

	mov $SYS_exit,%eax
	mov $42, %rdi
	syscall

.data

hello:
	.string "hello\n"
