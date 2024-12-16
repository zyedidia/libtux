#define N_RTCALLS 256
#define PROC_REGS 16

.p2align 4
.global pal_ctx_entry
pal_ctx_entry:
	int3

.p2align 4
.global pal_asm_ctx_exit
pal_asm_ctx_exit:
	int3

.p2align 4
.global pal_get_tp
pal_get_tp:
	int3

.p2align 4
.global pal_set_tp
pal_set_tp:
	int3

.p2align 4
.global pal_syscall_entry
pal_syscall_entry:
	int3
