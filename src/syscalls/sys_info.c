#include <stdalign.h>

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
    strncpy(uts->sysname, "Linux", UTSNAME_LENGTH);
    strncpy(uts->release, "6.0.0-libtux", UTSNAME_LENGTH);
    uts->nodename[0] = 0;
    uts->version[0] = 0;
    uts->machine[0] = 0;
    return 0;
}
