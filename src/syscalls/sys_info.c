#include <stdalign.h>

#include "config.h"

#include "syscalls/syscalls.h"

enum {
    UTSNAME_LENGTH = 65,
};

struct UTSName {
    char sysname[UTSNAME_LENGTH];
    char nodename[UTSNAME_LENGTH];
    char release[UTSNAME_LENGTH];
    char version[UTSNAME_LENGTH];
    char machine[UTSNAME_LENGTH];
};

int
sys_uname(struct TuxProc* p, asuserptr_t bufp)
{
    uint8_t* utsb = procbufalign(p, bufp, sizeof(struct UTSName), alignof(struct UTSName));
    if (!utsb)
        return -TUX_EFAULT;
    struct UTSName* uts = (struct UTSName*) utsb;
    strcpy(uts->sysname, "Linux");
    strcpy(uts->release, LINUX_VERSION "-libtux");
    strcpy(uts->release, "#XXX");
    strcpy(uts->machine, "x86_64");
    return 0;
}
