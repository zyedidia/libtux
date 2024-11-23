#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "arch_regs.h"

#include "buf.h"
#include "proc.h"
#include "elf.h"

static bool procsetup(struct TuxProc* p, uint8_t* prog, size_t progsz, uint8_t* interp, size_t interpsz, int argc, char** argv);
static bool procfile(struct TuxProc* p, uint8_t* prog, size_t progsz, int argc, char** argv);
static void procfree(struct TuxProc*);

static struct TuxProc*
procnewempty(void)
{
    struct TuxProc* p = calloc(sizeof(struct TuxProc), 1);
    if (!p)
        return NULL;
    return p;
}

static struct TuxProc*
procnewfile(struct Tux* tux, uint8_t* prog, size_t size, int argc, char** argv)
{
    struct TuxProc* p = procnewempty();
    if (!p)
        return NULL;
    p->tux = tux;
    struct PlatAddrSpace* as = pal_as_new(tux->plat);
    if (!as)
        goto err1;
    struct PlatContext* ctx = pal_ctx_new(tux->plat, p);
    if (!ctx)
        goto err2;
    p->p_as = as;
    p->p_info = pal_as_info(as);
    p->p_ctx = ctx;

    if (!procfile(p, prog, size, argc, argv))
        goto err3;

    // TODO: initialize fdtable

    return p;
err3:
    pal_ctx_free(ctx);
err2:
    pal_as_free(as);
err1:
    procfree(p);
    return NULL;
}

static bool
procfile(struct TuxProc* p, uint8_t* prog, size_t progsz, int argc, char** argv)
{
    char* interppath = elfinterp(prog, progsz);
    buf_t interp = (buf_t){NULL, 0};
    if (interppath) {
        interp = bufreadfile(interppath);
        if (!interp.data) {
            fprintf(stderr, "error opening dynamic linker %s: %s\n", interppath, strerror(errno));
            free(interppath);
            return false;
        }
        free(interppath);
    }

    bool success = true;
    if (!procsetup(p, prog, progsz, interp.data, interp.size, argc, argv))
        success = false;

    return success;
}

enum {
    ARGC_MAX = 1024,
    ARGV_MAX = 1024,

    ARG_BLOCK = 4096,
};

static bool
stacksetup(struct TuxProc* p, int argc, char** argv, struct ELFLoadInfo* info, asptr_t* newsp)
{
#ifdef PAL_EXTERNAL_ADDRSPACE
#error "TODO: platforms that require external address spaces are currently unsupported"
#endif

    char* argv_ptrs[ARGC_MAX];
    char* stack_top = (char*) info->stack + info->stacksize;
    char* p_argv = (char*) stack_top - ARG_BLOCK;

    // Write argv string values to the stack.
    for (int i = 0; i < argc; i++) {
        size_t len = strnlen(argv[i], ARGV_MAX) + 1;

        if (p_argv + len >= stack_top) {
            return false;
        }

        memcpy(p_argv, argv[i], len);
        p_argv[len - 1] = 0;
        argv_ptrs[i] = p_argv;
        p_argv += len;
    }

    // Write argc and argv pointers to the stack.
    long* p_argc = (long*) (stack_top - 2 * ARG_BLOCK);
    *newsp = (asptr_t) p_argc;
    *p_argc++ = argc;
    char** p_argvp = (char**) p_argc;
    for (int i = 0; i < argc; i++) {
        if ((uintptr_t) p_argvp >= (uintptr_t) stack_top - ARG_BLOCK) {
            return false;
        }
        p_argvp[i] = (char*) pal_as_p2user(p->p_as, argv_ptrs[i]);
    }
    p_argvp[argc] = NULL;
    // Empty envp.
    char** p_envp = (char**) &p_argvp[argc + 1];
    *p_envp++ = NULL;

    struct Auxv* av = (struct Auxv*) p_envp;
    *av++ = (struct Auxv) { AT_SECURE, 0 };
    *av++ = (struct Auxv) { AT_BASE, info->ldbase };
    *av++ = (struct Auxv) { AT_PHDR, info->elfbase + info->elfphoff };
    *av++ = (struct Auxv) { AT_PHNUM, info->elfphnum };
    *av++ = (struct Auxv) { AT_PHENT, info->elfphentsize };
    *av++ = (struct Auxv) { AT_ENTRY, info->elfentry };
    *av++ = (struct Auxv) { AT_EXECFN, pal_as_p2user(p->p_as, p_argvp[0]) };
    *av++ = (struct Auxv) { AT_PAGESZ, getpagesize() };
    *av++ = (struct Auxv) { AT_HWCAP, 0 };
    *av++ = (struct Auxv) { AT_HWCAP2, 0 };
    *av++ = (struct Auxv) { AT_RANDOM, info->elfentry }; // TODO
    *av++ = (struct Auxv) { AT_FLAGS, 0 };
    *av++ = (struct Auxv) { AT_UID, 1000 };
    *av++ = (struct Auxv) { AT_EUID, 1000 };
    *av++ = (struct Auxv) { AT_GID, 1000 };
    *av++ = (struct Auxv) { AT_EGID, 1000 };
    *av++ = (struct Auxv) { AT_NULL, 0 };

    return true;
}

static bool
procsetup(struct TuxProc* p, uint8_t* prog, size_t progsz, uint8_t* interp, size_t interpsz, int argc, char** argv)
{
    struct ELFLoadInfo info = {0};
    bool b = elfload(p, prog, progsz, interp, interpsz, &info);
    if (!b)
        return false;

    asptr_t sp;
    if (!stacksetup(p, argc, argv, &info, &sp))
        return false;

    asptr_t entry = info.elfentry;
    if (interp != NULL)
        entry = info.ldentry;

    struct TuxRegs regs;
    regs_init(&regs, entry, sp);
    *pal_ctx_regs(p->p_ctx) = regs;

    p->brkbase = info.lastva;
    p->brksize = 0;

    // Reserve the brk region.
    const int mapflags = PAL_MAP_PRIVATE | PAL_MAP_ANONYMOUS;
    asptr_t brkregion = pal_as_mapat(p->p_as, p->brkbase, BRKMAXSIZE, PAL_PROT_NONE, mapflags, -1, 0);
    if (brkregion == (asptr_t) -1)
        return false;

    return true;
}

static void
procfree(struct TuxProc* proc)
{
    (void) proc;
    // TODO: free proc
}

struct TuxProc*
tux_proc_newfile(struct Tux* tux, uint8_t* prog, size_t progsz, int argc, char** argv)
{
    return procnewfile(tux, prog, progsz, argc, argv);
}
