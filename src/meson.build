srcs = files(
  'tux.c',
  'buf.c',
  'proc.c',
  'elf.c',
  'sys.c',
)

subdir('syscalls')
subdir('arch/' + cpuarch)

libtux = static_library(
  'tux',
  srcs,
  c_args: c_args_hardened,
  include_directories: include_directories('include', 'arch/' + cpuarch),
  install: true,
)

install_headers('include/tux.h')

libtux_dep = declare_dependency(link_with: libtux, include_directories: include_directories('include'))
