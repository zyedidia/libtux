# libtux

libtux is an embeddable implementation of the Linux API. With libtux, you can
run Linux binaries inside of your application. The library is organized such
that it can be used with different isolation mechanisms and underlying host
platforms. Currently, the PAL (platform abstraction layer) provides the
isolation mechanism (see `src/include/tux_pal.h`), and the remaining
non-portable functions needed by libtux are provided by the host backend (see
`src/host.h`).

Libtux is very in-progress currently and cannot run many programs yet.
