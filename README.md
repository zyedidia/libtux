# libtux

libtux is an embeddable implementation of the Linux API. With libtux, you can
run Linux binaries inside of your application. The library is written such that
the underlying platform abstraction is provided by a PAL (platform abstraction
layer), and all other functionality within libtux is implemented using only
portable, standard C code. The PAL provides mechanisms for creating address
spaces and execution contexts, and allows libtux to run in a variety of
environments with different isolation mechanisms.

Each platform consists of a host and an isolation mechanism that can be used by
the host. We plan to suppport PALs for the following host/isolation pairs.

* Linux/LFI, macOS/LFI, Windows/LFI.
* Linux/Revisor (KVM).
* Linux/ptrace.
* Linux/SUD (no isolation is provided by SUD, this mode is meant for non-secure
  applications or debugging).
* Baremetal/paging.

libtux only supports x86-64 and aarch64.
